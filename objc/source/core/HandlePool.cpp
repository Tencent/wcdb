/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <WCDB/Assertion.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/HandlePool.hpp>
#include <WCDB/Notifier.hpp>
#include <WCDB/Path.hpp>
#include <WCDB/Serialization.hpp>
#include <algorithm>
#include <map>
#include <thread>

namespace WCDB {

#pragma mark - Initialize
HandlePool::HandlePool(const String &thePath)
: path(thePath), m_configs(nullptr)
{
}

HandlePool::~HandlePool()
{
    //wait until all handles back.
    drain(nullptr);
}

#pragma mark - Config
void HandlePool::setConfigs(const std::shared_ptr<Configs> &configs)
{
    LockGuard lockGuard(m_lock);
    m_configs = configs;
}

void HandlePool::setConfig(const String &name, const std::shared_ptr<Config> &config, int priority)
{
    LockGuard lockGuard(m_lock);
    std::shared_ptr<Configs> configs(new Configs(*m_configs.get()));
    configs->insert(name, config, priority);
    m_configs = configs;
}

void HandlePool::removeConfig(const String &name)
{
    LockGuard lockGuard(m_lock);
    std::shared_ptr<Configs> configs(new Configs(*m_configs.get()));
    configs->remove(name);
    m_configs = configs;
}

#pragma mark - Concurrency
int HandlePool::hardwareConcurrency()
{
    static const int s_hardwareConcurrency
    = std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 8;
    return s_hardwareConcurrency;
}

int HandlePool::maxConcurrency()
{
    static int s_maxConcurrency = std::max<int>(hardwareConcurrency(), 64);
    return s_maxConcurrency;
}

bool HandlePool::allowedConcurrent()
{
    WCTInnerAssert(m_concurrency.readSafety());
    WCTInnerAssert(m_lock.readSafety());
    if (m_handles.size() < maxConcurrency()) {
        return true;
    }
    Error error;
    error.setCode(Error::Code::Exceed);
    error.message = "The concurrency of database exceeds the maximum allowed.";
    error.infos.set("Path", path);
    error.infos.set("MaxConcurrency", maxConcurrency());
    error.infos.set("Concurrency", m_handles.size());
    Notifier::shared()->notify(error);
    setThreadedError(std::move(error));
    return false;
}

void HandlePool::blockade()
{
    m_concurrency.lock();
}

void HandlePool::unblockade()
{
    WCTRemedialAssert(
    isBlockaded(), "Unblockade should not be called without blockaded.", return;);
    m_concurrency.unlock();
}

bool HandlePool::isBlockaded() const
{
    return m_concurrency.level() == SharedLock::Level::Write;
}

void HandlePool::drain(const HandlePool::DrainedCallback &onDrained)
{
    LockGuard lockConcurrencyGuard(m_concurrency);
    {
        LockGuard lockGuard(m_lock);
        WCTInnerAssert(m_frees.size() == m_handles.size());
        for (const auto &handle : m_handles) {
            handle->get()->close();
        }
        m_handles.clear();
        m_frees.clear();
    }
    if (onDrained) {
        onDrained();
    }
}

bool HandlePool::isDrained() const
{
    SharedLockGuard lockConcurrencyGuard(m_concurrency);
    SharedLockGuard lockGuard(m_lock);
    return m_handles.size() == 0;
}

#pragma mark - Handle
void HandlePool::purge()
{
    SharedLockGuard lockConcurrencyGuard(m_concurrency);
    LockGuard lockGuard(m_lock);
    for (const auto &handle : m_frees) {
        handle->get()->close();
        m_handles.erase(handle);
    }
    m_frees.clear();
}

size_t HandlePool::aliveHandleCount() const
{
    SharedLockGuard lockConcurrencyGuard(m_concurrency);
    SharedLockGuard lockGuard(m_lock);
    return m_handles.size();
}

RecyclableHandle HandlePool::flowOut()
{
    SharedLockGuard lockConcurrencyGuard(m_concurrency);
    std::shared_ptr<ConfiguredHandle> configuredHandle;
    {
        LockGuard lockGuard(m_lock);
        if (!m_frees.empty()) {
            configuredHandle = m_frees.back();
            WCTInnerAssert(configuredHandle != nullptr);
            m_frees.pop_back();
        } else if (!allowedConcurrent()) {
            // no free handle and concurrency reach the limitation.
            return nullptr;
        }
    }
    if (configuredHandle == nullptr) {
        // lock free
        std::shared_ptr<Handle> handle = generateHandle();
        if (handle == nullptr) {
            setThreadedError(Error(Error::Code::NoMemory));
            return nullptr;
        }
        if (!handle->open()) {
            setThreadedError(handle->getError());
            return nullptr;
        }

        configuredHandle.reset(new ConfiguredHandle(handle));
        if (configuredHandle == nullptr) {
            setThreadedError(Error(Error::Code::NoMemory));
            return nullptr;
        }
    }

    if (!handleWillConfigure(configuredHandle->get())) {
        return nullptr;
    }

    std::shared_ptr<Configs> configs;
    {
        SharedLockGuard lockGuard(m_lock);
        configs = m_configs;
    }
    if (!configuredHandle->configure(configs)) {
        setThreadedError(configuredHandle->get()->getError());
        return nullptr;
    }

    if (configuredHandle != nullptr) {
        {
            LockGuard lockGuard(m_lock);
            WCTInnerAssert(configuredHandle != nullptr);
            if (!allowedConcurrent()) {
                return nullptr;
            }
            m_handles.emplace(configuredHandle);
        }
        m_concurrency.lockShared();
        return RecyclableHandle(
        configuredHandle, std::bind(&HandlePool::flowBack, this, std::placeholders::_1));
    }
    return nullptr;
}

void HandlePool::flowBack(const std::shared_ptr<ConfiguredHandle> &configuredHandle)
{
    WCTInnerAssert(configuredHandle != nullptr);
    WCTInnerAssert(m_concurrency.readSafety());
    {
        LockGuard lockGuard(m_lock);
        if (m_handles.size() < hardwareConcurrency()) {
            m_frees.push_back(configuredHandle);
        } else {
            // drop it
            configuredHandle->get()->close();
            m_handles.erase(configuredHandle);
        }
    }
    m_concurrency.unlockShared();
}

} //namespace WCDB
