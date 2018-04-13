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

#include <WCDB/MigrationBuiltinConfig.hpp>
#include <WCDB/MigrationHandle.hpp>

namespace WCDB {

const Config
MigrationBuiltinConfig::autoAttachAndDetachWithInfos(MigrationInfos *infos)
{
    assert(infos != nullptr);
    return Config(
        "migration",
        [infos](Handle *handle) -> bool {
            MigrationHandle *migrationHandle =
                static_cast<MigrationHandle *>(handle);
            return MigrationBuiltinConfig::doAutoAttachAndDetachWithInfos(
                migrationHandle, infos);
        },
        MigrationBuiltinConfig::Order::Migration);
}

bool MigrationBuiltinConfig::doAutoAttachAndDetachWithInfos(
    MigrationHandle *handle, MigrationInfos *infos)
{
    SharedLockGuard lockGuard(infos->getSharedLock());    
    auto pair = handle->getAttachedSchemas();
    if (!pair.first) {
        return false;
    }
    std::map<std::string, std::pair<std::string, int>> toAttacheds =
        infos->getSchemasForAttaching();
    const std::string &schemaPrefix = MigrationInfo::schemaPrefix();
    for (const auto &schema : pair.second) {
        if (schema.compare(0, schemaPrefix.length(), schemaPrefix) == 0) {
            //WCDBMigration_ schema
            auto iter = toAttacheds.find(schema);
            if (iter != toAttacheds.end()) {
                //already attached
                toAttacheds.erase(iter);
            } else {
                //detach
                WCDB::StatementDetach statement =
                    WCDB::StatementDetach().detach(schema);
                if (!handle->executeWithoutTampering(statement)) {
                    return false;
                }
            }
        }
    }
    for (const auto &toAttached : toAttacheds) {
        WCDB::StatementAttach statement = WCDB::StatementAttach()
                                              .attach(toAttached.second.first)
                                              .as(toAttached.first);
        if (!handle->executeWithoutTampering(statement)) {
            return false;
        }
    }
    return true;
}

} //namespace WCDB
