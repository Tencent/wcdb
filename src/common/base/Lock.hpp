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

#pragma once

#include "Thread.hpp"
#include "ThreadLocal.hpp"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>

namespace WCDB {

#pragma mark - Conditional
class Conditional final : public std::condition_variable {
public:
    // false for timeout
    bool wait_for(std::unique_lock<std::mutex> &lockGuard, double seconds);
#ifdef __APPLE__
    void notify(const Thread &thread);
#endif
};

#pragma mark - Shared Lock
// TODO:
// std::shared_timed_mutex is supported since iOS 10 and macOS 10.12.
// std::shared_mutex is supported in a more recent version.
class SharedLock final {
public:
    typedef std::function<void(void)> PendingCallback;
    SharedLock();
    ~SharedLock();

    void lock();
    bool isLocked();
    void unlock();

    void lockShared();
    void unlockShared();

    enum Level {
        None = 0,
        Read = 1,
        Write = 2,
    };
    Level level() const;
    bool readSafety() const;
    bool writeSafety() const;

protected:
    mutable std::mutex m_lock;
    Conditional m_conditionalReaders;
    Conditional m_conditionalWriters;
    int m_readers;
    int m_writers;
    int m_pendingReaders;
    std::queue<Thread> m_pendingWriters;
    Thread m_locking;
    // mutable since it can be only modified threaded
    mutable ThreadLocal<int> m_threadedReaders;
};

#pragma mark - Lock Guard
class LockGuard final {
public:
    LockGuard(SharedLock &lock);
    LockGuard(LockGuard &&movable);
    LockGuard(const std::nullptr_t &);
    ~LockGuard();

    LockGuard() = delete;
    LockGuard(const LockGuard &) = delete;
    LockGuard &operator=(const LockGuard &) = delete;

    bool valid() const;

protected:
    SharedLock *m_lock;
};

#pragma mark - Shared Lock Guard
class SharedLockGuard final {
public:
    SharedLockGuard(SharedLock &lock);
    SharedLockGuard(SharedLockGuard &&movable);
    SharedLockGuard(const std::nullptr_t &);
    ~SharedLockGuard();

    SharedLockGuard() = delete;
    SharedLockGuard(const SharedLockGuard &) = delete;
    SharedLockGuard &operator=(const SharedLockGuard &) = delete;

    bool valid() const;

protected:
    SharedLock *m_lock;
};

} //namespace WCDB
