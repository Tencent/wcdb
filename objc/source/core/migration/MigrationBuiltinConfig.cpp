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

#include <WCDB/Core.h>

namespace WCDB {

const Config MigrationBuiltinConfig::migrationPreset(MigrationInfos *infos)
{
    WCTInnerAssert(infos != nullptr);
    return Config(
        "migration",
        [infos](Handle *handle) -> bool {
            MigrationHandle *migrationHandle =
                static_cast<MigrationHandle *>(handle);
            bool schemaChanged = false;
            if (!doAttachSchema(migrationHandle, infos) ||
                !doCreateView(migrationHandle, infos, schemaChanged)) {
                return false;
            }
            return schemaChanged ? doAttachSchema(migrationHandle, infos)
                                 : true;
        },
        MigrationBuiltinConfig::Order::Migration);
}

bool MigrationBuiltinConfig::doCreateView(MigrationHandle *handle,
                                          MigrationInfos *infos,
                                          bool &schemaChanged)
{
    schemaChanged = false;
    if (infos->isStarted()) {
        return true;
    }
    LockGuard lockGuard(infos->getSharedLock());
    if (infos->isStarted()) {
        return true;
    }
    std::list<std::shared_ptr<MigrationInfo>> migratedInfos;
    if (handle->runTransaction(
            [&handle, &migratedInfos, infos](Handle *) -> bool {
                for (const auto &info : infos->getInfos()) {
                    auto pair =
                        handle->isTableExists(info.second->getSourceTable());
                    if (!pair.first) {
                        return false;
                    }
                    //Create view
                    bool result = false;
                    if (pair.second) {
                        result = handle->executeWithoutTampering(
                            info.second->getStatementForCreatingUnionedView());
                    } else {
                        result = handle->executeWithoutTampering(
                            info.second->getStatementForDroppingUnionedView());
                        migratedInfos.push_back(info.second);
                    }
                    if (!result) {
                        return false;
                    }
                }
                return true;
            })) {
        for (const auto &info : migratedInfos) {
            if (infos->markAsMigrated(info->targetTable)) {
                schemaChanged = true;
            }
        }
        infos->markAsStarted();
        return true;
    }
    return false;
}

bool MigrationBuiltinConfig::doAttachSchema(MigrationHandle *handle,
                                            MigrationInfos *infos)
{
    SharedLockGuard lockGuard(infos->getSharedLock());
    auto attacheds = handle->getUnorderedAttachedSchemas();
    if (!attacheds.first) {
        return false;
    }

    auto &toDetachs = attacheds.second;
    for (const auto &schemas : infos->getSchemasForAttaching()) {
        auto iter = toDetachs.find(schemas.first);
        if (iter != toDetachs.end()) {
            //necessary
            toDetachs.erase(iter);
        } else {
            WCDB::StatementAttach statement = WCDB::StatementAttach()
                                                  .attach(schemas.second.first)
                                                  .as(schemas.first);
            if (!handle->executeWithoutTampering(statement)) {
                return false;
            }
        }
    }

    const std::string &schemaPrefix = MigrationInfo::schemaPrefix();
    for (const auto &toDetach : toDetachs) {
        if (toDetach.compare(0, schemaPrefix.length(), schemaPrefix) != 0) {
            continue;
        }
        //detach unnecessary WCDBMigration_ schema
        WCDB::StatementDetach statement =
            WCDB::StatementDetach().detach(toDetach);
        if (!handle->executeWithoutTampering(statement)) {
            return false;
        }
    }
    return true;
}

} //namespace WCDB
