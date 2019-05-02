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

#ifndef __WCDB_BACKUP_QUEUE_HPP
#define __WCDB_BACKUP_QUEUE_HPP

#include <WCDB/AsyncQueue.hpp>
#include <WCDB/Lock.hpp>
#include <WCDB/TimedQueue.hpp>
#include <map>

namespace WCDB {

class BackupQueueEvent {
public:
    virtual ~BackupQueueEvent() = 0;

protected:
    virtual bool databaseShouldBackup(const String& path) = 0;
    friend class BackupQueue;
};

class BackupQueue final : public AsyncQueue {
public:
    BackupQueue(const String& name, BackupQueueEvent* event);

    void put(const String& path, int frames);
    void register_(const String& path);
    void unregister(const String& path);

protected:
    bool onTimed(const String& path, const int& frames);
    void loop() override final;

    TimedQueue<String, int> m_timedQueue;
    SharedLock m_lock;

    // path -> frame
    std::map<String, int> m_records;
    BackupQueueEvent* m_event;
};

} // namespace WCDB

#endif /* __WCDB_BACKUP_QUEUE_HPP */
