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

#include <chrono>
#include <condition_variable>
#include <list>
#include <mutex>
#include <stdio.h>
#include <string>
#include <thread>
#include <unordered_map>

#pragma GCC visibility push(hidden)

namespace WCDB {

template <typename Key, typename Info>
class TimedQueue {
public:
    TimedQueue(double delay)
        : m_delay(std::chrono::microseconds((long long) (delay * 1000000)))
        , m_stop(false){};

    typedef std::function<void(const Key &, const Info &)> ExpiredCallback;

    void reQueue(const Key &key, const Info &info)
    {
        bool notify = false;
        {
            std::lock_guard<std::mutex> lockGuard(m_mutex);
            if (m_stop) {
                return;
            }

            notify = m_list.empty();

            auto iter = m_map.find(key);
            if (iter != m_map.end()) {
                m_list.erase(iter->second);
                m_map.erase(iter);
            }

            //delay
            Element element(key, std::chrono::steady_clock::now() + m_delay,
                            info);
            m_list.push_back(std::move(element));
            auto last = m_list.end();
            std::advance(last, -1);
            m_map.insert({key, last});
        }
        if (notify) {
            m_cond.notify_one();
        }
    }

    void stop()
    {
        {
            std::lock_guard<std::mutex> lockGuard(m_mutex);
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
            if (m_list.empty()) {
                m_cond.wait(lockGuard);
                continue;
            }
            Element &element = m_list.front();
            Time now = std::chrono::steady_clock::now();
            if (now < element.expired) {
                m_cond.wait_for(lockGuard, element.expired - now);
                continue;
            }
            Element expired = std::move(element);
            m_list.pop_front();
            m_map.erase(expired.key);
            lockGuard.unlock();
            onElementExpired(expired.key, expired.info);
            lockGuard.lock();
        }
        m_running.store(false);
    }

protected:
    using Time = std::chrono::steady_clock::time_point;
    struct Element {
        Element(const Key &key_, const Time &expired_, const Info &info_)
            : key(key_), expired(expired_), info(info_)
        {
        }
        Key key;
        Time expired;
        Info info;
    };
    typedef struct Element Element;
    //TODO refactor
    using List = std::list<Element>;
    using Map = std::unordered_map<Key, typename List::iterator>;
    Map m_map;
    List m_list;
    std::condition_variable m_cond;
    std::mutex m_mutex;
    std::chrono::microseconds m_delay;
    bool m_stop;
    std::atomic<bool> m_running;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* TimedQueue_hpp */
