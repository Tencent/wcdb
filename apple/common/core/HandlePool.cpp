//
// Created by sanhuazhang on 2019/05/02
//

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
#include <WCDB/InnerHandle.hpp>
#include <WCDB/Notifier.hpp>
#include <WCDB/Path.hpp>
#include <WCDB/Serialization.hpp>
#include <algorithm>
#include <map>

namespace WCDB {

#pragma mark - Initialize
HandlePool::HandlePool(const UnsafeStringView &thePath) : path(thePath)
{
}

HandlePool::~HandlePool()
{
    //wait until all handles back.
    drain(nullptr);
}

#pragma mark - Concurrency
bool HandlePool::isNumberOfHandlesAllowed() const
{
    WCTAssert(m_concurrency.readSafety());
    WCTAssert(m_memory.readSafety());
    return numberOfAliveHandles() < HandlePoolMaxAllowedNumberOfHandles;
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

void HandlePool::didDrain()
{
}

void HandlePool::drain(const HandlePool::DrainedCallback &onDrained)
{
    WCTRemedialAssert(m_concurrency.level() != SharedLock::Level::Read,
                      "There are some threaded handles not invalidated.",
                      return;);
    LockGuard concurrencyGuard(m_concurrency);
    LockGuard memoryGuard(m_memory);
    clearAllHandles();
    didDrain();
    if (onDrained != nullptr) {
        onDrained();
        // double-clear since there might be some operations inside the drained block.
        clearAllHandles();
        didDrain();
    }
}

void HandlePool::clearAllHandles()
{
    WCTAssert(m_concurrency.writeSafety());
    WCTAssert(m_memory.writeSafety());
    for (unsigned int i = 0; i < HandleSlotCount; ++i) {
        m_frees[i].clear();
        auto &handles = m_handles[i];
        for (const auto &handle : handles) {
            handle->close();
        }
        handles.clear();
    }
}

#pragma mark - Handle
void HandlePool::purge()
{
    SharedLockGuard concurrencyGuard(m_concurrency);
    LockGuard memoryGuard(m_memory);
    for (unsigned int i = 0; i < HandleSlotCount; ++i) {
        auto &handles = m_handles[i];
        auto &frees = m_frees[i];
        for (const auto &handle : frees) {
            handle->close();
            handles.erase(handle);
        }
        frees.clear();
    }
}

size_t HandlePool::numberOfAliveHandles() const
{
    size_t count = 0;
    {
        SharedLockGuard concurrencyGuard(m_concurrency);
        SharedLockGuard memoryGuard(m_memory);
        for (const auto &handles : m_handles) {
            count += handles.size();
        }
    }
    return count;
}

bool HandlePool::isAliving() const
{
    bool aliving = false;
    {
        SharedLockGuard concurrencyGuard(m_concurrency);
        SharedLockGuard memoryGuard(m_memory);
        for (const auto &handles : m_handles) {
            if (handles.size() > 0) {
                aliving = true;
                break;
            }
        }
    }
    return aliving;
}

const std::set<std::shared_ptr<InnerHandle>> &HandlePool::getHandlesOfSlot(HandleSlot slot)
{
    WCTAssert(m_concurrency.readSafety());
    WCTAssert(m_memory.readSafety());
    WCTAssert(slot < HandleSlotCount);
    return m_handles[slot];
}

RecyclableHandle HandlePool::flowOut(HandleType type)
{
    HandleSlot slot = slotOfHandleType(type);
    WCTAssert(slot < HandleSlotCount);
    HandleCategory category = categoryOfHandleType(type);
    WCTAssert(category < HandleCategoryCount);

    ReferencedHandle &referencedHandle = m_threadedHandles.getOrCreate().at(category);
    {
        // threaded handles is thread safe.
        if (referencedHandle.handle != nullptr) {
            WCTAssert(m_concurrency.readSafety());
            WCTAssert(referencedHandle.reference > 0);
            ++referencedHandle.reference;
            return RecyclableHandle(
            referencedHandle.handle,
            std::bind(&HandlePool::flowBack, this, type, std::placeholders::_1));
        }
    }

    SharedLockGuard concurrencyGuard(m_concurrency);
    std::shared_ptr<InnerHandle> handle;
    {
        LockGuard memoryGuard(m_memory);
        auto &freeSlot = m_frees[slot];
        if (!freeSlot.empty()) {
            handle = freeSlot.back();
            WCTAssert(handle != nullptr);
            freeSlot.pop_back();
        } else if (!isNumberOfHandlesAllowed()) {
            // auto purge to remove unused handles
            purge();
            if (!isNumberOfHandlesAllowed()) {
                // handle count reachs the limitation.
                Error error(Error::Code::Exceed,
                            Error::Level::Error,
                            "The operating count of database exceeds the maximum allowed.");
                error.infos.insert_or_assign("MaxAllowed", HandlePoolMaxAllowedNumberOfHandles);
                error.infos.insert_or_assign(ErrorStringKeyPath, path);
                Notifier::shared().notify(error);
                setThreadedError(std::move(error));
                return nullptr;
            }
        }
    }

    if (handle == nullptr) {
        handle = generateSlotedHandle(type);
        if (handle == nullptr) {
            return nullptr;
        }

        LockGuard memoryGuard(m_memory);
        // re-check handle count limitation since all lock free code above
        if (!isNumberOfHandlesAllowed()) {
            purge();
            if (!isNumberOfHandlesAllowed()) {
                // the number fof handles reachs the limitation.
                Error error(Error::Code::Exceed,
                            Error::Level::Error,
                            "The operating count of database exceeds the maximum allowed.");
                error.infos.insert_or_assign("MaxAllowed", HandlePoolMaxAllowedNumberOfHandles);
                error.infos.insert_or_assign(ErrorStringKeyPath, path);
                Notifier::shared().notify(error);
                setThreadedError(std::move(error));
                return nullptr;
            }
        }
        WCTAssert(m_handles[slot].find(handle) == m_handles[slot].end());
        m_handles[slot].emplace(handle);
    } else {
        if (!willReuseSlotedHandle(type, handle.get())) {
            handle->close();
            LockGuard memoryGuard(m_memory);
            // remove if the exists handle fails in handles
            m_handles[slot].erase(handle);
            return nullptr;
        }
    }

    WCTAssert(handle != nullptr);

    m_concurrency.lockShared();
    WCTAssert(referencedHandle.handle == nullptr && referencedHandle.reference == 0);
    referencedHandle.handle = handle;
    referencedHandle.reference = 1;
    return RecyclableHandle(
    handle, std::bind(&HandlePool::flowBack, this, type, std::placeholders::_1));
}

void HandlePool::flowBack(HandleType type, const std::shared_ptr<InnerHandle> &handle)
{
    WCTAssert(handle != nullptr);
    WCTAssert(m_concurrency.readSafety());

    HandleSlot slot = slotOfHandleType(type);
    WCTAssert(slot < HandleSlotCount);
    HandleCategory category = categoryOfHandleType(type);
    WCTAssert(category < HandleCategoryCount);

    ReferencedHandle &referencedHandle = m_threadedHandles.getOrCreate().at(category);
    WCTAssert(referencedHandle.handle == handle);
    WCTAssert(referencedHandle.reference > 0);
    if (--referencedHandle.reference == 0) {
        referencedHandle.handle = nullptr;
        WCTRemedialAssert(
        !handle->isPrepared(), "Statement is not finalized.", handle->finalize(););
        {
            LockGuard memoryGuard(m_memory);
            m_frees[slot].push_back(handle);
        }
        m_concurrency.unlockShared();
    }
}

HandlePool::ReferencedHandle::ReferencedHandle() : handle(nullptr), reference(0)
{
}

} //namespace WCDB
