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

#include <WCDB/Assertion.hpp>
#include <WCDB/MigrationInfo.hpp>

namespace WCDB {

MigrationInfo::MigrationInfo(const std::string& migratedTable_,
                             const std::string& originTable_,
                             const std::string& originDatabase_)
: originTable(originTable_)
, migratedTable(migratedTable_)
, originDatabase(originDatabase_)
, m_state(State::None)
{
    WCTInnerAssert(!originTable.empty());
    WCTInnerAssert(!migratedTable.empty());
}

MigrationInfo::State MigrationInfo::getState() const
{
    SharedLockGuard lockGuard(m_lock);
    return m_state;
}

void MigrationInfo::markAsMigrated()
{
    LockGuard lockGuard(m_lock);
    m_state = State::Migrated;
}

void MigrationInfo::initialize(const std::list<std::string>& columns)
{
    WCTInnerAssert(!isInitialized());
    LockGuard lockGuard(m_lock);
    //TODO
    m_state = State::Initialized;
}

bool MigrationInfo::isSameDatabaseMigration() const
{
    WCTInnerAssert(isInitialized());
    SharedLockGuard lockGuard(m_lock);
    return m_schemaForOriginDatabase == Schema::main();
}

bool MigrationInfo::isInitialized() const
{
    return m_state == State::Initialized;
}

} // namespace WCDB
