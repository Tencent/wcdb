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

#ifndef BasicConfig_hpp
#define BasicConfig_hpp

#include <WCDB/Config.hpp>

#pragma GCC visibility push(hidden)

namespace WCDB {

class BasicConfig : public Config {
public:
    BasicConfig(const std::string& name);
    bool invoke(Handle* handle) override;

protected:
    const StatementPragma m_getJournalMode;
    const StatementPragma m_getLockingMode;
    const StatementPragma m_setSynchronousNormal;
    const StatementPragma m_setJournalModeWAL;
    const StatementPragma m_setFullFSync;
    const StatementPragma m_setLockingModeNormal;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* BasicConfig_hpp */
