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

#include "FactoryBackup.hpp"
#include "Core.hpp"
#include "CoreConst.h"
#include "Data.hpp"
#include "Factory.hpp"
#include "FileManager.hpp"
#include "Notifier.hpp"

namespace WCDB {

namespace Repair {

FactoryBackup::~FactoryBackup() = default;

bool FactoryBackup::work(const UnsafeStringView& database, bool interruptible)
{
    m_sharedDelegate->setBackupPath(database);
    m_exclusiveDelegate->setBackupPath(database);

    //acquire checkpoint lock to avoid checkpoint during whole iteration of backup
    if (!m_sharedDelegate->acquireBackupSharedLock()) {
        return false;
    }

    bool succeed = doBackUp(database, interruptible);

    if (!m_sharedDelegate->releaseBackupSharedLock() && succeed) {
        setError(m_sharedDelegate->getBackupError());
        return false;
    }

    return succeed;
}

bool FactoryBackup::doBackUp(const UnsafeStringView& database, bool interruptible)
{
    Optional<size_t> incrementalMaterialSize = 0;
    auto config = Core::shared().getABTestConfig("clicfg_wcdb_incremental_backup");
    bool incrememtalBackupEnable = config.succeed() && config.value().length() > 0
                                   && atoi(config.value().data()) == 1;
    SharedIncrementalMaterial incrementalMaterial;
    if (incrememtalBackupEnable) {
        incrementalMaterial = Core::shared().tryGetIncrementalMaterial(database);
        if (interruptible && incrementalMaterial != nullptr) {
            incrementalMaterialSize = saveIncrementalMaterial(database, incrementalMaterial);
            if (!incrementalMaterialSize.hasValue()) {
                return false;
            }
            if (Time::now().seconds() - incrementalMaterial->info.lastBackupTime < OperationQueueTimeIntervalForBackup
                && incrementalMaterial->pages.size() < BackupMaxIncrementalPageCount) {
                return true;
            }
        }
    }

    if (interruptible) {
        notifiyBackupBegin(database);
    }

    Backup backup(database);
    backup.setCipherDelegate(m_cipherDelegate);
    backup.setBackupSharedDelegate(m_sharedDelegate);
    backup.setBackupExclusiveDelegate(m_exclusiveDelegate);
    backup.filter(factory.getFilter());
    if (!backup.work(incrementalMaterial)) {
        // Treat database empty error as succeed
        if (backup.getError().code() == Error::Code::Empty) {
            notifiyBackupEnd(database, 0, 0, backup.getMaterial(), incrementalMaterial);
            return true;
        }
        setError(backup.getError());
        return false;
    }

    const Material& material = backup.getMaterial();
    auto materialSize = saveMaterial(database, material);
    if (!materialSize.hasValue()) {
        return false;
    }

    SharedIncrementalMaterial newIncrementalMaterial = backup.getIncrementalMaterial();
    if (incrememtalBackupEnable) {
        if (!saveIncrementalMaterial(database, newIncrementalMaterial).hasValue()) {
            return false;
        }
        Core::shared().tryRegisterIncrementalMaterial(database, newIncrementalMaterial);
    }

    if (interruptible) {
        notifiyBackupEnd(
        database, materialSize.value(), incrementalMaterialSize.value(), material, newIncrementalMaterial);
    }
    return true;
}

Optional<size_t>
FactoryBackup::saveIncrementalMaterial(const UnsafeStringView& database,
                                       SharedIncrementalMaterial material)
{
    WCTAssert(material != nullptr);
    if (material == nullptr) {
        return 0;
    }
    StringView materialPath = Repair::Factory::incrementalMaterialPathForDatabase(database);
    bool succeed = false;
    if (m_cipherDelegate->isCipherDB()) {
        material->setCipherDelegate(m_cipherDelegate);
        StringView salt = m_cipherDelegate->tryGetSaltFromDatabase(database).value();
        succeed = material->encryptedSerialize(materialPath);
        material->setCipherDelegate(nullptr);
    } else {
        succeed = material->serialize(materialPath);
    }
    if (!succeed) {
        return NullOpt;
    }
    return FileManager::getFileSize(materialPath);
}

Optional<size_t>
FactoryBackup::saveMaterial(const UnsafeStringView& database, const Material& material)
{
    auto materialPath = Factory::materialForSerializingForDatabase(database);
    if (!materialPath.succeed()) {
        assignWithSharedThreadedError();
        return NullOpt;
    }
    if (!m_cipherDelegate->isCipherDB()) {
        if (!material.serialize(materialPath.value())) {
            assignWithSharedThreadedError();
            return NullOpt;
        }
    } else {
        if (!material.encryptedSerialize(materialPath.value())) {
            assignWithSharedThreadedError();
            return NullOpt;
        }
    }
    return FileManager::getFileSize(materialPath.value());
}

void FactoryBackup::notifiyBackupBegin(const UnsafeStringView& database)
{
    Error error(Error::Code::Notice, Error::Level::Notice, "Backup Begin.");
    error.infos.insert_or_assign(ErrorStringKeyPath, database);
    Notifier::shared().notify(error);
}

void FactoryBackup::notifiyBackupEnd(const UnsafeStringView& database,
                                     size_t materialSize,
                                     size_t incrementalMaterialSize,
                                     const Material& material,
                                     SharedIncrementalMaterial incrementalMaterial)
{
    uint32_t associatedTableCount = 0;
    uint32_t leafPageCount = 0;
    for (auto& content : material.contentsMap) {
        associatedTableCount += content.second->associatedSQLs.size();
        leafPageCount += content.second->verifiedPagenos.size();
    }
    Error error(Error::Code::Notice, Error::Level::Notice, "Backup End.");
    error.infos.insert_or_assign("Incremental",
                                 incrementalMaterial != nullptr
                                 && incrementalMaterial->info.incrementalBackupTimes > 0);
    error.infos.insert_or_assign("MaterialSize", materialSize);
    error.infos.insert_or_assign("LastIncrementalMaterialSize", incrementalMaterialSize);
    error.infos.insert_or_assign("TableCount", material.contentsMap.size());
    error.infos.insert_or_assign("AssociatedTableCount", associatedTableCount);
    error.infos.insert_or_assign("LeafPageCount", leafPageCount);
    error.infos.insert_or_assign(ErrorStringKeyPath, database);
    Notifier::shared().notify(error);
}

} //namespace Repair

} //namespace WCDB
