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
#include <WCDB/Factory.hpp>
#include <WCDB/FactoryBackup.hpp>
#include <WCDB/FactoryRenewer.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/Notifier.hpp>
#include <WCDB/Path.hpp>

namespace WCDB {

namespace Repair {

FactoryRenewer::FactoryRenewer(const Factory &factory_)
: FactoryRelated(factory_)
, directory(factory.getRenewDirectory())
, database(Path::addComponent(directory, factory.getDatabaseName()))
{
}

bool FactoryRenewer::work()
{
    bool succeed, exists;
    std::tie(succeed, exists) = FileManager::fileExists(database);
    if (!exists) {
        if (!succeed) {
            assignWithSharedThreadedError();
        }
        factory.removeDirectoryIfEmpty();
        return succeed;
    }

    std::tie(succeed, exists) = FileManager::fileExists(factory.database);
    if (!succeed) {
        assignWithSharedThreadedError();
        return false;
    }
    if (exists) {
        Error error;
        error.level = Error::Level::Warning;
        error.setCode(Error::Code::Misuse, "Repair");
        error.message = "Database already exists when renewing.";
        error.infos.set("Path", database);
        Notifier::shared()->notify(error);
        FileManager::removeItem(directory);
        factory.removeDirectoryIfEmpty();
        return true;
    }
    if (!FileManager::removeItems(Factory::associatedPathsForDatabase(factory.database))) {
        assignWithSharedThreadedError();
        return false;
    }

    std::list<String> toMove = Factory::associatedPathsForDatabase(database);
    toMove.reverse();
    if (!FileManager::moveItems(toMove, Path::getDirectoryName(factory.database))) {
        assignWithSharedThreadedError();
        return false;
    }
    FileManager::removeItem(directory);
    factory.removeDirectoryIfEmpty();

    return true;
}

bool FactoryRenewer::prepare()
{
    WCTRemedialAssert(m_assembler != nullptr, "Assembler is not available.", return false;);

    // 1. create temp directory for acquisition
    String tempDirectory = Path::addComponent(directory, "temp");
    String tempDatabase = Path::addComponent(tempDirectory, factory.getDatabaseName());
    m_assembler->setPath(tempDatabase);

    if (!FileManager::removeItem(tempDirectory)
        || !FileManager::createDirectoryWithIntermediateDirectories(tempDirectory)) {
        assignWithSharedThreadedError();
        return false;
    }

    // 2. get deposited directories for acquisition
    bool succeed;
    std::list<String> workshopDirectories;
    std::tie(succeed, workshopDirectories) = factory.getWorkshopDirectories();
    if (!succeed) {
        assignWithSharedThreadedError();
        return false;
    }

    // 3. resolve infos
    std::map<String, Info> infos;
    if (!resolveInfosForDatabase(infos, factory.database)) {
        return false;
    }
    const String databaseName = Path::getFileName(factory.database);
    for (const auto &workshopDirectory : workshopDirectories) {
        String databaseForAcquisition = Path::addComponent(workshopDirectory, databaseName);
        if (!resolveInfosForDatabase(infos, databaseForAcquisition)) {
            return false;
        }
    }

    // 4. assemble infos
    if (!m_assembler->markAsAssembling()) {
        setError(m_assembler->getError());
        return false;
    }
    for (const auto &element : infos) {
        if (!m_assembler->assembleTable(element.first, element.second.sql)
            || !m_assembler->assembleSequence(element.first, element.second.sequence)) {
            succeed = false;
            setError(m_assembler->getError());
            break;
        }
    }
    if (!m_assembler->markAsAssembled()) {
        if (!succeed) {
            setError(m_assembler->getError());
        }
        return false;
    }

    // 5. force backup assembled database if exists
    size_t fileSize;
    std::tie(succeed, fileSize) = FileManager::getFileSize(tempDatabase);
    if (!succeed) {
        assignWithSharedThreadedError();
        return false;
    }
    if (fileSize > 0) {
        FactoryBackup backup(factory);
        backup.setWriteLocker(m_writeLocker);
        backup.setReadLocker(m_readLocker);
        if (!backup.work(tempDatabase)) {
            setError(backup.getError());
            return false;
        }
    }

    // 6. move the assembled database to renew directory and wait for renew.
    std::list<String> toRemove = Factory::associatedPathsForDatabase(database);
    toRemove.reverse(); // move from end to start, which can avoid issues that unexpected crash happens before all files moved.
    if (!FileManager::removeItems(toRemove)) {
        assignWithSharedThreadedError();
        return false;
    }
    std::list<String> toMove = Factory::associatedPathsForDatabase(tempDatabase);
    toMove.reverse();
    if (!FileManager::moveItems(toMove, directory)) {
        assignWithSharedThreadedError();
        return false;
    }
    FileManager::removeItem(tempDirectory);
    return true;
}

bool FactoryRenewer::resolveInfosForDatabase(std::map<String, Info> &infos,
                                             const String &databaseForAcquisition)
{
    bool succeed;
    std::list<String> materialPaths;
    std::tie(succeed, materialPaths)
    = Factory::materialsForDeserializingForDatabase(databaseForAcquisition);
    if (!succeed) {
        assignWithSharedThreadedError();
        return false;
    }
    if (materialPaths.empty()) {
        Error error;
        error.level = Error::Level::Warning;
        error.setCode(Error::Code::NotFound, "Repair");
        error.message = "Material is not found when renewing.";
        error.infos.set("Path", databaseForAcquisition);
        Notifier::shared()->notify(error);
        return true;
    }
    Material material;
    for (const auto &materialPath : materialPaths) {
        succeed = material.deserialize(materialPath);
        if (!succeed) {
            if (ThreadedErrors::shared()->getThreadedError().isCorruption()) {
                continue;
            }
            break;
        }
        for (auto &element : material.contents) {
            auto iter = infos.find(element.first);
            if (iter == infos.end()) {
                iter = infos.emplace(std::move(element.first), Info()).first;
                iter->second.sql = std::move(element.second.sql);
            } else {
                if (iter->second.sql != element.second.sql) {
                    Error error;
                    error.level = Error::Level::Notice;
                    error.setCode(Error::Code::Mismatch, "Repair");
                    error.message = "Different sqls is found in materials.";
                    error.infos.set("Path", materialPath);
                    error.infos.set("SQL1", element.second.sql);
                    error.infos.set("SQL2", iter->second.sql);
                    Notifier::shared()->notify(error);
                }
            }
            if (element.second.sequence > iter->second.sequence) {
                iter->second.sequence = element.second.sequence;
            }
        }
        return true;
    }
    Error error;
    error.level = Error::Level::Notice;
    error.setCode(Error::Code::Notice, "Repair");
    error.message = "All materials are corrupted when renewing.";
    error.infos.set("Path", databaseForAcquisition);
    Notifier::shared()->notify(error);
    return true;
}

FactoryRenewer::Info::Info() : sequence(0)
{
}

} //namespace Repair

} //namespace WCDB
