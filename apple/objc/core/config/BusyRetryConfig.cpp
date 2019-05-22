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
#include <WCDB/BusyRetryConfig.hpp>
#include <WCDB/CoreConst.h>
#include <WCDB/Handle.hpp>
#include <WCDB/Time.hpp>

namespace WCDB {

BusyRetryConfig::BusyRetryConfig()
: Config(), m_identifier(String::formatted("Busy-%p", this))
{
    Global::shared().setNotificationForLockEvent(
    m_identifier,
    std::bind(&BusyRetryConfig::willLock, this, std::placeholders::_1, std::placeholders::_2),
    std::bind(&BusyRetryConfig::lockDidChange, this, std::placeholders::_1, std::placeholders::_2),
    std::bind(
    &BusyRetryConfig::willShmLock, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
    std::bind(&BusyRetryConfig::shmLockDidChange,
              this,
              std::placeholders::_1,
              std::placeholders::_2,
              std::placeholders::_3,
              std::placeholders::_4,
              std::placeholders::_5));
}

BusyRetryConfig::~BusyRetryConfig()
{
    Global::shared().setNotificationForLockEvent(
    m_identifier, nullptr, nullptr, nullptr, nullptr);
}

bool BusyRetryConfig::invoke(Handle* handle)
{
    handle->setNotificationWhenBusy(std::bind(
    &BusyRetryConfig::onBusy, this, std::placeholders::_1, std::placeholders::_2));
    return true;
}

bool BusyRetryConfig::uninvoke(Handle* handle)
{
    handle->setNotificationWhenBusy(nullptr);
    return true;
}

bool BusyRetryConfig::onBusy(const String& path, int numberOfTimes)
{
    WCTInnerAssert(m_tryings.getOrCreate()->find(path)
                   != m_tryings.getOrCreate()->end());
    Trying& trying = m_tryings.getOrCreate()->at(path);
    if (numberOfTimes == 0) {
        trying.retrying(pthread_main_np() != 0 ? BusyRetryTimeOutForMainThread :
                                                 BusyRetryTimeOutForSubThread);
    }
    return getOrCreateState(path).wait(trying);
}

#pragma mark - State
BusyRetryConfig::Expecting::Expecting() : m_type(Type::None)
{
}

void BusyRetryConfig::Expecting::expecting(ShmLock lock, int mask)
{
    m_type = Type::ShmLock;
    m_shmLock = lock;
    m_shmMask = mask;
}

void BusyRetryConfig::Expecting::expecting(Lock lock)
{
    m_type = Type::Lock;
    WCTInnerAssert(lock != Lock::None);
    m_lock = lock;
}

bool BusyRetryConfig::Expecting::satisfied(Lock lock) const
{
    WCTInnerAssert(m_type != Type::None);
    bool satisfied = true;
    if (m_type == Type::Lock) {
        switch (m_lock) {
        case Lock::Reserved:
            satisfied = lock != Lock::Reserved && lock != Lock::Pending
                        && lock != Lock::Exclusive;
            break;
        case Lock::Pending:
            // fallthrough
        case Lock::Exclusive:
            satisfied = lock == Lock::None;
            break;
        default:
            WCTInnerAssert(m_lock == Lock::Shared);
            satisfied = lock != Lock::Pending && lock != Lock::Exclusive;
            break;
        }
    }
    return satisfied;
}

bool BusyRetryConfig::Expecting::satisfied(int sharedMask, int exclusiveMask) const
{
    bool satisified = true;
    if (m_type == Type::ShmLock) {
        switch (m_shmLock) {
        case ShmLock::Shared:
            satisified = (m_shmMask & exclusiveMask) == 0;
            break;
        default:
            WCTInnerAssert(m_shmLock == ShmLock::Exclusive);
            satisified
            = (m_shmMask & sharedMask) == 0 && (m_shmMask & exclusiveMask) == 0;
            break;
        }
    }
    return satisified;
}

BusyRetryConfig::State::Mask::Mask() : shared(0), exclusive(0)
{
}

BusyRetryConfig::State& BusyRetryConfig::getOrCreateState(const String& path)
{
    {
        SharedLockGuard lockGuard(m_statesLock);
        auto iter = m_states.find(path);
        if (iter != m_states.end()) {
            return iter->second;
        }
    }
    {
        LockGuard lockGuard(m_statesLock);
        return m_states[path];
    }
}

BusyRetryConfig::State::State()
: m_lock(Lock::None), m_cond(PTHREAD_COND_INITIALIZER), m_mutex(PTHREAD_MUTEX_INITIALIZER)
{
}

void BusyRetryConfig::State::updateLock(Lock lock)
{
    pthread_mutex_lock(&m_mutex);
    if (m_lock != lock) {
        m_lock = lock;
        tryNotify();
    }
    pthread_mutex_unlock(&m_mutex);
}

void BusyRetryConfig::State::updateShmSharedLocked(void* identifier, int mask)
{
    pthread_mutex_lock(&m_mutex);
    m_masks[identifier].shared |= mask;
    tryNotify();
    pthread_mutex_unlock(&m_mutex);
}

void BusyRetryConfig::State::updateShmExclusiveLocked(void* identifier, int mask)
{
    pthread_mutex_lock(&m_mutex);
    m_masks[identifier].exclusive |= mask;
    tryNotify();
    pthread_mutex_unlock(&m_mutex);
}

void BusyRetryConfig::State::updateShmUnlocked(void* identifier, int mask_)
{
    pthread_mutex_lock(&m_mutex);
    State::Mask& mask = m_masks[identifier];
    mask.shared &= ~mask_;
    mask.exclusive &= ~mask_;
    tryNotify();
    pthread_mutex_unlock(&m_mutex);
}

bool BusyRetryConfig::State::shouldWait(const Expecting& expecting) const
{
    if (!expecting.satisfied(m_lock)) {
        return true;
    }
    for (const auto& iter : m_masks) {
        if (!expecting.satisfied(iter.second.shared, iter.second.exclusive)) {
            return true;
        }
    }
    return false;
}

bool BusyRetryConfig::State::wait(Trying& trying)
{
    bool result = false;
    double remainingTimeForRetring = trying.remainingTimeForRetring();
    if (remainingTimeForRetring > 0) {
        pthread_mutex_lock(&m_mutex);
        if (shouldWait(trying)) {
            m_waitings.insert(0, pthread_self(), trying);
            SteadyClock before = SteadyClock::now();
            struct timespec relative;
            relative.tv_nsec
            = (long) ((remainingTimeForRetring - (long) remainingTimeForRetring) * 1E9);
            relative.tv_sec = (long) remainingTimeForRetring;
            result = pthread_cond_timedwait_relative_np(&m_cond, &m_mutex, &relative) == 0;
            if (result) {
                std::time_t cost
                = (std::time_t) std::chrono::duration_cast<std::chrono::nanoseconds>(
                  SteadyClock::now() - before)
                  .count();
                trying.retried((double) cost / 1E9);
            }
            m_waitings.erase(pthread_self());
        } else {
            // retry since the previous lock is released.
            result = true;
        }
        pthread_mutex_unlock(&m_mutex);
    }
    return result;
}

void BusyRetryConfig::State::tryNotify()
{
    const auto& elements = m_waitings.elements();
    for (auto iter = elements.rbegin(); iter != elements.rend(); ++iter) {
        if (shouldWait(iter->value)) {
            return;
        } else {
            pthread_cond_signal_thread_np(&m_cond, iter->key);
        }
    }
}

#pragma mark - Trying
BusyRetryConfig::Trying::Trying()
: Expecting(), m_elapsedTime(0), m_timeout(std::numeric_limits<int>::max())
{
}

void BusyRetryConfig::Trying::retrying(double timeout)
{
    m_timeout = timeout;
    m_elapsedTime = 0;
}

void BusyRetryConfig::Trying::retried(double cost)
{
    m_elapsedTime += cost;
}

double BusyRetryConfig::Trying::remainingTimeForRetring() const
{
    return m_timeout - m_elapsedTime;
}

#pragma mark - Lock Event
void BusyRetryConfig::willLock(const String& path, Lock lock)
{
    (*m_tryings.getOrCreate())[path].expecting(lock);
}

void BusyRetryConfig::lockDidChange(const String& path, Lock lock)
{
    getOrCreateState(path).updateLock(lock);
}

void BusyRetryConfig::willShmLock(const String& path, ShmLock lock, int mask)
{
    (*m_tryings.getOrCreate())[path].expecting(lock, mask);
}

void BusyRetryConfig::shmLockDidChange(
const String& path, ShmAction action, ShmLock lock, void* identifier, int mask)
{
    State& state = getOrCreateState(path);
    switch (action) {
    case ShmAction::Lock:
        switch (lock) {
        case ShmLock::Shared:
            state.updateShmSharedLocked(identifier, mask);
            break;
        default:
            WCTInnerAssert(lock == ShmLock::Exclusive);
            state.updateShmExclusiveLocked(identifier, mask);
            break;
        }
        break;
    default:
        WCTInnerAssert(action == ShmAction::Unlock);
        state.updateShmUnlocked(identifier, mask);
        break;
    }
}

} // namespace WCDB
