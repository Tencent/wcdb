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
    LockGuard memoryGuard(m_memory);
    m_configs = configs;
}

void HandlePool::setConfig(const String &name, const std::shared_ptr<Config> &config, int priority)
{
    LockGuard memoryGuard(m_memory);
    std::shared_ptr<Configs> configs(new Configs(*m_configs.get()));
    configs->insert(name, config, priority);
    m_configs = configs;
}

void HandlePool::removeConfig(const String &name)
{
    LockGuard memoryGuard(m_memory);
    std::shared_ptr<Configs> configs(new Configs(*m_configs.get()));
    configs->remove(name);
    m_configs = configs;
}

#pragma mark - Concurrency
int HandlePool::maxHandleCount()
{
    static const int s_maxHandleCount
    = std::max<int>(16, std::thread::hardware_concurrency());
    return s_maxHandleCount;
}

bool HandlePool::allowedHandleCount()
{
    WCTInnerAssert(m_concurrency.readSafety());
    WCTInnerAssert(m_memory.readSafety());
    return aliveHandleCount() < maxHandleCount();
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
    WCTRemedialAssert(m_concurrency.level() != SharedLock::Level::Read,
                      "There are some threaded handles not invalidated.",
                      return;);
    LockGuard concurrencyGuard(m_concurrency);
    LockGuard memoryGuard(m_memory);
    clearAllHandles();
    if (onDrained) {
        onDrained();
        // double-clear since there might be some operation inside the drained block.
        clearAllHandles();
    }
}

void HandlePool::clearAllHandles()
{
    WCTInnerAssert(m_concurrency.writeSafety());
    WCTInnerAssert(m_memory.writeSafety());
    for (const auto &iter : m_handles) {
        for (const auto &handle : iter.second) {
            handle->get()->close();
        }
    }
    m_handles.clear();
    m_frees.clear();
}

#pragma mark - Handle
void HandlePool::purge()
{
    SharedLockGuard concurrencyGuard(m_concurrency);
    LockGuard memoryGuard(m_memory);
    for (const auto &iter : m_frees) {
        auto &handles = m_handles[iter.first];
        for (const auto &handle : iter.second) {
            handle->get()->close();
            handles.erase(handle);
        }
    }
    m_frees.clear();
}

size_t HandlePool::aliveHandleCount() const
{
    SharedLockGuard concurrencyGuard(m_concurrency);
    SharedLockGuard memoryGuard(m_memory);
    int count = 0;
    for (const auto &iter : m_handles) {
        count += iter.second.size();
    }
    return count;
}

RecyclableHandle HandlePool::flowOut(const Slot &slot)
{
    SharedLockGuard concurrencyGuard(m_concurrency);
    bool isGenerated = false;
    std::shared_ptr<ConfiguredHandle> configuredHandle;
    {
        LockGuard memoryGuard(m_memory);
        auto &freeSlot = m_frees[slot];
        if (!freeSlot.empty()) {
            configuredHandle = freeSlot.back();
            WCTInnerAssert(configuredHandle != nullptr);
            freeSlot.pop_back();
        } else if (!allowedHandleCount()) {
            // auto purge to remove unused handles
            purge();
            if (!allowedHandleCount()) {
                // handle count reachs the limitation.
                Error error;
                error.setCode(Error::Code::Exceed);
                error.message = "The operating count of database exceeds the maximum allowed.";
                error.infos.set("Path", path);
                Notifier::shared()->notify(error);
                setThreadedError(std::move(error));
                return nullptr;
            }
        }
    }

    if (configuredHandle == nullptr) {
        // generate new handle
        // lock free
        std::shared_ptr<Handle> handle = generateHandle(slot);
        if (handle == nullptr) {
            return nullptr;
        }

        configuredHandle.reset(new ConfiguredHandle(handle));
        if (configuredHandle == nullptr) {
            setThreadedError(Error(Error::Code::NoMemory));
            return nullptr;
        }
        isGenerated = true;
    }

    WCTInnerAssert(configuredHandle != nullptr);
    bool failed = false;
    do {
        // configuration
        if (!willConfigureHandle(slot, configuredHandle.get())) {
            failed = true;
            break;
        }

        if (isGenerated) {
            LockGuard memoryGuard(m_memory);
            // re-check handle count limitation since all lock free code above
            if (!allowedHandleCount()) {
                purge();
                if (!allowedHandleCount()) {
                    // handle count reachs the limitation.
                    failed = true;
                    Error error;
                    error.setCode(Error::Code::Exceed);
                    error.message = "The operating count of database exceeds the maximum allowed.";
                    error.infos.set("Path", path);
                    Notifier::shared()->notify(error);
                    setThreadedError(std::move(error));
                    break;
                }
            }
            WCTInnerAssert(m_handles[slot].find(configuredHandle)
                           == m_handles[slot].end());
            m_handles[slot].emplace(configuredHandle);
        }
    } while (false);
    if (failed) {
        configuredHandle->get()->close();
        if (!isGenerated) {
            // remove if it already exists in handles
            m_handles[slot].erase(configuredHandle);
        }
        WCTInnerAssert(m_handles[slot].find(configuredHandle) == m_handles[slot].end());
        return nullptr;
    }

    m_concurrency.lockShared();
    return RecyclableHandle(
    configuredHandle, std::bind(&HandlePool::flowBack, this, slot, std::placeholders::_1));
}

bool HandlePool::willConfigureHandle(const Slot &slot, ConfiguredHandle *handle)
{
    std::shared_ptr<Configs> configs;
    {
        SharedLockGuard memoryGuard(m_memory);
        configs = m_configs;
    }
    if (handle->reconfigure(configs)) {
        return true;
    }
    setThreadedError(handle->get()->getError());
    return false;
}

void HandlePool::flowBack(const Slot &slot, const std::shared_ptr<ConfiguredHandle> &configuredHandle)
{
    WCTInnerAssert(configuredHandle != nullptr);
    WCTInnerAssert(m_concurrency.readSafety());
    WCTRemedialAssert(!configuredHandle->get()->isPrepared(),
                      "Statement is not finalized.",
                      configuredHandle->get()->finalize(););
    {
        LockGuard memoryGuard(m_memory);
        m_frees[slot].push_back(configuredHandle);
    }
    m_concurrency.unlockShared();
}

} //namespace WCDB
