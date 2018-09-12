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
        WCTInnerAssert(*m_threadedReaders.getOrCreate() == 0);
        ++m_pendingReaders;
        do {
            m_readersCond.wait(lockGuard);
        } while (m_writers > 0 || m_pendingWriters > 0);
        --m_pendingReaders;
    }
    ++m_readers;
    ++*m_threadedReaders.getOrCreate();
}

void SharedLock::unlockShared()
{
    std::unique_lock<std::mutex> lockGuard(m_mutex);
    WCTInnerAssert(m_readers > 0);
    WCTInnerAssert(*m_threadedReaders.getOrCreate() > 0);
    WCTInnerAssert(m_writers == 0
                   || (m_readers == *m_threadedReaders.getOrCreate()
                       && m_locking == std::this_thread::get_id()));
    --m_readers;
    --*m_threadedReaders.getOrCreate();
    if (m_readers == 0) {
        WCTInnerAssert(*m_threadedReaders.getOrCreate() == 0);
        if (m_writers == 0 && m_pendingWriters > 0) {
            m_writersCond.notify_one();
        }
    }
}

void SharedLock::lock()
{
    std::unique_lock<std::mutex> lockGuard(m_mutex);
    WCTInnerAssert(*m_threadedReaders.getOrCreate() == 0); //Upgrade lock is not supported yet.
    if ((m_readers > 0 || m_writers > 0) && m_locking != std::this_thread::get_id()) {
        ++m_pendingWriters;
        do {
            m_writersCond.wait(lockGuard);
        } while (m_readers > 0 || m_writers > 0);
        --m_pendingWriters;
    }
    ++m_writers;
    m_locking = std::this_thread::get_id();
}

void SharedLock::unlock()
{
    std::unique_lock<std::mutex> lockGuard(m_mutex);
    WCTInnerAssert(*m_threadedReaders.getOrCreate() == 0); //Upgrade lock is not supported yet.
    if (--m_writers == 0) {
        m_locking = std::thread::id();
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
    if (m_writers > 0) {
        return Level::Write;
    } else if (m_readers > 0) {
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
    WCTRemedialAssert(
    m_readers == 0 && m_writers == 0 && m_pendingWriters == 0
    && m_pendingWriters == 0 && m_pendingReaders == 0,
    "Unpaired shared lock",
    while (m_writers > 0) { unlock(); } while (m_readers > 0) { unlockShared(); })
}

LockGuard::LockGuard(Lockable &lock) : m_lock(lock)
{
    m_lock.lock();
}

LockGuard::~LockGuard()
{
    m_lock.unlock();
}

SharedLockGuard::SharedLockGuard(SharedLock &lock) : m_lock(lock)
{
    m_lock.lockShared();
}

SharedLockGuard::~SharedLockGuard()
{
    m_lock.unlockShared();
}

} //namespace WCDB
