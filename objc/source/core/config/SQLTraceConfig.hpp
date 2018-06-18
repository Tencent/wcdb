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

#ifndef SQLTraceConfig_hpp
#define SQLTraceConfig_hpp

#include <WCDB/Config.hpp>
#include <WCDB/Lock.hpp>

namespace WCDB {

class SQLTraceConfig : public Config {
public:
    SQLTraceConfig(const std::string &name,
                   const Handle::SQLNotification &trace);

    bool invoke(Handle *handle) override;

protected:
    Handle::SQLNotification m_sqlTrace;
};

class SharedSQLTraceConfig : public SQLTraceConfig {
public:
    static const std::shared_ptr<Config> &shared();

    bool invoke(Handle *handle) override;

    void setSQLTrace(const Handle::SQLNotification &trace);

    SharedSQLTraceConfig() = delete;
    SharedSQLTraceConfig(const SharedSQLTraceConfig &) = delete;
    SharedSQLTraceConfig &operator=(const SharedSQLTraceConfig &) = delete;

protected:
    using SQLTraceConfig::SQLTraceConfig;
    mutable SharedLock m_lock;
};

} //namespace WCDB

#endif /* SQLTraceConfig_hpp */
