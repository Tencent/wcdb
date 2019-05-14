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
#include <WCDB/Lock.hpp>

namespace WCDB {

Lockable::Lockable()
{
}

Lockable::~Lockable()
{
}

SpinLock::SpinLock() : Lockable()
{
}

void SpinLock::lock()
{
    while (m_locked.test_and_set(std::memory_order_acquire))
        ;
}

void SpinLock::unlock()
{
    m_locked.clear(std::memory_order_release);
}

SharedLock::SharedLock()
: m_readers(0)
, m_writers(0)
, m_pendingReaders(0)
, m_pendingWriters(0)
, m_locking(std::thread::id())
, m_threadedReaders(0)
{
}

void SharedLock::lockShared()
{
    std::unique_lock<std::mutex> lockGuard(m_mutex);
    if (m_writers > 0 ? m_locking != std::this_thread::get_id() :
                        (m_pendingWriters > 0 && *m_threadedReaders.getOrCreate() == 0)) {
        // If it is locked but not current thread, it should wait for the write lock.
        // If it is not locked but there is someone pending to lock and current thread is not already shared locked, it should wait for the pending lock to avoid the pending lock starve.
        ++m_pendingReaders;
        do {
            m_readersCond.wait(lockGuard);
        } while (m_writers > 0 || m_pendingWriters > 0);
        --m_pendingReaders;
    }
    // it's already locked by current thread
    // or it's already shared locked by current thread
    // or it's not locked
    WCTInnerAssert(m_locking == std::this_thread::get_id()
                   || *m_threadedReaders.getOrCreate() > 0 || m_writers == 0);
    ++m_readers;
    ++*m_threadedReaders.getOrCreate();
}

void SharedLock::unlockShared()
{
    int *threadedReader = m_threadedReaders.getOrCreate();
    WCTRemedialAssert(*threadedReader > 0, "Unpaired unlock shared.", return;);
    WCTInnerAssert(*threadedReader > 0);

    std::unique_lock<std::mutex> lockGuard(m_mutex);
    WCTInnerAssert(m_readers > 0);
    --*threadedReader;
    --m_readers;
    if (m_readers == 0) {
        WCTInnerAssert(*m_threadedReaders.getOrCreate() == 0);
        if (m_writers == 0 && m_pendingWriters > 0) {
            m_writersCond.notify_one();
        }
    }
}

void SharedLock::lock()
{
    WCTRemedialAssert(
    *m_threadedReaders.getOrCreate() == 0, "Upgrade lock is not supported.", return;);

    std::unique_lock<std::mutex> lockGuard(m_mutex);
    if ((m_readers > 0 || m_writers > 0) && m_locking != std::this_thread::get_id()) {
        // If it is locked but not current thread, it should wait for the write lock
        // If it is shared locked but not current thread, it should wait for the read lock
        // Note that it can't be called when it is shared locked by current thread, no matter the write lock is hold or not.
        ++m_pendingWriters;
        do {
            m_writersCond.wait(lockGuard);
        } while (m_readers > 0 || m_writers > 0);
        --m_pendingWriters;
    }
    // it's already locked by current thread
    // or it's not locked and it's not shared locked
    WCTInnerAssert(m_locking == std::this_thread::get_id()
                   || (m_writers == 0 && m_readers == 0));
    ++m_writers;
    m_locking = std::this_thread::get_id();
}

void SharedLock::unlock()
{
    WCTRemedialAssert(
    *m_threadedReaders.getOrCreate() == 0, "Downgrade lock is not supported.", return;);

    std::unique_lock<std::mutex> lockGuard(m_mutex);
    WCTRemedialAssert(m_locking == std::this_thread::get_id(), "Unpaired unlock.", return;);
    WCTInnerAssert(m_readers == 0);
    WCTInnerAssert(m_writers > 0);
    if (--m_writers == 0) {
        m_locking = std::thread::id();
        // write lock first
        if (m_pendingWriters > 0) {
            m_writersCond.notify_one();
        } else if (m_pendingReaders > 0) {
            m_readersCond.notify_all();
        }
    }
}

SharedLock::Level SharedLock::level() const
{
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    if (m_locking == std::this_thread::get_id()) {
        return Level::Write;
    } else if ((*m_threadedReaders.getOrCreate()) > 0) {
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

SharedLock::~SharedLock()
{
    WCTRemedialAssert(m_writers == 0 && m_pendingWriters == 0, "Unpaired lock", ;);
    WCTRemedialAssert(m_readers == 0 && m_pendingReaders == 0, "Unpaired shared lock", ;);
}

LockGuard::LockGuard(const std::nullptr_t &) : m_lock(nullptr)
{
}

LockGuard::LockGuard(Lockable &lock) : m_lock(&lock)
{
    m_lock->lock();
}

LockGuard::LockGuard(LockGuard &&guard) : m_lock(guard.m_lock)
{
    guard.m_lock = nullptr;
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

SharedLockGuard::SharedLockGuard(SharedLockGuard &&guard) : m_lock(guard.m_lock)
{
    guard.m_lock = nullptr;
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
