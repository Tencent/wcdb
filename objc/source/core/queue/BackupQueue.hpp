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
    BackupQueue(const std::string& name, const std::shared_ptr<DatabasePool>& databasePool);
    ~BackupQueue();
    void put(const std::string& path, double delay, int frames);
    int getBackedUpFrames(const std::string& path);

protected:
    bool onTimed(const std::string& path, const int& frames);
    void loop() override final;

    std::shared_ptr<DatabasePool> m_databasePool;
    TimedQueue<std::string, int> m_timedQueue;
    SharedLock m_lock;
    std::map<std::string, int> m_backedUp;
};

} // namespace WCDB

#endif /* BackupQueue_hpp */
