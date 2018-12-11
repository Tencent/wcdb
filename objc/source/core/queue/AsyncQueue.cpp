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
#include <WCDB/AsyncQueue.hpp>
#include <WCDB/Dispatch.hpp>
#include <WCDB/String.hpp>
#include <atomic>

namespace WCDB {

AsyncQueue::Event::~Event()
{
}

AsyncQueue::AsyncQueue(const String& name_, Event* event)
: name(name_), m_running(false), m_started(false), m_event(event)
{
    WCTInnerAssert(event != nullptr);
}

AsyncQueue::~AsyncQueue()
{
    std::unique_lock<std::mutex> lockGuard(m_mutex);
    while (m_running) {
        // wait until done
        m_cond.wait_for(lockGuard, std::chrono::seconds(10));
    }
    WCTRemedialAssert(
    !m_running, String::formatted("Queue: %s does not exit on time.", name.c_str()), ;);
}

void AsyncQueue::run()
{
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    if (!m_started) {
        m_started = true;
        Dispatch::async(name, std::bind(&AsyncQueue::willRun, this));
    }
}

void AsyncQueue::willRun()
{
    m_running.store(true);
    loop();
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    m_running.store(false);
    m_cond.notify_one();
}

void AsyncQueue::lazyRun()
{
    // lock free if it's already running
    if (!m_running.load()) {
        run();
    }
}

bool AsyncQueue::exit()
{
    static std::atomic<bool>* s_exit = new std::atomic<bool>(false);
    static auto s_dummy __attribute__((unused)) = []() -> std::nullptr_t {
        atexit([]() { s_exit->store(true); });
        return nullptr;
    }();
    return s_exit->load();
}

} // namespace WCDB
