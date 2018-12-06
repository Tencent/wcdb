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

#ifndef _WCDB_LOCK_HPP
#define _WCDB_LOCK_HPP

#include <WCDB/ThreadLocal.hpp>
#include <atomic>
#include <thread>

namespace WCDB {

class Lockable {
public:
    Lockable();
    Lockable(const Lockable &) = delete;
    Lockable &operator=(const Lockable &) = delete;
    virtual ~Lockable();
    virtual void lock() = 0;
    virtual void unlock() = 0;
};

// TODO: use Lock instead of SpinLock for single-core device.
class SpinLock final : public Lockable {
public:
    SpinLock();

    void lock() override final;
    void unlock() override final;

protected:
    std::atomic_flag m_locked = ATOMIC_FLAG_INIT;
};

#if __cplusplus > 201402L
#warning TODO \
std::shared_mutex is available since C++17, iOS 10.0, macOS 10.12\
std::shared_timed_mutex is available since C++14, iOS 10.0, macOS 10.12
#endif
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
    mutable std::mutex m_mutex;
    std::condition_variable m_readersCond;
    std::condition_variable m_writersCond;
    int m_readers;
    int m_writers;
    int m_pendingReaders;
    int m_pendingWriters;
    std::thread::id m_locking;
    // mutable since it can be only modified threaded
    mutable ThreadLocal<int> m_threadedReaders;
};

class SharedLockGuard final {
public:
    SharedLockGuard(SharedLock &lock);
    ~SharedLockGuard();

    SharedLockGuard() = delete;
    SharedLockGuard(const SharedLockGuard &) = delete;
    SharedLockGuard &operator=(const SharedLockGuard &) = delete;

protected:
    SharedLock &m_lock;
};

class LockGuard final {
public:
    LockGuard(Lockable &lock);
    ~LockGuard();

    LockGuard() = delete;
    LockGuard(const LockGuard &) = delete;
    LockGuard &operator=(const LockGuard &) = delete;

protected:
    Lockable &m_lock;
};

} //namespace WCDB

#endif /* _WCDB_LOCK_HPP */
