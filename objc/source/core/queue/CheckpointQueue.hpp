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

#ifndef CheckpointQueue_hpp
#define CheckpointQueue_hpp

#include <WCDB/AsyncQueue.hpp>
#include <WCDB/TimedQueue.hpp>

namespace WCDB {

class CheckpointQueue : public AsyncQueue {
public:
    CheckpointQueue(const std::string& name);
    ~CheckpointQueue();

    static constexpr const int framesForFull = 10 * 1024;

    void put(const std::string& path, double delay, int frames);
    void remove(const std::string& path);

protected:
    bool onTimed(const std::string& path, const int& frames);
    void loop() override;

    TimedQueue<std::string, int> m_timedQueue;
    const StatementPragma m_checkpointPassive;
    const StatementPragma m_checkpointTruncate;
};

} // namespace WCDB

#endif /* CheckpointQueue_hpp */
