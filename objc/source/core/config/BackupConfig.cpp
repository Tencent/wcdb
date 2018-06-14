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
#include <WCDB/BackupConfig.hpp>
#include <WCDB/Database.hpp>

namespace WCDB {

const std::shared_ptr<Config> &BackupConfig::shared()
{
    static const std::shared_ptr<Config> *s_shared =
        new std::shared_ptr<Config>(new BackupConfig);
    return *s_shared;
}

BackupConfig::BackupConfig() : Config(BackupConfig::name)
{
}

bool BackupConfig::invoke(Handle *handle)
{
    handle->setNotificationWhenCheckpoint(
        [](Handle *handle, int frames) -> bool {
            std::shared_ptr<Database> database =
                Database::databaseWithExistingPath(handle->path);
            WCTInnerAssert(database != nullptr);
            if (database->backup(frames)) {
                return true;
            }
            if (frames > 10000) {
                Error error;
                error.level = Error::Level::Warning;
                error.setCode(Error::Code::Exceed);
                error.message =
                    "Number of frames exceeds, mandatory checkpoint, and "
                    "delete all backups.";
                error.infos.set("Path", handle->path);
                error.infos.set("Frames", frames);
                Notifier::shared()->notify(error);

                database->removeMaterials();
                return true;
            }
            return false;
        });
    return true;
}

} //namespace WCDB
