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

#include <WCDB/Lock.hpp>

namespace WCDB {

Lock::Lock()
{
}

SpinLock::SpinLock() : Lock()
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
    : m_reader(0), m_writer(0), m_pending(0), m_lockingThread(std::thread::id())
{
}

void SharedLock::lockShared()
{
    std::unique_lock<decltype(m_mutex)> lock(m_mutex);
    if (!isThreadedLocked()) {
        while (m_writer > 0 || m_pending > 0) {
            m_cond.wait(lock);
        }
    }
#ifdef DEBUG
    debug_threadedLockShared();
#endif
    ++m_reader;
}

void SharedLock::unlockShared()
{
    std::unique_lock<decltype(m_mutex)> lock(m_mutex);
#ifdef DEBUG
    debug_threadedUnlockShared();
#endif
    if (--m_reader == 0 && !isThreadedLocked()) {
        m_cond.notify_all();
    }
}

void SharedLock::lock()
{
    std::unique_lock<decltype(m_mutex)> lock(m_mutex);
    if (!isThreadedLocked()) {
        ++m_pending;
        while (m_writer > 0 || m_reader > 0) {
            m_cond.wait(lock);
        }
        --m_pending;
        m_lockingThread = std::this_thread::get_id();
    }
    ++m_writer;
}

void SharedLock::unlock()
{
    std::unique_lock<decltype(m_mutex)> lock(m_mutex);
    if (--m_writer == 0) {
        m_lockingThread = std::thread::id();
        m_cond.notify_all();
    }
}

bool SharedLock::isLocked() const
{
    std::unique_lock<decltype(m_mutex)> lock(m_mutex);
    return m_writer > 0;
}

bool SharedLock::isThreadedLocked() const
{
    return m_lockingThread == std::this_thread::get_id();
}

#ifdef DEBUG
bool SharedLock::debug_isSharedLocked() const
{
    std::unique_lock<decltype(m_mutex)> lock(m_mutex);
    std::map<std::thread::id, int> *lockingSharedThread =
        debug_m_lockingSharedThread.get();
    return lockingSharedThread->find(std::this_thread::get_id()) !=
           lockingSharedThread->end();
}

void SharedLock::debug_threadedLockShared()
{
    std::map<std::thread::id, int> *lockingSharedThread =
        debug_m_lockingSharedThread.get();
    std::thread::id current = std::this_thread::get_id();
    auto iter = lockingSharedThread->find(current);
    if (iter == lockingSharedThread->end()) {
        iter = lockingSharedThread->insert({current, 0}).first;
    }
    ++iter->second;
}

void SharedLock::debug_threadedUnlockShared()
{
    std::map<std::thread::id, int> *lockingSharedThread =
        debug_m_lockingSharedThread.get();
    std::thread::id current = std::this_thread::get_id();
    auto iter = lockingSharedThread->find(current);
    assert(iter != lockingSharedThread->end());
    if (--iter->second == 0) {
        lockingSharedThread->erase(iter);
    }
}
#endif

LockGuard::LockGuard(Lock &lock) : m_lock(lock)
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
