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

#include <WCDB/Assertion.hpp>
#include <WCDB/Exiting.hpp>
#include <WCDB/Time.hpp>
#include <WCDB/UniqueList.hpp>
#include <condition_variable>
#include <list>
#include <stdio.h>

namespace WCDB {

template<typename Key, typename Info>
class TimedQueue final {
private:
    typedef UniqueList<Key, Info, SteadyClock> List;
    List m_list;
    Conditional m_conditional;
    std::mutex m_lock;
    bool m_stop;
    std::atomic<bool> m_running;

public:
    TimedQueue() : m_stop(false), m_running(false) {}
    ~TimedQueue()
    {
        stop();
        waitUntilDone();
    }

    // return true to erase the element
    typedef std::function<void(const Key &, const Info &)> ExpiredCallback;

    void queue(const Key &key, double delay, const Info &info)
    {
        if (isExiting()) {
            stop();
            return;
        }
        bool notify = false;
        {
            SteadyClock expired
            = SteadyClock::now().steadyClockByAddingTimeInterval(delay);

            std::lock_guard<std::mutex> lockGuard(m_lock);
            if (m_stop) {
                return;
            }

            auto *element = m_list.find(key);
            if (element == nullptr || (element != nullptr && expired < element->order)) {
                // insert if key doesn't exist or new key is arrived faster.
                m_list.insert(expired, key, info);
                notify = m_list.elements().front().key == key;
            }
        }
        if (notify) {
            m_conditional.notify_one();
        }
    }

    void remove(const Key &key)
    {
        {
            std::lock_guard<std::mutex> lockGuard(m_lock);
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
            std::lock_guard<std::mutex> lockGuard(m_lock);
            m_list.clear();
            m_stop = true;
        }
        m_conditional.notify_one();
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
            std::list<std::pair<Key, Info>> expireds;
            {
                std::unique_lock<std::mutex> lockGuard(m_lock);
                if (m_stop) {
                    break;
                }
                if (m_list.elements().empty() && !isExiting()) {
                    m_conditional.wait(lockGuard);
                    continue;
                }
                SteadyClock now = SteadyClock::now();
                const auto &shortest = m_list.elements().begin();
                if (now < shortest->order && !isExiting()) {
                    m_conditional.wait_for(
                    lockGuard, shortest->order.timeIntervalSinceSteadyClock(now));
                    continue;
                }
                expireds.push_back(std::make_pair(shortest->key, shortest->value));
                m_list.erase(shortest->key);
            }
            if (!isExiting()) {
                WCTInnerAssert(expireds.size() == 1);
                onElementExpired(expireds.begin()->first, expireds.begin()->second);
            }
        }
        m_running.store(false);
    }
};

} //namespace WCDB
