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
#include <WCDB/Path.hpp>

namespace WCDB {

namespace Repair {

FactoryRenewer::FactoryRenewer(Factory &factory_)
    : FactoryRelated(factory_)
    , directory(factory.getRenewDirectory())
    , database(Path::addComponent(directory, factory.getDatabaseName()))
{
}

bool FactoryRenewer::work()
{
    bool succeed, exists;
    FileManager *fileManager = FileManager::shared();
    std::tie(succeed, exists) = fileManager->fileExists(database);
    if (!exists) {
        if (!succeed) {
            assignWithSharedThreadedError();
        }
        return succeed;
    }

    std::tie(succeed, exists) = fileManager->fileExists(factory.database);
    if (!succeed) {
        assignWithSharedThreadedError();
        return false;
    }
    if (exists) {
        Error error;
        error.level = Error::Level::Warning;
        error.setCode(Error::Code::Misuse, "Repair");
        error.message = "Database already exists when renewing";
        error.infos.set("Path", database);
        Notifier::shared()->notify(error);
        fileManager->removeItem(directory);
        return true;
    }
    if (!fileManager->removeItems(
            Factory::associatedPathsForDatabase(factory.database))) {
        assignWithSharedThreadedError();
        return false;
    }

    std::list<std::string> toMove =
        Factory::associatedPathsForDatabase(database);
    toMove.reverse();
    if (!fileManager->moveItems(toMove, Path::getBaseName(factory.database))) {
        assignWithSharedThreadedError();
        return false;
    }
    fileManager->removeItem(directory);
    return true;
}

bool FactoryRenewer::prepare()
{
    // 1. create temp directory for acquisition
    std::string tempDirectory = Path::addComponent(directory, "temp");
    std::string tempDatabase =
        Path::addComponent(tempDirectory, factory.getDatabaseName());
    m_assembler->setPath(tempDatabase);

    FileManager *fileManager = FileManager::shared();
    if (!fileManager->removeItem(tempDirectory) ||
        !fileManager->createDirectoryWithIntermediateDirectories(
            tempDirectory)) {
        assignWithSharedThreadedError();
        return false;
    }

    // 2. get deposited directories for acquisition
    bool succeed;
    std::list<std::string> workshopDirectories;
    std::tie(succeed, workshopDirectories) = factory.getWorkshopDirectories();
    if (!succeed) {
        assignWithSharedThreadedError();
        return false;
    }

    // 3. resolve infos
    std::map<std::string, Info> infos;
    if (!resolveInfosForDatabase(infos, factory.database)) {
        return false;
    }
    const std::string databaseName = Path::getFileName(factory.database);
    for (const auto &workshopDirectory : workshopDirectories) {
        std::string databaseForAcquisition =
            Path::addComponent(workshopDirectory, databaseName);
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
        if (!m_assembler->assembleTable(element.first, element.second.sql) ||
            !m_assembler->assembleSequence(element.first,
                                           element.second.sequence)) {
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

    // 6. force backup assembled database if exists
    size_t fileSize;
    std::tie(succeed, fileSize) = fileManager->getFileSize(tempDatabase);
    if (!succeed) {
        assignWithSharedThreadedError();
        return false;
    }
    if (fileSize > 0) {
        FactoryBackup backup(factory);
        if (!backup.work(tempDatabase)) {
            setError(backup.getError());
            return false;
        }
    }

    // 7. move the assembled database to renew directory and wait for renew.
    std::list<std::string> toRemove =
        Factory::associatedPathsForDatabase(database);
    toRemove.reverse();
    if (!fileManager->removeItems(toRemove)) {
        assignWithSharedThreadedError();
        return false;
    }
    std::list<std::string> toMove =
        Factory::associatedPathsForDatabase(tempDatabase);
    toMove.reverse();
    if (!fileManager->moveItems(toMove, directory)) {
        assignWithSharedThreadedError();
        return false;
    }
    fileManager->removeItem(tempDirectory);
    return true;
}

bool FactoryRenewer::resolveInfosForDatabase(
    std::map<std::string, Info> &infos,
    const std::string &databaseForAcquisition)
{
    bool succeed;
    std::string materialPath;
    std::tie(succeed, materialPath) =
        Factory::materialForDeserializingForDatabase(databaseForAcquisition);
    if (!succeed) {
        assignWithSharedThreadedError();
        return false;
    }
    if (materialPath.empty()) {
        Error error;
        error.level = Error::Level::Warning;
        error.setCode(Error::Code::NotFound, "Repair");
        error.message = "Material is not found when renewing.";
        error.infos.set("Path", databaseForAcquisition);
        Notifier::shared()->notify(error);
        return true;
    }
    Material material;
    if (!material.deserialize(materialPath)) {
        if (ThreadedErrors::shared()->getThreadedError().code() ==
            Error::Code::Corrupt) {
            return true;
        }
        return false;
    }
    for (auto &element : material.contents) {
        auto iter = infos.find(element.first);
        if (iter == infos.end()) {
            iter = infos.insert({std::move(element.first), Info()}).first;
            iter->second.sql = std::move(element.second.sql);
        } else {
            if (iter->second.sql != element.second.sql) {
                Error error;
                error.level = Error::Level::Warning;
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

FactoryRenewer::Info::Info() : sequence(0)
{
}

void FactoryRenewer::setAssembler(const std::shared_ptr<Assembler> &assembler)
{
    WCTInnerAssert(assembler->getPath().empty());
    m_assembler = assembler;
}

} //namespace Repair

} //namespace WCDB
