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
#include <WCDB/Path.hpp>
#include <WCDB/Serialization.hpp>
#include <algorithm>
#include <map>
#include <thread>

namespace WCDB {

#pragma mark - Initialize
HandlePool::HandlePool(const std::string &thePath)
: path(thePath), m_configs(nullptr), m_aliveHandleCount(0)
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

void HandlePool::setConfig(const std::string &name, const std::shared_ptr<Config> &config, int priority)
{
    LockGuard lockGuard(m_lock);
    std::shared_ptr<Configs> configs(new Configs(*m_configs.get()));
    configs->insert(name, config, priority);
    m_configs = configs;
}

void HandlePool::removeConfig(const std::string &name)
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
    if (m_aliveHandleCount < maxConcurrency()) {
        return true;
    }
    Error error;
    error.setCode(Error::Code::Exceed);
    error.message = "The concurrency of database exceeds the maximum allowed.";
    error.infos.set("Path", path);
    error.infos.set("MaxConcurrency", maxConcurrency());
    error.infos.set("Concurrency", m_aliveHandleCount);
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
    purge();
    if (onDrained) {
        onDrained();
    }
}

bool HandlePool::isDrained() const
{
    SharedLockGuard lockGuard(m_lock);
    return m_aliveHandleCount == 0;
}

#pragma mark - Handle
void HandlePool::purge()
{
    SharedLockGuard lockConcurrencyGuard(m_concurrency);
    LockGuard lockGuard(m_lock);
    while (!m_handles.empty()) {
        m_handles.back()->get()->close();
        m_handles.pop_back();
        --m_aliveHandleCount;
    }
}

bool HandlePool::canFlowOut()
{
    SharedLockGuard lockConcurrencyGuard(m_concurrency);
    std::shared_ptr<ConfiguredHandle> configuredHandle = flowOutConfiguredHandle();
    if (configuredHandle == nullptr) {
        configuredHandle = generateConfiguredHandle();
    }
    if (configuredHandle != nullptr) {
        flowBackConfiguredHandle(configuredHandle);
        return true;
    }
    return false;
}

RecyclableHandle HandlePool::flowOut()
{
    SharedLockGuard lockConcurrencyGuard(m_concurrency);
    std::shared_ptr<ConfiguredHandle> configuredHandle = flowOutConfiguredHandle();
    if (configuredHandle == nullptr) {
        configuredHandle = generateConfiguredHandle();
    }
    if (configuredHandle != nullptr) {
        m_concurrency.lockShared();
        return RecyclableHandle(
        configuredHandle, std::bind(&HandlePool::flowBack, this, std::placeholders::_1));
    }
    return nullptr;
}

void HandlePool::flowBack(const std::shared_ptr<ConfiguredHandle> &configuredHandle)
{
    flowBackConfiguredHandle(configuredHandle);
    m_concurrency.unlockShared();
}

std::shared_ptr<ConfiguredHandle> HandlePool::flowOutConfiguredHandle()
{
    WCTInnerAssert(m_concurrency.readSafety());
    std::shared_ptr<ConfiguredHandle> configuredHandle = nullptr;
    std::shared_ptr<Configs> configs = nullptr;
    {
        LockGuard lockGuard(m_lock);
        if (m_handles.empty()) {
            return nullptr;
        }
        configuredHandle = m_handles.back();
        WCTInnerAssert(configuredHandle != nullptr);
        m_handles.pop_back();
        configs = m_configs;
    }
    if (!configuredHandle->configure(configs)) {
        setThreadedError(configuredHandle->get()->getError());
        return nullptr;
    }
    return configuredHandle;
}

std::shared_ptr<ConfiguredHandle> HandlePool::generateConfiguredHandle()
{
    WCTInnerAssert(m_concurrency.readSafety());
    std::shared_ptr<Configs> configs = nullptr;
    {
        SharedLockGuard lockGuard(m_lock);
        configs = m_configs;
        if (!allowedConcurrent()) {
            return nullptr;
        }
    }

    std::shared_ptr<Handle> handle = generateHandle();
    if (handle == nullptr) {
        setThreadedError(Error(Error::Code::NoMemory));
        return nullptr;
    }
    handle->setPath(path);
    if (!handle->open()) {
        setThreadedError(handle->getError());
        return nullptr;
    }

    std::shared_ptr<ConfiguredHandle> configuredHandle(new ConfiguredHandle(handle));
    if (configuredHandle == nullptr) {
        setThreadedError(Error(Error::Code::NoMemory));
        return nullptr;
    }
    if (!configuredHandle->configure(configs)) {
        setThreadedError(handle->getError());
        return nullptr;
    }
    {
        LockGuard lockGuard(m_lock);
        WCTInnerAssert(configuredHandle != nullptr);
        ++m_aliveHandleCount;
        if (!allowedConcurrent()) {
            --m_aliveHandleCount;
            return nullptr;
        }
    }
    return configuredHandle;
}

void HandlePool::flowBackConfiguredHandle(const std::shared_ptr<ConfiguredHandle> &configuredHandle)
{
    WCTInnerAssert(configuredHandle != nullptr);
    WCTInnerAssert(m_concurrency.readSafety());
    {
        handleWillFlowBack(configuredHandle->get());
        LockGuard lockGuard(m_lock);
        if (m_handles.size() < hardwareConcurrency()) {
            m_handles.push_back(configuredHandle);
        } else {
            // drop this handle
            --m_aliveHandleCount;
        }
    }
}

} //namespace WCDB
