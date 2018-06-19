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

FactoryRenewer::FactoryRenewer(Factory &factory)
    : FactoryRelated(factory)
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
        fileManager->removeItem(directory);
        Error error;
        error.level = Error::Level::Warning;
        error.setCode(Error::Code::Misuse, "Repair");
        error.message = "Database already exists when renewing";
        error.infos.set("Path", database);
        Notifier::shared()->notify(error);
        return true;
    }

    std::list<std::string> toMove =
        Factory::associatedPathsForDatabase(database);
    if (!fileManager->moveItems(toMove, factory.directory)) {
        assignWithSharedThreadedError();
        return false;
    }
    return true;
}

bool FactoryRenewer::prepare()
{
    WCTInnerAssert(m_assembler != nullptr);

    // 1. get directories for acquisition
    bool succeed;
    std::list<std::string> workshopDirectories;
    std::tie(succeed, workshopDirectories) = factory.getWorkshopDirectories();
    if (workshopDirectories.empty()) {
        if (!succeed) {
            assignWithSharedThreadedError();
        }
        return succeed;
    }

    // 2. create temp directory for acquisition
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

    // 3. acquire sequences of materials
    std::map<std::string, int64_t> resolvedSequences;
    const std::string databaseName = Path::getFileName(factory.database);
    for (const auto &workshopDirectory : workshopDirectories) {
        std::string databaseForAcquisition =
            Path::addComponent(workshopDirectory, databaseName);
        std::string materialPath;
        std::tie(succeed, materialPath) =
            Factory::materialForDeserializingForDatabase(database);
        if (!succeed) {
            assignWithSharedThreadedError();
            return false;
        }
        if (materialPath.empty()) {
            continue;
        }
        Material material;
        if (!material.deserialize(materialPath)) {
            assignWithSharedThreadedError();
            return false;
        }
        for (const auto &element : material.contents) {
            auto iter = resolvedSequences.find(element.first);
            if (iter != resolvedSequences.end()) {
                iter->second = std::max(iter->second, element.second.sequence);
            } else {
                resolvedSequences[element.first] = element.second.sequence;
            }
        }
    }

    if (!m_assembler->assembleSequences(resolvedSequences)) {
        setError(m_assembler->getError());
        return false;
    }

    FactoryBackup backup(factory);
    if (!backup.work(database)) {
        setError(backup.getError());
        return false;
    }

    // 3. move the assembled database to renew directory and wait for renew.
    std::list<std::string> toRemove =
        Factory::associatedPathsForDatabase(database);
    toRemove.reverse();
    std::list<std::string> toMove =
        Factory::associatedPathsForDatabase(tempDatabase);
    toMove.reverse();
    if (!fileManager->removeItems(toRemove) ||
        !fileManager->moveItems(toMove, directory)) {
        assignWithSharedThreadedError();
        return false;
    }

    return true;
}

void FactoryRenewer::setAssembler(const std::shared_ptr<Assembler> &assembler)
{
    WCTInnerAssert(assembler->getPath().empty());
    m_assembler = assembler;
}

} //namespace Repair

} //namespace WCDB
