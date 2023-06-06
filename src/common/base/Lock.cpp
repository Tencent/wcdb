//
// Created by sanhuazhang on 2018/09/29
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

#include "Lock.hpp"
#include "Assertion.hpp"
#include "CrossPlatform.h"
#include <condition_variable>

namespace WCDB {

#pragma mark - Conditional
bool Conditional::wait_for(std::unique_lock<std::mutex> &lockGuard, double seconds)
{
    WCTAssert(lockGuard.owns_lock());
    bool timeout = false;
    if (seconds > 0) {
        timeout = std::condition_variable::wait_for(
                  lockGuard, std::chrono::nanoseconds((long long) (seconds * 1E9)))
                  == std::cv_status::timeout;
    }
    return !timeout;
}

#ifdef __APPLE__
void Conditional::notify(const Thread &thread)
{
    pthread_cond_signal_thread_np(native_handle(), thread.m_pthreadId);
}
#endif

#pragma mark - Shared Lock
SharedLock::SharedLock()
: m_readers(0), m_writers(0), m_pendingReaders(0), m_threadedReaders(0)
{
}

SharedLock::~SharedLock()
{
    WCTRemedialAssert(m_writers == 0 && m_pendingWriters.size() == 0, "Unpaired lock", ;);
    WCTRemedialAssert(m_readers == 0 && m_pendingReaders == 0, "Unpaired shared lock", ;);
}

void SharedLock::lockShared()
{
    std::unique_lock<std::mutex> lockGuard(m_lock);
    if (m_writers > 0 ?
        !m_locking.isCurrentThread() :
        (m_pendingWriters.size() > 0 && m_threadedReaders.getOrCreate() == 0)) {
        // If it is locked but not current thread, it should wait for the write lock.
        // If it is not locked but there is someone pending to lock and current thread is not already shared locked, it should wait for the pending lock to avoid the pending lock starve.
        ++m_pendingReaders;
        do {
            m_conditionalReaders.wait(lockGuard);
        } while (m_writers > 0 || m_pendingWriters.size() > 0);
        --m_pendingReaders;
    }
    // it's already locked by current thread
    // or it's already shared locked by current thread
    // or it's not locked
    WCTAssert(m_locking.isCurrentThread() || m_threadedReaders.getOrCreate() > 0
              || m_writers == 0);
    ++m_readers;
    ++m_threadedReaders.getOrCreate();
}

void SharedLock::unlockShared()
{
    int &threadedReader = m_threadedReaders.getOrCreate();
    WCTRemedialAssert(threadedReader > 0, "Unpaired unlock shared.", return;);
    WCTAssert(threadedReader > 0);

    std::unique_lock<std::mutex> lockGuard(m_lock);
    WCTAssert(m_readers > 0);
    --threadedReader;
    --m_readers;
    if (m_readers == 0) {
        WCTAssert(m_threadedReaders.getOrCreate() == 0);
        if (m_writers == 0 && m_pendingWriters.size() > 0) {
#ifdef __APPLE__
            m_conditionalWriters.notify(m_pendingWriters.front());
#else
            m_conditionalWriters.notify_all();
#endif
        }
    }
}

void SharedLock::lock()
{
    WCTRemedialAssert(
    m_threadedReaders.getOrCreate() == 0, "Upgrade lock is not supported.", return;);

    Thread current = Thread::current();
    std::unique_lock<std::mutex> lockGuard(m_lock);
    if (!m_locking.equal(current)) {
        m_pendingWriters.emplace(current);
        while (m_readers > 0 || m_writers > 0 || !m_pendingWriters.front().equal(current)) {
            m_conditionalWriters.wait(lockGuard);
        }
        WCTAssert(m_pendingWriters.front().isCurrentThread());
        m_pendingWriters.pop();
    }
    // it's already locked by current thread
    // or it's not locked and it's not shared locked
    WCTAssert(m_locking.isCurrentThread() || (m_writers == 0 && m_readers == 0));
    ++m_writers;
    m_locking = Thread::current();
}

bool SharedLock::isLocked()
{
    return m_writers > 0;
}

void SharedLock::unlock()
{
    WCTRemedialAssert(
    m_threadedReaders.getOrCreate() == 0, "Downgrade lock is not supported.", return;);

    std::unique_lock<std::mutex> lockGuard(m_lock);
    WCTRemedialAssert(m_locking.isCurrentThread(), "Unpaired unlock.", return;);
    WCTAssert(m_readers == 0);
    WCTAssert(m_writers > 0);
    if (--m_writers == 0) {
        m_locking = nullptr;
        // write lock first
        if (m_pendingWriters.size() > 0) {
#ifdef __APPLE__
            m_conditionalWriters.notify(m_pendingWriters.front());
#else
            m_conditionalWriters.notify_all();
#endif
        } else if (m_pendingReaders > 0) {
            m_conditionalReaders.notify_all();
        }
    }
}

SharedLock::Level SharedLock::level() const
{
    std::unique_lock<std::mutex> lockGuard(m_lock);
    if (m_locking.isCurrentThread()) {
        return Level::Write;
    } else if ((m_threadedReaders.getOrCreate()) > 0) {
        return Level::Read;
    }
    return Level::None;
}

bool SharedLock::readSafety() const
{
    return level() >= SharedLock::Level::Read;
}

bool SharedLock::writeSafety() const
{
    return level() >= SharedLock::Level::Write;
}

#pragma mark - Lock Guard
LockGuard::LockGuard(const std::nullptr_t &) : m_lock(nullptr)
{
}

LockGuard::LockGuard(SharedLock &lock) : m_lock(&lock)
{
    m_lock->lock();
}

LockGuard::LockGuard(LockGuard &&movable) : m_lock(movable.m_lock)
{
    movable.m_lock = nullptr;
}

LockGuard::~LockGuard()
{
    if (m_lock != nullptr) {
        m_lock->unlock();
    }
}

bool LockGuard::valid() const
{
    return m_lock != nullptr;
}

#pragma mark - Shared Lock Guard
SharedLockGuard::SharedLockGuard(SharedLockGuard &&movable)
: m_lock(movable.m_lock)
{
    movable.m_lock = nullptr;
}

SharedLockGuard::SharedLockGuard(const std::nullptr_t &) : m_lock(nullptr)
{
}

SharedLockGuard::SharedLockGuard(SharedLock &lock) : m_lock(&lock)
{
    m_lock->lockShared();
}

bool SharedLockGuard::valid() const
{
    return m_lock != nullptr;
}

SharedLockGuard::~SharedLockGuard()
{
    if (m_lock != nullptr) {
        m_lock->unlockShared();
    }
}

} //namespace WCDB
