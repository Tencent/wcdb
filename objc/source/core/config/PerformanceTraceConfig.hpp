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

#ifndef PerformanceTraceConfig_hpp
#define PerformanceTraceConfig_hpp

#include <WCDB/Config.hpp>
#include <WCDB/Lock.hpp>

namespace WCDB {

class PerformanceTraceConfig : public Config {
public:
    static const std::shared_ptr<Config> &shared();

    PerformanceTraceConfig(const std::string &name,
                           const Handle::PerformanceNotification &trace);
    bool invoke(Handle *handle) override;

protected:
    Handle::PerformanceNotification m_performanceTrace;
};

class SharedPerformanceTraceConfig : public PerformanceTraceConfig {
public:
    static const std::shared_ptr<Config> &shared();

    SharedPerformanceTraceConfig() = delete;
    SharedPerformanceTraceConfig(const SharedPerformanceTraceConfig &) = delete;
    SharedPerformanceTraceConfig &
    operator=(const SharedPerformanceTraceConfig &) = delete;

    bool invoke(Handle *handle) override;

    void setPerformanceTrace(const Handle::PerformanceNotification &trace);

protected:
    using PerformanceTraceConfig::PerformanceTraceConfig;
    mutable SharedLock m_lock;
};

} //namespace WCDB

#endif /* PerformanceTraceConfig_hpp */
