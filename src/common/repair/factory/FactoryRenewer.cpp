//
// Created by sanhuazhang on 2018/06/12
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

#include "FactoryRenewer.hpp"
#include "Assertion.hpp"
#include "CoreConst.h"
#include "Factory.hpp"
#include "FactoryBackup.hpp"
#include "FileManager.hpp"
#include "Notifier.hpp"
#include "Path.hpp"

namespace WCDB {

namespace Repair {

FactoryRenewer::FactoryRenewer(const Factory &factory_)
: FactoryRelated(factory_)
, directory(factory.getRenewDirectory())
, database(Path::addComponent(directory, factory.getDatabaseName()))
{
}

FactoryRenewer::~FactoryRenewer() = default;

bool FactoryRenewer::work()
{
    auto exists = FileManager::fileExists(database);
    if (!exists.succeed()) {
        assignWithSharedThreadedError();
        return false;
    }
    if (!exists.value()) {
        factory.removeDirectoryIfEmpty();
        return true;
    }

    exists = FileManager::fileExists(factory.database);
    if (!exists.succeed()) {
        assignWithSharedThreadedError();
        return false;
    }
    if (exists.value()) {
        Error error(Error::Code::Misuse, Error::Level::Warning, "Database already exists when renewing.");
        error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
        error.infos.insert_or_assign(ErrorStringKeyPath, factory.database);
        Notifier::shared().notify(error);
        FileManager::removeItem(directory);
        factory.removeDirectoryIfEmpty();
        return true;
    }
    if (!FileManager::removeItems(Factory::associatedPathsForDatabase(factory.database))) {
        assignWithSharedThreadedError();
        return false;
    }

    std::list<StringView> toMove = Factory::associatedPathsForDatabase(database);
    toMove.reverse();
    if (!FileManager::moveItems(toMove, Path::getDirectory(factory.database))) {
        assignWithSharedThreadedError();
        return false;
    }
    FileManager::removeItem(directory);
    factory.removeDirectoryIfEmpty();

    return true;
}

bool FactoryRenewer::prepare()
{
    WCTRemedialAssert(
    m_assembleDelegate != nullptr, "Assemble is not available.", return false;);

    // 1. create temp directory for acquisition
    StringView tempDirectory = Path::addComponent(directory, "temp");
    StringView tempDatabase
    = Path::addComponent(tempDirectory, factory.getDatabaseName());
    m_assembleDelegate->setAssemblePath(tempDatabase);

    if (!FileManager::removeItem(tempDirectory)
        || !FileManager::createDirectoryWithIntermediateDirectories(tempDirectory)) {
        assignWithSharedThreadedError();
        return false;
    }

    // 2. get deposited directories for acquisition
    auto optionalWorkshopDirectories = factory.getWorkshopDirectories();
    if (!optionalWorkshopDirectories.succeed()) {
        assignWithSharedThreadedError();
        return false;
    }
    std::list<StringView> &workshopDirectories = optionalWorkshopDirectories.value();

    // 3. resolve infos
    StringViewMap<Info> infos;
    if (!resolveInfosForDatabase(infos, factory.database)) {
        return false;
    }
    const StringView databaseName = Path::getFileName(factory.database);
    for (const auto &workshopDirectory : workshopDirectories) {
        StringView databaseForAcquisition
        = Path::addComponent(workshopDirectory, databaseName);
        if (!resolveInfosForDatabase(infos, databaseForAcquisition)) {
            return false;
        }
    }

    // 4. assemble infos
    if (!m_assembleDelegate->markAsAssembling()) {
        setError(m_assembleDelegate->getAssembleError());
        return false;
    }
    bool succeed = true;
    for (const auto &element : infos) {
        if (!m_assembleDelegate->assembleTable(element.first, element.second.sql)
            || !m_assembleDelegate->assembleSequence(element.first,
                                                     element.second.sequence)) {
            succeed = false;
            setError(m_assembleDelegate->getAssembleError());
            break;
        }
        for (const auto &iter : element.second.associatedSQLs) {
            m_assembleDelegate->assembleSQL(iter);
        }
    }
    if (!m_assembleDelegate->markAsAssembled()) {
        if (!succeed) {
            setError(m_assembleDelegate->getAssembleError());
        }
        return false;
    }

    m_assembleDelegate->finishAssemble();

    // 5. force backup assembled database if exists
    auto fileSize = FileManager::getFileSize(tempDatabase);
    if (!fileSize.succeed()) {
        assignWithSharedThreadedError();
        return false;
    }
    if (fileSize.value() > 0) {
        FactoryBackup backup(factory);
        backup.setBackupExclusiveDelegate(m_exclusiveDelegate);
        backup.setBackupSharedDelegate(m_sharedDelegate);
        backup.setCipherDelegate(m_cipherDelegate);
        if (m_cipherDelegate->isCipherDB()) {
            auto salt = m_cipherDelegate->tryGetSaltFromDatabase(tempDatabase);
            if (!salt.succeed()) {
                assignWithSharedThreadedError();
                return false;
            }
            if (!m_cipherDelegate->switchCipherSalt(salt.value())) {
                setError(m_cipherDelegate->getCipherError());
                return false;
            }
        }
        if (!backup.work(tempDatabase)) {
            setError(backup.getError());
            return false;
        }
    }

    m_exclusiveDelegate->finishBackup();
    m_sharedDelegate->finishBackup();

    // 6. move the assembled database to renew directory and wait for renew.
    std::list<StringView> toRemove = Factory::associatedPathsForDatabase(database);
    toRemove.reverse(); // move from end to start, which can avoid issues that unexpected crash happens before all files moved.
    if (!FileManager::removeItems(toRemove)) {
        assignWithSharedThreadedError();
        return false;
    }
    std::list<StringView> toMove = Factory::associatedPathsForDatabase(tempDatabase);
    toMove.reverse();
    if (!FileManager::moveItems(toMove, directory)) {
        assignWithSharedThreadedError();
        return false;
    }
    FileManager::removeItem(tempDirectory);
    return true;
}

bool FactoryRenewer::resolveInfosForDatabase(StringViewMap<Info> &infos,
                                             const UnsafeStringView &databaseForAcquisition)
{
    auto optionalMaterialPaths
    = Factory::materialsForDeserializingForDatabase(databaseForAcquisition);
    if (!optionalMaterialPaths.succeed()) {
        assignWithSharedThreadedError();
        return false;
    }
    std::list<StringView> &materialPaths = optionalMaterialPaths.value();

    if (materialPaths.empty()) {
        Error error(Error::Code::NotFound, Error::Level::Warning, "Material is not found when renewing.");
        error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
        error.infos.insert_or_assign(ErrorStringKeyAssociatePath, databaseForAcquisition);
        Notifier::shared().notify(error);
        return true;
    }
    for (const auto &materialPath : materialPaths) {
        Material material;
        bool succeed = false;
        if (!m_cipherDelegate->isCipherDB()) {
            succeed = material.deserialize(materialPath);
        } else {
            material.setCipherDelegate(m_cipherDelegate);
            succeed = material.decryptedDeserialize(materialPath, true);
        }
        if (!succeed) {
            if (ThreadedErrors::shared().getThreadedError().isCorruption()) {
                continue;
            } else {
                break;
            }
        }

        for (auto &element : material.contentsMap) {
            auto iter = infos.find(element.first);
            if (iter == infos.end()) {
                iter = infos.emplace(std::move(element.first), Info()).first;
                iter->second.sql = std::move(element.second->sql);
                iter->second.associatedSQLs = std::move(element.second->associatedSQLs);
            } else {
                if (iter->second.sql != element.second->sql) {
                    Error error(Error::Code::Mismatch,
                                Error::Level::Notice,
                                "Different sqls is found in materials.");
                    error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
                    error.infos.insert_or_assign("MaterialPaths", materialPath);
                    error.infos.insert_or_assign("SQL1", element.second->sql);
                    error.infos.insert_or_assign("SQL2", iter->second.sql);
                    Notifier::shared().notify(error);
                }
            }
            if (element.second->sequence > iter->second.sequence) {
                iter->second.sequence = element.second->sequence;
            }
        }
        return true;
    }
    // Regard it as no error
    Error error(Error::Code::Notice, Error::Level::Notice, "All materials are corrupted when renewing.");
    error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
    error.infos.insert_or_assign(ErrorStringKeyAssociatePath, databaseForAcquisition);
    Notifier::shared().notify(error);
    return true;
}

FactoryRenewer::Info::Info() : sequence(0)
{
}

} //namespace Repair

} //namespace WCDB
