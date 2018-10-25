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

#ifndef BackupQueue_hpp
#define BackupQueue_hpp

#include <WCDB/AsyncQueue.hpp>
#include <WCDB/Lock.hpp>
#include <WCDB/TimedQueue.hpp>
#include <map>

namespace WCDB {

class DatabasePool;

class BackupQueue final : public AsyncQueue {
public:
    BackupQueue(const String& name, const std::shared_ptr<DatabasePool>& databasePool);
    ~BackupQueue();
    void put(const String& path, double delay, int frames);
    int getBackedUpFrames(const String& path);

protected:
    bool onTimed(const String& path, const int& frames);
    void loop() override final;

    std::shared_ptr<DatabasePool> m_databasePool;
    TimedQueue<String, int> m_timedQueue;
    SharedLock m_lock;
    std::map<String, int> m_backedUp;
};

} // namespace WCDB

#endif /* BackupQueue_hpp */
