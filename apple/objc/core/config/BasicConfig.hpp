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

#ifndef __WCDB_BASIC_CONFIG_HPP
#define __WCDB_BASIC_CONFIG_HPP

#include <WCDB/Config.hpp>
#include <WCDB/WINQ.h>

namespace WCDB {

class BasicConfig final : public Config {
public:
    BasicConfig();
    bool invoke(Handle* handle) override final;

private:
    bool executeStatement(Handle* handle, const StatementPragma& statement, int& numberOfRemainingAttempts);

    const StatementPragma m_journalModeWAL;
    const StatementPragma m_lockingModeNormal;
    const StatementPragma m_synchronousNormal;
    const StatementPragma m_fullfsync;
};

} //namespace WCDB

#endif /* __WCDB_BASIC_CONFIG_HPP */
