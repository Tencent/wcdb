//
// Created by sanhuazhang on 2018/06/05
//

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

#include <WCDB/CoreConst.h>
#include <WCDB/Data.hpp>
#include <WCDB/Factory.hpp>
#include <WCDB/FactoryBackup.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/Notifier.hpp>

namespace WCDB {

namespace Repair {

FactoryBackup::~FactoryBackup() = default;

bool FactoryBackup::work(const UnsafeStringView& database)
{
    auto materialPath = Factory::materialForSerializingForDatabase(database);
    if (!materialPath.succeed()) {
        assignWithSharedThreadedError();
        return false;
    }

    notifiyBackupBegin(database);

    Backup backup(database);
    backup.setCipherDelegate(m_cipherDelegate);
    backup.setBackupSharedDelegate(m_sharedDelegate);
    backup.setBackupExclusiveDelegate(m_exclusiveDelegate);
    backup.filter(factory.getFilter());
    if (!backup.work()) {
        // Treat database empty error as succeed
        if (backup.getError().code() == Error::Code::Empty) {
            notifiyBackupEnd(database, materialPath.value(), backup);
            return true;
        }
        setError(backup.getError());
        return false;
    }

    if (!m_sharedDelegate->isCipherDB()) {
        if (!backup.getMaterial().serialize(materialPath.value())) {
            assignWithSharedThreadedError();
            return false;
        }
    } else {
        StringView salt = m_sharedDelegate->getCipherSalt();
        if (salt.length() == 0) {
            setError(m_sharedDelegate->getCipherError());
            return false;
        }
        WCTAssert(salt.length() == 32) if (!backup.getMaterial().encryptedSerialize(
                                           materialPath.value(), salt))
        {
            assignWithSharedThreadedError();
            return false;
        }
    }
    notifiyBackupEnd(database, materialPath.value(), backup);
    return true;
}

void FactoryBackup::notifiyBackupBegin(const UnsafeStringView& database)
{
    Error error(Error::Code::Notice, Error::Level::Notice, "Backup Begin.");
    error.infos.insert_or_assign(ErrorStringKeyPath, database);
    Notifier::shared().notify(error);
}

void FactoryBackup::notifiyBackupEnd(const UnsafeStringView& database,
                                     const UnsafeStringView& materialPath,
                                     Backup& backup)
{
    auto fileSize = FileManager::getFileSize(materialPath);
    if (fileSize.succeed()) {
        uint32_t associatedTableCount = 0;
        uint32_t leafPageCount = 0;
        for (auto content : backup.getMaterial().contents) {
            associatedTableCount += content.second.associatedSQLs.size();
            leafPageCount += content.second.verifiedPagenos.size();
        }
        Error error(Error::Code::Notice, Error::Level::Notice, "Backup End.");
        error.infos.insert_or_assign("Size", fileSize.value());
        error.infos.insert_or_assign("WalFrameCount", backup.getMaterial().info.numberOfWalFrames);
        error.infos.insert_or_assign("TableCount", backup.getMaterial().contents.size());
        error.infos.insert_or_assign("AssociatedTableCount", associatedTableCount);
        error.infos.insert_or_assign("LeafPageCount", leafPageCount);
        error.infos.insert_or_assign(ErrorStringKeyPath, database);
        Notifier::shared().notify(error);
    }
}

} //namespace Repair

} //namespace WCDB
