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

static_assert((int) MigrationBuiltinConfig::Order::Migration >
                  (int) BuiltinConfig::Order::Trace,
              "");
static_assert((int) MigrationBuiltinConfig::Order::Migration >
                  (int) BuiltinConfig::Order::Basic,
              "");
static_assert((int) MigrationBuiltinConfig::Order::Migration >
                  (int) BuiltinConfig::Order::Cipher,
              "");
static_assert((int) MigrationBuiltinConfig::Order::Migration >
                  (int) BuiltinConfig::Order::Tokenize,
              "");

const std::string &MigrationBuiltinConfig::migrationConfigName()
{
    static const std::string s_migrationConfigName = "migration";
    return s_migrationConfigName;
}

const Config
MigrationBuiltinConfig::migrationWithSetting(MigrationSetting *setting)
{
    WCTInnerAssert(setting != nullptr);
    return Config(MigrationBuiltinConfig::migrationConfigName(),
                  [setting](Handle *handle) -> bool {
                      bool schemaChanged = false;
                      if (!doAttachSchema(handle, setting) ||
                          !doCreateView(handle, setting, schemaChanged)) {
                          return false;
                      }
                      return schemaChanged ? doAttachSchema(handle, setting)
                                           : true;
                  },
                  MigrationBuiltinConfig::Order::Migration);
}

bool MigrationBuiltinConfig::doCreateView(Handle *handle,
                                          MigrationSetting *setting,
                                          bool &schemaChanged)
{
    schemaChanged = false;
    std::list<std::shared_ptr<MigrationInfo>> infos;
    {
        SharedLockGuard lockGuard(setting->getSharedLock());
        if (!handle->runTransaction([&infos, setting](Handle *handle) -> bool {
                for (const auto &info : setting->getInfos()) {
                    auto pair =
                        handle->isTableExists(info.second->getSourceTable());
                    if (!pair.first) {
                        return false;
                    }
                    if (pair.second) {
                        //Create view
                        if (!handle->execute(
                                info.second
                                    ->getStatementForCreatingUnionedView())) {
                            return false;
                        }
                    } else {
                        infos.push_back(info.second);
                    }
                }
                return true;
            })) {
            return false;
        }
    }
    if (!infos.empty()) {
        LockGuard lockGuard(setting->getSharedLock());
        if (!handle->runTransaction([&infos](Handle *handle) {
                for (const auto &info : infos) {
                    if (!handle->execute(
                            info->getStatementForDroppingUnionedView())) {
                        return false;
                    }
                }
                return true;
            })) {
            return false;
        }
        for (const auto &info : infos) {
            schemaChanged =
                setting->markAsMigrated(info->targetTable) || schemaChanged;
        }
    }
    return true;
}

bool MigrationBuiltinConfig::doAttachSchema(Handle *handle,
                                            MigrationSetting *setting)
{
    SharedLockGuard lockGuard(setting->getSharedLock());
    auto attacheds = handle->getUnorderedAttachedSchemas();
    if (!attacheds.first) {
        return false;
    }

    auto &toDetachs = attacheds.second;
    for (const auto &schemas : setting->getSchemasForAttaching()) {
        auto iter = toDetachs.find(schemas.first);
        if (iter != toDetachs.end()) {
            //necessary
            toDetachs.erase(iter);
        } else {
            WCDB::StatementAttach statement = WCDB::StatementAttach()
                                                  .attach(schemas.second.first)
                                                  .as(schemas.first);
            if (!handle->execute(statement)) {
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
        if (!handle->execute(statement)) {
            return false;
        }
    }
    return true;
}

} //namespace WCDB
