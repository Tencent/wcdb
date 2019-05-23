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

#include <WCDB/Exiting.hpp>
#include <WCDB/OrderedUniqueList.hpp>
#include <WCDB/Time.hpp>
#include <chrono>
#include <condition_variable>
#include <stdio.h>

namespace WCDB {

template<typename Key, typename Info>
class TimedQueue final {
private:
    typedef OrderedUniqueList<Key, Info, SteadyClock> List;
    List m_list;
    Conditional m_conditional;
    Lock m_lock;
    bool m_stop;
    std::atomic<bool> m_running;

public:
    TimedQueue() : m_stop(false), m_running(false){};
    ~TimedQueue()
    {
        stop();
        waitUntilDone();
    }

    // return true to erase the element
    typedef std::function<bool(const Key &, const Info &)> ExpiredCallback;

    void queue(const Key &key, double delay, const Info &info, bool reQueue = true)
    {
        if (isExiting()) {
            stop();
            return;
        }
        bool notify = false;
        {
            LockGuard lockGuard(m_lock);
            if (m_stop) {
                return;
            }

            if (!reQueue && m_list.find(key) != nullptr) {
                // no need to queue
                return;
            }

            SteadyClock expired
            = SteadyClock::now() + std::chrono::nanoseconds((long long) (delay * 1E9));

            SteadyClock shortest;
            if (!m_list.elements().empty()) {
                shortest = m_list.elements().begin()->order;
            } else {
                shortest = SteadyClock::max();
            }
            m_list.insert(expired, key, info);
            if (m_list.elements().begin()->order < shortest) {
                notify = true;
            }
        }
        if (notify) {
            m_conditional.signal();
        }
    }

    void remove(const Key &key)
    {
        {
            LockGuard lockGuard(m_lock);
            if (m_stop) {
                return;
            }
            m_list.erase(key);
        }
        if (isExiting()) {
            stop();
            return;
        }
    }

    void stop()
    {
        {
            LockGuard lockGuard(m_lock);
            m_list.clear();
            m_stop = true;
        }
        m_conditional.signal();
    }

    void waitUntilDone()
    {
        while (m_running.load())
            ;
    }

    void loop(const ExpiredCallback &onElementExpired)
    {
        m_running.store(true);
        while (!isExiting()) {
            LockGuard lockGuard(m_lock);
            if (m_stop) {
                break;
            }
            if (m_list.elements().empty() && !isExiting()) {
                m_conditional.wait(m_lock);
                continue;
            }
            SteadyClock now = SteadyClock::now();
            const auto &shortest = m_list.elements().begin();
            if (now < shortest->order && !isExiting()) {
                m_conditional.wait_for(
                m_lock, shortest->order.timeIntervalSinceSteadyClock(now));
                continue;
            }
            Key key = shortest->key;
            Info info = shortest->value;
            m_lock.unlock();
            bool erase = false;
            if (!isExiting()) {
                erase = onElementExpired(key, info);
            }
            m_lock.lock();
            if (erase) {
                m_list.erase(key);
            }
        }
        m_running.store(false);
    }
};

} //namespace WCDB
