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
#include <WCDB/Core.h>
#include <WCDB/MigrationInfos.hpp>

namespace WCDB {

#pragma mark - Initialize
MigrationHandle::MigrationHandle(const std::string &path, const MigrationInfos const *infos)
: Handle(path), m_infos(infos)
{
    WCTInnerAssert(m_infos != nullptr);
}

std::pair<bool, std::set<std::string>> MigrationHandle::getAllExistingTables()
{
    // PRAGMA table_info
    return { false, {} };
}

std::pair<bool, std::set<std::string>>
MigrationHandle::getAllColumns(const std::string &schema, const std::string &table)
{
    // return {true, {}} if the schema.table does not exist.

    // attach schema, PRAGMA table_info, detach schema
    return { false, {} };
}

//    bool MigrationHandle::initializeInfo(MigrationInfo& info)
//    {
//        WCTInnerAssert(!isPrepared());
//        WCTInnerAssert(!info.isInitialized());
//        WCTInnerAssert(info.shouldMigrate());
//
//        // resolve path and same database migration
//        std::string originDatabase = info.getOriginDatabase();
//        if (!originDatabase.empty()) {
//            originDatabase = Path::normalize(originDatabase);
//            if (originDatabase == path) {
//                originDatabase.clear();
//            }
//            if (originDatabase != info.getOriginDatabase()) {
//                info.setOrigin(info.getOriginTable(), originDatabase);
//            }
//        }
//
//        bool attached = false;
//        if (!info.isSameDatabaseMigration()) {
//            // Attach if needed
//            if (!execute(info.getStatementForAttachingSchema())) {
//                return false;
//            }
//            attached = true;
//        }
//
//        bool succeed = false;
//        do {
//            bool exists;
//            std::tie(succeed, exists) = tableExists(TableOrSubquery(info.getOriginTable()).withSchema(info.getSchemaForOriginDatabase()));
//            if (!succeed) {
//                break;
//            }
//            if (!exists) {
//                // an un-existing origin table means that migration is completed.
//                break;
//            }
//
//            std::set<std::string> columns;
//            std::tie(succeed, columns) = getUnorderedColumnsWithTable(info.getOriginTable(), info.getSchemaForOriginDatabase());
//            if (!succeed) {
//                break;
//            }
//            info.initialize(columns);
//        } while (false);
//
//        if (attached) {
//            // Detach if needed.
//            if (!execute(info.getStatementForDetachingSchema())) {
//                succeed = false;
//            }
//        }
//
//        return succeed;
//    }
//
//    bool MigrationHandle::resolveMigrationInfos(const std::set<const MigrationInfo*>& infos)
//    {
//        // 传进来的是这次 SQL 找出来的 info
//
//        // 新问题：如果这个找出来，是在 transaction 中，那么 创建的 temp view 会被 transaction 一起 rollback 掉，怎么解决？
//        // ATTACH 也不能在 transaction 里进行
//        // 旧版是不是也有这个问题
//
//        // 传进来的 infos 都要 attach
//        // 找出不需要的 detach 掉
//        // 有需要 attach 的时候，才主动去 databaseList 找 detachable，否则只从 list 里找
//        bool renew = false;
//        for (const auto& info : infos) {
//            if (m_attachedInfos.emplace(info).second) {
//                renew = true;
//            }
//        }
//
//        std::set<const MigrationInfo*> detachable;
//        if (renew) {
//            bool succeed;
//            std::set<std::string> attached;
//            std::tie(succeed, attached) = getUnorderedAttachedSchemas();
//            if (!succeed) {
//                return false;
//            }
//            detachable = m_infos->getDetachableInfos(attached);
//        }else {
//            detachable = m_infos->getDetachableInfos(m_attachedInfos);
//        }
//        for (const auto& ; <#condition#>; <#increment#>) {
//            <#statements#>
//        }
//
//    }

#pragma mark - Override
bool MigrationHandle::execute(const Statement &statement)
{
    return Handle::execute(statement);
}

bool MigrationHandle::prepare(const Statement &statement)
{
    return Handle::prepare(statement);
}

bool MigrationHandle::step(bool &done)
{
    return Handle::step(done);
}

void MigrationHandle::reset()
{
    Handle::reset();
}

void MigrationHandle::bindInteger32(const Integer32 &value, int index)
{
    Handle::bindInteger32(value, index);
}

void MigrationHandle::bindInteger64(const Integer64 &value, int index)
{
    Handle::bindInteger64(value, index);
}

void MigrationHandle::bindDouble(const Float &value, int index)
{
    Handle::bindDouble(value, index);
}

void MigrationHandle::bindText(const Text &value, int length, int index)
{
    Handle::bindText(value, length, index);
}

void MigrationHandle::bindBLOB(const BLOB &value, int index)
{
    Handle::bindBLOB(value, index);
}

void MigrationHandle::bindNull(int index)
{
    Handle::bindNull(index);
}

void MigrationHandle::finalize()
{
    Handle::finalize();
}

} //namespace WCDB
