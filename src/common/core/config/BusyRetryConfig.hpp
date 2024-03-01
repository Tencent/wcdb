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

#pragma once

#include "Config.hpp"
#include "Global.hpp"
#include "Lock.hpp"
#include "StringView.hpp"
#include "ThreadLocal.hpp"
#include "UniqueList.hpp"

namespace WCDB {

class BusyRetryConfig final : public Config {
public:
    BusyRetryConfig();
    ~BusyRetryConfig() override;

    bool invoke(InnerHandle* handle) override final;
    bool uninvoke(InnerHandle* handle) override final;
    bool checkMainThreadBusyRetry(const UnsafeStringView& path);
    bool checkHasBusyRetry(const UnsafeStringView& path);

protected:
    bool onBusy(const UnsafeStringView& path, int numberOfTimes);

    const StringView m_identifier;

#pragma mark - Busy Moniter
public:
    typedef std::function<void(const UnsafeStringView& path, uint64_t tid)> BusyMonitor;
    void setBusyMonitor(const BusyMonitor& monitor, double timeOut);

private:
    BusyMonitor m_busyMonitor;
    double m_timeOut;

#pragma mark - Lock Event
protected:
    typedef Global::PagerLock PagerLockType;
    typedef Global::ShmLock ShmLockType;
    void willLock(const UnsafeStringView& path, PagerLockType type);
    void lockDidChange(const UnsafeStringView& path, PagerLockType type);
    void willShmLock(const UnsafeStringView& path, ShmLockType type, int mask);
    void shmLockDidChange(const UnsafeStringView& path, void* identifier, int sharedMask, int exclusiveMask);

#pragma mark - State
protected:
    class Expecting {
    public:
        Expecting();

        bool satisfied(PagerLockType type) const;
        bool satisfied(int sharedMask, int exclusiveMask) const;

    protected:
        void expecting(ShmLockType type, int mask);
        void expecting(PagerLockType type);
        bool valid() const;

    private:
        enum class Category {
            None,
            Pager,
            Shm,
        };
        Category m_category;
        ShmLockType m_shmType;
        int m_shmMask;
        PagerLockType m_pagerType;
    };

    class Trying;
    class State {
    public:
        State();

        void updatePagerLock(PagerLockType type);
        void updateShmLock(void* identifier, int sharedMask, int exclusiveMask);

        bool wait(Trying& trying);
        StringView m_path;
        bool checkMainThreadBusyRetry();
        bool checkHasBusyRetry();
        void setBusyMonitor(const BusyMonitor& monitor, double timeOut);

    protected:
        bool shouldWait(const Expecting& expecting) const;
        bool localShouldWait(const Expecting& expecting) const;
        PagerLockType m_pagerType;
        uint64_t m_pagerChangeTid;
        struct ShmMask {
            ShmMask();
            int shared;
            int exclusive;
            uint64_t tid;
        };
        typedef struct ShmMask ShmMask;
        std::map<void* /* identifier */, ShmMask> m_shmMasks;

        void tryNotify();
        std::mutex m_lock;
        Conditional m_conditional;

        enum class Exclusivity {
            Must = 0,
            NoMatter = 1,
        };
        Trying* m_mainThreadBusyTrying;
        UniqueList<Thread, Expecting, Exclusivity> m_waitings;

        BusyMonitor m_busyMonitor;
        double m_timeOut;
    };

    State& getOrCreateState(const UnsafeStringView& path);

private:
    SharedLock m_statesLock;
    StringViewMap<State> m_states;

#pragma mark - Trying
protected:
    class Trying : public Expecting {
    public:
        using Expecting::Expecting;

        void expecting(const UnsafeStringView& path, ShmLockType type, int mask);
        void expecting(const UnsafeStringView& path, PagerLockType type);

        const StringView& getPath() const;

        bool valid() const;

    private:
        StringView m_path;
    };
    typedef struct StateHolder {
        State* state;
    } StateHolder;
    ThreadLocal<Trying> m_tryings;
    ThreadLocal<StateHolder> m_localState;
};

} //namespace WCDB
