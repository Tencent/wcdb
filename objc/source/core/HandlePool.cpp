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
#include <WCDB/CoreConst.h>
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
HandlePool::HandlePool(const String &thePath) : path(thePath)
{
}

HandlePool::~HandlePool()
{
    //wait until all handles back.
    drain(nullptr);
}

#pragma mark - Concurrency
int HandlePool::maxAllowedNumberOfHandles()
{
    static const int s_maxAllowedNumberOfHandles = std::max<int>(
    HandlePooMaxAllowedNumberOfHandles, std::thread::hardware_concurrency());
    return s_maxAllowedNumberOfHandles;
}

bool HandlePool::isNumberOfHandlesAllowed()
{
    WCTInnerAssert(m_concurrency.readSafety());
    WCTInnerAssert(m_memory.readSafety());
    return numberOfAliveHandles() < maxAllowedNumberOfHandles();
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
    if (onDrained != nullptr) {
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
            handle->close();
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
            handle->close();
            handles.erase(handle);
        }
    }
    m_frees.clear();
}

size_t HandlePool::numberOfAliveHandles() const
{
    SharedLockGuard concurrencyGuard(m_concurrency);
    SharedLockGuard memoryGuard(m_memory);
    int count = 0;
    for (const auto &iter : m_handles) {
        count += iter.second.size();
    }
    return count;
}

size_t HandlePool::numberOfActiveHandles(Slot slot) const
{
    SharedLockGuard concurrencyGuard(m_concurrency);
    SharedLockGuard memoryGuard(m_memory);
    size_t numberOfHandles = 0;
    size_t numberOfFreeHandles = 0;
    {
        auto iter = m_handles.find(slot);
        if (iter != m_handles.end()) {
            numberOfHandles = iter->second.size();
        }
    }
    {
        auto iter = m_frees.find(slot);
        if (iter != m_frees.end()) {
            numberOfFreeHandles = iter->second.size();
        }
    }
    WCTInnerAssert(numberOfHandles >= numberOfFreeHandles);
    return numberOfHandles - numberOfFreeHandles;
}

bool HandlePool::isAliving() const
{
    SharedLockGuard concurrencyGuard(m_concurrency);
    SharedLockGuard memoryGuard(m_memory);
    bool aliving = false;
    for (const auto &iter : m_handles) {
        if (iter.second.size() > 0) {
            aliving = true;
            break;
        }
    }
    return aliving;
}

RecyclableHandle HandlePool::flowOut(Slot slot)
{
    SharedLockGuard concurrencyGuard(m_concurrency);
    std::shared_ptr<Handle> handle;
    {
        LockGuard memoryGuard(m_memory);
        auto &freeSlot = m_frees[slot];
        if (!freeSlot.empty()) {
            handle = freeSlot.back();
            WCTInnerAssert(handle != nullptr);
            freeSlot.pop_back();
        } else if (!isNumberOfHandlesAllowed()) {
            // auto purge to remove unused handles
            purge();
            if (!isNumberOfHandlesAllowed()) {
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

    bool isGenerated = false;
    if (handle == nullptr) {
        // generate new handle
        // lock free
        handle = generateHandle(slot);
        if (handle == nullptr) {
            return nullptr;
        }
        isGenerated = true;
    }

    WCTInnerAssert(handle != nullptr);
    bool failed = false;
    do {
        // configuration
        if (!willConfigureHandle(slot, handle.get())) {
            failed = true;
            break;
        }

        if (isGenerated) {
            LockGuard memoryGuard(m_memory);
            // re-check handle count limitation since all lock free code above
            if (!isNumberOfHandlesAllowed()) {
                purge();
                if (!isNumberOfHandlesAllowed()) {
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
            WCTInnerAssert(m_handles[slot].find(handle) == m_handles[slot].end());
            m_handles[slot].emplace(handle);
        }
    } while (false);
    if (failed) {
        handle->close();
        if (!isGenerated) {
            LockGuard memoryGuard(m_memory);
            // remove if it already exists in handles
            m_handles[slot].erase(handle);
        }
        return nullptr;
    }

    m_concurrency.lockShared();
    return RecyclableHandle(
    handle, std::bind(&HandlePool::flowBack, this, slot, std::placeholders::_1));
}

bool HandlePool::handlesExist(Slot slot) const
{
    WCTInnerAssert(m_concurrency.readSafety());
    WCTInnerAssert(m_memory.readSafety());
    return m_handles.find(slot) != m_handles.end();
}

const std::set<std::shared_ptr<Handle>> &HandlePool::getHandles(Slot slot) const
{
    WCTInnerAssert(m_concurrency.readSafety());
    WCTInnerAssert(m_memory.readSafety());
    WCTInnerAssert(handlesExist(slot));
    return m_handles.find(slot)->second;
}

void HandlePool::flowBack(Slot slot, const std::shared_ptr<Handle> &handle)
{
    WCTInnerAssert(handle != nullptr);
    WCTInnerAssert(m_concurrency.readSafety());
    WCTRemedialAssert(
    !handle->isPrepared(), "Statement is not finalized.", handle->finalize(););
    {
        LockGuard memoryGuard(m_memory);
        m_frees[slot].push_back(handle);
    }
    m_concurrency.unlockShared();
}

} //namespace WCDB
