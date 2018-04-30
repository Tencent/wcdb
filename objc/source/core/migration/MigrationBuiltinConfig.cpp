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

const Config
MigrationBuiltinConfig::migrationWithSetting(MigrationSetting *setting)
{
    WCTInnerAssert(setting != nullptr);
    return Config("migration",
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
    if (setting->isStarted()) {
        return true;
    }
    LockGuard lockGuard(setting->getSharedLock());
    if (setting->isStarted()) {
        return true;
    }
    std::list<std::shared_ptr<MigrationInfo>> infos;
    if (handle->runTransaction([&handle, &infos, setting](Handle *) -> bool {
            for (const auto &info : setting->getInfos()) {
                auto pair =
                    handle->isTableExists(info.second->getSourceTable());
                if (!pair.first) {
                    return false;
                }
                //Create view
                bool result = false;
                if (pair.second) {
                    result = handle->execute(
                        info.second->getStatementForCreatingUnionedView());
                } else {
                    result = handle->execute(
                        info.second->getStatementForDroppingUnionedView());
                    infos.push_back(info.second);
                }
                if (!result) {
                    return false;
                }
            }
            return true;
        })) {
        for (const auto &info : infos) {
            if (setting->markAsMigrated(info->targetTable)) {
                schemaChanged = true;
            }
        }
        setting->markAsStarted();
        return true;
    }
    return false;
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
