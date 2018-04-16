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

#include <WCDB/MigrationInfos.hpp>
#include <WCDB/MigrationTamperer.hpp>

namespace WCDB {

#pragma mark - Initialize
std::shared_ptr<MigrationInfos>
MigrationInfos::infos(const std::list<std::shared_ptr<MigrationInfo>> &infos)
{
    return std::shared_ptr<MigrationInfos>(new MigrationInfos(infos));
}

MigrationInfos::MigrationInfos(
    const std::list<std::shared_ptr<MigrationInfo>> &infos)
    : m_migratingStarted(false)
{
    assert(!infos.empty());
    for (const auto &info : infos) {
        if (!info->isSameDatabaseMigration()) {
            auto iter = m_schemas.find(info->schema);
            if (iter == m_schemas.end()) {
                iter =
                    m_schemas
                        .insert({info->schema, {info->sourceDatabasePath, 0}})
                        .first;
            }
            ++iter->second.second;
        }
        assert(m_infos.find(info->targetTable) == m_infos.end());
        m_infos.insert({info->targetTable, info});
    }
}

#pragma mark - Basic
bool MigrationInfos::isSameDatabaseMigration() const
{
    return m_schemas.empty();
}

const std::map<std::string, std::pair<std::string, int>> &
MigrationInfos::getSchemasForAttaching() const
{
    assert(m_lock.debug_isSharedLocked());
    return m_schemas;
}

const std::map<std::string, std::shared_ptr<MigrationInfo>> &
MigrationInfos::getInfos() const
{
    return m_infos;
}

std::shared_ptr<MigrationInfo> MigrationInfos::getMigratingInfo()
{
    return m_migratingInfo;
}

bool MigrationInfos::didMigratingStart() const
{
    return m_migratingStarted.load();
}

bool MigrationInfos::didMigrationDone() const
{
    return m_infos.empty();
}

void MigrationInfos::markAsMigrationStarted()
{
    assert(m_lock.isLocked());
    m_migratingStarted.store(true);
}

void MigrationInfos::markAsMigrationStarted(const std::string &table)
{
    assert(m_lock.isLocked());
    m_migratingStarted.store(true);
    assert(!table.empty());
    auto iter = m_infos.find(table);
    assert(iter != m_infos.end());
    m_migratingInfo = iter->second;
}

void MigrationInfos::markAsMigrating(const std::string &table)
{
    assert(m_lock.isLocked());
    m_migratingStarted.store(true);
    assert(!table.empty());
    auto iter = m_infos.find(table);
    if (iter != m_infos.end()) {
        m_migratingInfo = iter->second;
    }
}

void MigrationInfos::markAsMigrated(bool &schemasChanged)
{
    assert(m_lock.isLocked());
    assert(m_migratingInfo != nullptr);
    auto iter = m_infos.find(m_migratingInfo->targetTable);
    assert(iter != m_infos.end());
    schemasChanged = false;
    if (!iter->second->isSameDatabaseMigration()) {
        auto schemaIter = m_schemas.find(iter->second->schema);
        assert(schemaIter != m_schemas.end());
        if (--schemaIter->second.second == 0) {
            m_schemas.erase(schemaIter);
            schemasChanged = true;
        }
    }
    m_infos.erase(iter);
    m_migratingInfo = nullptr;
}

SharedLock &MigrationInfos::getSharedLock()
{
    return m_lock;
}

} //namespace WCDB
