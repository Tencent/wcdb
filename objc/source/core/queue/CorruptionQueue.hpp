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

#ifndef _WCDB_CORRUPTIONQUEUE_HPP
#define _WCDB_CORRUPTIONQUEUE_HPP

#include <WCDB/AsyncQueue.hpp>
#include <WCDB/Lock.hpp>
#include <WCDB/TimedQueue.hpp>
#include <map>

namespace WCDB {

class CorruptionEvent : public AsyncQueue::Event {
public:
    virtual ~CorruptionEvent();

protected:
    virtual void databaseDidBecomeCorrupted(const String& path, uint32_t identifier) = 0;
    friend class CorruptionQueue;
};

class CorruptionQueue final : public AsyncQueue {
public:
    CorruptionQueue(const String& name, CorruptionEvent* event);
    ~CorruptionQueue();

protected:
    void handleError(const Error& error);

    void loop() override final;

    bool onTimed(const String& path, const uint32_t& identifier);

    TimedQueue<String, uint32_t> m_timedQueue;

    // identifier -> notify time
    SharedLock m_lock;
    std::map<uint32_t, SteadyClock> m_refractories;
};

} //namespace WCDB

#endif /* _WCDB_CORRUPTIONQUEUE_HPP */
