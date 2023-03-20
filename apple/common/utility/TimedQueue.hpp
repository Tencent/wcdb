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

#include "Assertion.hpp"
#include "Exiting.hpp"
#include "Time.hpp"
#include "UniqueList.hpp"
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

    enum class Mode {
        ForwardOnly,
        ReQueue,
    };

    void queue(const Key &key, double delay, const Info &info, Mode mode = Mode::ForwardOnly)
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

            auto iter = m_list.find(key);
            if (mode == Mode::ForwardOnly && iter != m_list.end() && iter->order() < expired) {
                iter->value() = info;
            } else {
                m_list.insert(key, info, expired);
                notify = m_list.front().key() == key;
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
                auto shortest = m_list.begin();
                if (shortest == m_list.end()) {
                    if (!isExiting()) {
                        m_conditional.wait(lockGuard);
                    }
                    continue;
                }
                double timeInterval = shortest->order().timeIntervalSinceNow();
                if (timeInterval > 0) {
                    if (!isExiting()) {
                        m_conditional.wait_for(lockGuard, timeInterval);
                    }
                    continue;
                }
                expireds.push_back(std::make_pair(shortest->key(), shortest->value()));
                m_list.erase(shortest);
            }
            if (!isExiting()) {
                WCTAssert(expireds.size() == 1);
                onElementExpired(expireds.begin()->first, expireds.begin()->second);
            }
        }
        m_running.store(false);
    }
};

} //namespace WCDB
