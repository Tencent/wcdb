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

#ifndef TimedQueue_hpp
#define TimedQueue_hpp

#include <WCDB/Time.hpp>
#include <chrono>
#include <condition_variable>
#include <list>
#include <map>
#include <mutex>
#include <stdio.h>
#include <string>
#include <thread>

#pragma GCC visibility push(hidden)

namespace WCDB {

template<typename Key, typename Info>
class TimedQueue {
public:
    TimedQueue() : m_stop(false), m_running(false){};

    typedef std::function<void(const Key &, const Info &)> ExpiredCallback;

    void reQueue(const Key &key, double delay, const Info &info)
    {
        bool notify = false;
        {
            std::lock_guard<std::mutex> lockGuard(m_mutex);
            if (m_stop) {
                return;
            }

            SteadyClock expired
            = std::chrono::steady_clock::now()
              + std::chrono::microseconds((long long) (delay * 1000000));

            auto iter = m_map.find(key);
            if (iter != m_map.end()) {
                iter->second.expired = expired;
                iter->second.info = info;
                notify = true;
            } else {
                Element element(key, expired, info);
                m_map.insert({ key, std::move(element) });
                std::pair<Key, Element> min
                = *std::min_element(m_map.begin(), m_map.end(), &TimedQueue::compare);
                if (min.first == key) {
                    notify = true;
                }
            }
        }
        if (notify) {
            m_cond.notify_one();
        }
    }

    void remove(const Key &key)
    {
        std::lock_guard<std::mutex> lockGuard(m_mutex);
        if (m_stop) {
            return;
        }
        m_map.erase(key);
    }

    void stop()
    {
        {
            std::lock_guard<std::mutex> lockGuard(m_mutex);
            m_map.clear();
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
        while (true) {
            std::unique_lock<std::mutex> lockGuard(m_mutex);
            if (m_stop) {
                break;
            }
            if (m_map.empty()) {
                m_cond.wait(lockGuard);
                continue;
            }
            std::pair<Key, Element> min
            = *std::min_element(m_map.begin(), m_map.end(), &TimedQueue::compare);
            SteadyClock now = SteadyClock::now();
            if (now < min.second.expired) {
                m_cond.wait_for(lockGuard, min.second.expired - now);
                continue;
            }
            m_map.erase(min.first);
            lockGuard.unlock();
            onElementExpired(min.first, min.second.info);
            lockGuard.lock();
        }
        m_running.store(false);
    }

protected:
    struct Element {
        Element(const Key &key_, const SteadyClock &expired_, const Info &info_)
        : key(key_), expired(expired_), info(info_)
        {
        }
        Key key;
        SteadyClock expired;
        Info info;
    };
    typedef struct Element Element;
    typedef std::map<Key, Element> Map;

    static bool
    compare(const std::pair<Key, Element> &lhs, const std::pair<Key, Element> &rhs)
    {
        return lhs.second.expired < rhs.second.expired;
    }

    Map m_map;
    std::condition_variable m_cond;
    std::mutex m_mutex;
    bool m_stop;
    std::atomic<bool> m_running;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* TimedQueue_hpp */
