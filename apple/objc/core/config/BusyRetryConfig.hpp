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

#include <WCDB/Config.hpp>
#include <WCDB/Global.hpp>
#include <WCDB/OrderedUniqueList.hpp>
#include <WCDB/String.hpp>
#include <WCDB/ThreadLocal.hpp>
#include <mutex>

namespace WCDB {

class BusyRetryConfig final : public Config {
public:
    BusyRetryConfig();
    ~BusyRetryConfig();

    bool invoke(Handle* handle) override final;
    bool uninvoke(Handle* handle) override final;

protected:
    bool onBusy(const String& path, int numberOfTimes);

    const String m_identifier;

#pragma mark - Lock Event
protected:
    typedef Global::Lock Lock;
    typedef Global::ShmLock ShmLock;
    void willLock(const String& path, Lock lock);
    void lockDidChange(const String& path, Lock lock);
    void willShmLock(const String& path, ShmLock lock, int mask);
    void shmLockDidChange(const String& path, void* identifier, int sharedMask, int exclusiveMask);

#pragma mark - State
protected:
    class Expecting {
    public:
        Expecting();

        void expecting(ShmLock lock, int mask);
        void expecting(Lock lock);

        bool satisfied(Lock lock) const;
        bool satisfied(int sharedMask, int exclusiveMask) const;

    private:
        enum class Type {
            None,
            Lock,
            ShmLock,
        };
        Type m_type;
        union {
            struct {
                ShmLock m_shmLock;
                int m_shmMask;
            };
            Lock m_lock;
        };
    };

    class Trying;
    class State {
    public:
        State();

        void updateLock(Lock lock);
        void updateShmLock(void* identifier, int sharedMask, int exclusiveMask);

        bool wait(Trying& trying);

    protected:
        bool shouldWait(const Expecting& expecting) const;
        Lock m_lock;
        struct Mask {
            Mask();
            int shared;
            int exclusive;
        };
        typedef struct Mask Mask;
        std::map<void* /* identifier */, Mask> m_masks;

        void tryNotify();
        pthread_mutex_t m_mutex;
        pthread_cond_t m_cond;
        OrderedUniqueList<pthread_t, Expecting> m_waitings;
    };

    State& getOrCreateState(const String& path);

private:
    SharedLock m_statesLock;
    std::map<String, State> m_states;

#pragma mark - Trying
protected:
    class Trying : public Expecting {
    public:
        Trying();

        void retrying(double timeout);
        void retried(double cost);
        double remainingTimeForRetring() const;

    private:
        double m_elapsedTime;
        double m_timeout;
    };
    ThreadLocal<std::map<String, Trying>> m_tryings;
};

} //namespace WCDB
