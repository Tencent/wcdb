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

#ifndef __WCDB_TIMED_QUEUE_HPP
#define __WCDB_TIMED_QUEUE_HPP

#include <WCDB/Exiting.hpp>
#include <WCDB/OrderedUniqueList.hpp>
#include <WCDB/Time.hpp>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <stdio.h>
#include <thread>

namespace WCDB {

template<typename Key, typename Info>
class TimedQueue final {
public:
    TimedQueue() : m_stop(false), m_running(false){};
    ~TimedQueue()
    {
        stop();
        waitUntilDone();
    }

    typedef std::function<bool(const Key &, const Info &)> ExpiredCallback;

    void reQueue(const Key &key, double delay, const Info &info)
    {
        if (exiting()) {
            stop();
            return;
        }
        bool notify = false;
        {
            std::lock_guard<std::mutex> lockGuard(m_mutex);
            if (m_stop) {
                return;
            }

            SteadyClock expired
            = SteadyClock::now() + std::chrono::microseconds((long long) (delay * 1000000));

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
            m_cond.notify_one();
        }
    }

    void remove(const Key &key)
    {
        {
            std::lock_guard<std::mutex> lockGuard(m_mutex);
            if (m_stop) {
                return;
            }
            m_list.erase(key);
        }
        if (exiting()) {
            stop();
            return;
        }
    }

    void stop()
    {
        {
            std::lock_guard<std::mutex> lockGuard(m_mutex);
            m_list.clear();
            m_stop = true;
        }
        m_cond.notify_one();
    }

    void waitUntilDone()
    {
        while (m_running.load())
            ;
    }

    void loop(const ExpiredCallback &onElementExpired)
    {
        m_running.store(true);
        while (!exiting()) {
            std::unique_lock<std::mutex> lockGuard(m_mutex);
            if (m_stop) {
                break;
            }
            if (m_list.elements().empty() && !exiting()) {
                m_cond.wait(lockGuard);
                continue;
            }
            SteadyClock now = SteadyClock::now();
            const auto &shortest = m_list.elements().begin();
            if (now < shortest->order && !exiting()) {
                m_cond.wait_for(lockGuard, shortest->order - now);
                continue;
            }
            Key key = shortest->key;
            Info info = shortest->value;
            lockGuard.unlock();
            bool erase = false;
            if (!exiting()) {
                erase = onElementExpired(key, info);
            }
            lockGuard.lock();
            if (erase) {
                m_list.erase(key);
            }
        }
        m_running.store(false);
    }

protected:
    OrderedUniqueList<Key, Info, SteadyClock> m_list;
    std::condition_variable m_cond;
    std::mutex m_mutex;
    bool m_stop;
    std::atomic<bool> m_running;
};

} //namespace WCDB

#endif /* __WCDB_TIMED_QUEUE_HPP */
