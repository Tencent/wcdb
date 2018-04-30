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

#ifndef BuiltinConfig_hpp
#define BuiltinConfig_hpp

#include <WCDB/Config.hpp>
#include <WCDB/TimedQueue.hpp>

#pragma GCC visibility push(hidden)

namespace WCDB {

class BuiltinConfig {
public:
    static BuiltinConfig *shared();

    enum Order : int {
        Trace = INT_MIN,
        Cipher = INT_MIN + 1,
        Basic = INT_MIN + 2,
        Tokenize = INT_MIN + 3,
        Checkpoint = 1,
    };

    const Config basic;

    const Config trace;

    const Config checkpoint;

    const Configs defaultConfigs;

    void setGlobalPerformanceTrace(
        const Handle::PerformanceTraceCallback &globalTrace);

    void setGlobalSQLTrace(const Handle::SQLTraceCallback &globalTrace);

    Config cipherWithKey(const NoCopyData &cipher, int pageSize);

    Config tokenizeWithNames(const std::list<std::string> &names);

protected:
    BuiltinConfig();

    TimedQueue<std::string, const int> m_timedQueue;

    std::shared_ptr<Handle::PerformanceTraceCallback> m_globalPerformanceTrace;

    std::shared_ptr<Handle::SQLTraceCallback> m_globalSQLTrace;

    static bool basicConfig(Handle *handle);
    static bool traceConfig(Handle *handle);
    static bool checkpointConfig(Handle *handle);

    void stopTimedQueue();
    void loopTimedQueue(const std::atomic<bool> &stop);
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* BuiltinConfig_hpp */
