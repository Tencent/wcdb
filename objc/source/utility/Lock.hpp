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

#ifndef Lock_hpp
#define Lock_hpp

#include <atomic>
#include <shared_mutex>
#include <thread>
#ifdef DEBUG
#include <WCDB/ThreadLocal.hpp>
#include <map>
#endif

namespace WCDB {

class Lock {
public:
    Lock();
    Lock(const Lock &) = delete;
    Lock &operator=(const Lock &) = delete;
    virtual void lock() = 0;
    virtual void unlock() = 0;
};

//TODO using mutex for single core CPU device
class SpinLock : public Lock {
public:
    SpinLock();

    void lock() override;
    void unlock() override;

protected:
    std::atomic_flag m_locked = ATOMIC_FLAG_INIT;
};

/*TODO 
 * std::shared_mutex is available since C++17, iOS 10.0, macOS 10.12    
 * std::shared_timed_mutex is available since C++14, iOS 10.0, macOS 10.12
 */
class SharedLock : public Lock {
public:
    SharedLock();
    void lock() override;
    void unlock() override;

    void lockShared();
    void unlockShared();

    bool isLocked() const;

protected:
    bool isThreadedLocked() const;
    mutable std::mutex m_mutex;
    std::condition_variable m_cond;
    int m_reader;
    int m_writer;
    int m_pending;
    std::thread::id m_lockingThread;

#ifdef DEBUG
public:
    bool debug_isSharedLocked() const;

protected:
    mutable ThreadLocal<std::map<std::thread::id, int>>
        debug_m_lockingSharedThread;
    void debug_threadedLockShared();
    void debug_threadedUnlockShared();
#endif
};

class SharedLockGuard {
public:
    SharedLockGuard(SharedLock &lock);
    ~SharedLockGuard();

    SharedLockGuard() = delete;
    SharedLockGuard(const SharedLockGuard &) = delete;
    SharedLockGuard &operator=(const SharedLockGuard &) = delete;

protected:
    SharedLock &m_lock;
};

class LockGuard {
public:
    LockGuard(Lock &lock);
    ~LockGuard();

    LockGuard() = delete;
    LockGuard(const LockGuard &) = delete;
    LockGuard &operator=(const LockGuard &) = delete;

protected:
    Lock &m_lock;
};

} //namespace WCDB

#endif /* Lock_hpp */
