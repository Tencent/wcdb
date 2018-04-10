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

const Config MigrationBuiltinConfig::autoAttachWithInfo(
    const std::shared_ptr<MigrationInfo> &info)
{
    assert(info != nullptr);
    return Config("migration",
                  [info](Handle *handle) -> bool {
                      if (info->isSameDatabaseMigration()) {
                          return true;
                      }
                      auto pair = handle->isSchemaExists(
                          MigrationInfo::migrationSchema);
                      if (!pair.first) {
                          return false;
                      }
                      if (pair.second) {
                          return true;
                      }
                      MigrationHandle *migrationHandle =
                          static_cast<MigrationHandle *>(handle);
                      return migrationHandle->executeWithoutTampering(
                          info->getStatementForAttachingOldDatabase());
                  },
                  MigrationBuiltinConfig::Order::Migration);
}

bool MigrationBuiltinConfig::autoDetachCallback(Handle *handle)
{
    auto pair = handle->isSchemaExists(MigrationInfo::migrationSchema);
    if (!pair.first) {
        return false;
    }
    if (!pair.second) {
        return true;
    }
    MigrationHandle *migrationHandle = static_cast<MigrationHandle *>(handle);
    return migrationHandle->executeWithoutTampering(
        MigrationInfo::getStatementForDetachingOldDatabase());
}

const Config MigrationBuiltinConfig::autoDetach()
{
    return Config("migration", MigrationBuiltinConfig::autoDetachCallback,
                  MigrationBuiltinConfig::Order::Migration);
}

Configs MigrationBuiltinConfig::defaultConfigsWithMigrationInfo(
    const std::shared_ptr<MigrationInfo> &info)
{
    Configs configs = BuiltinConfig::defaultConfigs();
    configs.setConfig(MigrationBuiltinConfig::autoAttachWithInfo(info));
    return configs;
}

} //namespace WCDB
