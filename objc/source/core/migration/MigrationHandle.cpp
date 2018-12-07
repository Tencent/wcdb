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
#include <WCDB/MigrationHandle.hpp>
#include <WCDB/MigrationInfo.hpp>
#include <WCDB/String.hpp>

namespace WCDB {

#pragma mark - Initialize
MigrationHandle::MigrationHandle(const String& path, Migration& migration)
: Handle(path), Migration::Binder(migration)
{
}

bool MigrationHandle::rebind(const std::set<const MigrationInfo*>& migratings)
{
    bool succeed;

    // views
    std::map<String, const MigrationInfo*> infosToCreateView; // view -> info
    for (const auto& migrating : migratings) {
        infosToCreateView.emplace(migrating->getUnionedView(), migrating);
    }

    std::set<String> createdViews;
    std::tie(succeed, createdViews)
    = getUnorderedValues(MigrationInfo::getStatementForSelectingUnionedView(), 0);
    if (!succeed) {
        return false;
    }
    for (const auto& createdView : createdViews) {
        WCTInnerAssert(createdView.hasPrefix(MigrationInfo::getUnionedViewPrefix()));
        auto iter = infosToCreateView.find(createdView);
        if (iter != infosToCreateView.end()) {
            // it is already created
            infosToCreateView.erase(iter);
        } else {
            // it is no longer needed
            if (!execute(MigrationInfo::getStatementForDroppingUnionedView(createdView))) {
                return false;
            }
        }
    }
    // create all needed views
    for (const auto& iter : infosToCreateView) {
        const MigrationInfo* infoToCreateView = iter.second;
        if (!execute(infoToCreateView->getStatementForCreatingUnionedView())) {
            return false;
        }
    }

    // schemas
    std::map<String, const MigrationInfo*> infosToAttachSchema; // schema -> info
    for (const auto& migrating : migratings) {
        if (!migrating->isSameDatabaseMigration()) {
            infosToAttachSchema.emplace(
            migrating->getSchemaForOriginDatabase().getDescription(), migrating);
        }
    }

    std::set<String> attachedSchemas;
    std::tie(succeed, attachedSchemas)
    = getUnorderedValues(MigrationInfo::getStatementForSelectingDatabaseList(), 1);
    if (!succeed) {
        return false;
    }

    for (const auto& attachedSchema : attachedSchemas) {
        if (attachedSchema.hasPrefix(MigrationInfo::getSchemaPrefix())) {
            auto iter = infosToAttachSchema.find(attachedSchema);
            if (iter != infosToAttachSchema.end()) {
                // it is already attached
                infosToAttachSchema.erase(iter);
            } else {
                // it is not longer needed
                if (!execute(MigrationInfo::getStatementForDetachingSchema(attachedSchema))) {
                    return false;
                }
            }
        }
    }
    // attach all needed schemas
    for (const auto& iter : infosToAttachSchema) {
        const MigrationInfo* infoToAttachSchema = iter.second;
        if (!execute(infoToAttachSchema->getStatementForAttachingSchema())) {
            return false;
        }
    }

    return true;
}

#pragma mark - Override
bool MigrationHandle::execute(const Statement& statement)
{
    return Handle::execute(statement);
}

bool MigrationHandle::prepare(const Statement& statement)
{
    return Handle::prepare(statement);
}

bool MigrationHandle::step(bool& done)
{
    return Handle::step(done);
}

void MigrationHandle::reset()
{
    Handle::reset();
}

void MigrationHandle::bindInteger32(const Integer32& value, int index)
{
    Handle::bindInteger32(value, index);
}

void MigrationHandle::bindInteger64(const Integer64& value, int index)
{
    Handle::bindInteger64(value, index);
}

void MigrationHandle::bindDouble(const Float& value, int index)
{
    Handle::bindDouble(value, index);
}

void MigrationHandle::bindText(const Text& value, int length, int index)
{
    Handle::bindText(value, length, index);
}

void MigrationHandle::bindBLOB(const BLOB& value, int index)
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
