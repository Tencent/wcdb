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

const Config MigrationBuiltinConfig::autoAttachAndDetachWithInfo(
    const std::shared_ptr<MigrationInfo> &info)
{
    std::function<bool(Handle *)> callback = nullptr;
    if (info != nullptr) {
        //Attach if not exists
        callback = [info](Handle *handle) -> bool {
            if (info->isSameDatabaseMigration()) {
                return true;
            }
            auto pair = handle->isSchemaExists(MigrationInfo::migrationSchema);
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
        };
    } else {
        //Detach if exists
        callback = [](Handle *handle) -> bool {
            auto pair = handle->isSchemaExists(MigrationInfo::migrationSchema);
            if (!pair.first) {
                return false;
            }
            if (!pair.second) {
                return true;
            }
            MigrationHandle *migrationHandle =
                static_cast<MigrationHandle *>(handle);
            return migrationHandle->executeWithoutTampering(
                MigrationInfo::getStatementForDetachingOldDatabase());
        };
    }
    return Config("autoAttachAndDetach", callback,
                  MigrationBuiltinConfig::Order::AutoAttachAndDetach);
}

} //namespace WCDB
