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

#include <WCDB/Thread.hpp>
#include <WCDB/ThreadLocal.hpp>
#include <atomic>
#include <mutex>

namespace WCDB {

#pragma mark - Lockable
class Lockable {
public:
    Lockable();
    Lockable(const Lockable &) = delete;
    Lockable &operator=(const Lockable &) = delete;
    virtual ~Lockable() = 0;
    virtual void lock() = 0;
    virtual void unlock() = 0;
};

#pragma mark - Lock
class Lock final : public Lockable {
public:
    using Lockable::Lockable;

    void lock() override final;
    void unlock() override final;

protected:
    friend class Conditional;
    std::mutex m_mutex;
};

#pragma mark - Conditional
class Conditional final {
public:
    void wait(Lock &lock);
    // false for timeout
    bool wait_for(Lock &lock, double seconds);

    void signal(); // notify one
    void notify(); // notify all
    void notify(const Thread &thread);

protected:
    std::condition_variable m_condition;
};

#pragma mark - Shared Lock
class SharedLock final : public Lockable {
public:
    SharedLock();
    ~SharedLock();

    void lock() override final;
    void unlock() override final;

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
    mutable Lock m_lock;
    Conditional m_conditionalReaders;
    Conditional m_conditionalWriters;
    int m_readers;
    int m_writers;
    int m_pendingReaders;
    int m_pendingWriters;
    Thread m_locking;
    // mutable since it can be only modified threaded
    mutable ThreadLocal<int> m_threadedReaders;
};

#pragma mark - Lock Guard
class LockGuard final {
public:
    LockGuard(Lockable &lock);
    LockGuard(LockGuard &&movable);
    LockGuard(const std::nullptr_t &);
    ~LockGuard();

    LockGuard() = delete;
    LockGuard(const LockGuard &) = delete;
    LockGuard &operator=(const LockGuard &) = delete;

    bool valid() const;

protected:
    Lockable *m_lock;
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
