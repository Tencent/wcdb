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
#include <WCDB/Data.hpp>
#include <WCDB/Factory.hpp>
#include <WCDB/FactoryDeconstructor.hpp>
#include <WCDB/FactoryRestorer.hpp>
#include <WCDB/FileHandle.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/FullCrawler.hpp>
#include <WCDB/Mechanic.hpp>
#include <WCDB/Path.hpp>
#include <WCDB/ThreadedErrors.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Deconstructor
FactoryRestorer::Deconstructor::Deconstructor(const FactoryRestorer &restorer)
    : FactoryDeconstructor(restorer.factory), m_restorer(restorer)
{
}

bool FactoryRestorer::Deconstructor::work(
    const Filter &shouldTableDeconstructed)
{
    return doWork(shouldTableDeconstructed, m_restorer.database);
}

FactoryRestorer::FactoryRestorer(const Factory &factory)
    : FactoryDerived(factory)
    , databaseFileName(Path::getFileName(factory.database))
    , database(
          Path::addComponent(factory.getRestoreDirectory(), databaseFileName))
{
}

void FactoryRestorer::filter(const Filter &filter)
{
    m_filter = filter;
}

#pragma mark - Restorer
bool FactoryRestorer::work()
{
    WCTInnerAssert(m_assembler->getPath().empty());

    FileManager *fileManager = FileManager::shared();
    bool succeed;

    //1. Remove the old restore db
    std::string restoreDirectory = factory.getRestoreDirectory();
    if (!fileManager->removeItem(restoreDirectory)) {
        tryUpgradeErrorWithThreadedError();
        return false;
    }
    if (!fileManager->createDirectoryWithIntermediateDirectories(
            factory.getRestoreDirectory())) {
        return false;
    }

    //2. Restore from current db. It must be succeed without even non-critical errors.
    if (!restore(factory.database) ||
        m_criticalError.code() != Error::Code::OK) {
        return false;
    }

    //3. Restore from all archived db. It should be succeed without critical errors.
    std::list<std::string> materialDirectories;
    std::tie(succeed, materialDirectories) = factory.getMaterialDirectories();
    if (!succeed) {
        tryUpgradeErrorWithThreadedError();
    }
    for (const auto &materialDirectory : materialDirectories) {
        if (!restore(Path::addComponent(materialDirectory, databaseFileName))) {
            return false;
        }
    }

    //4. Do a deconstructor on restore db.
    FactoryRestorer::Deconstructor deconstructor(*this);
    if (!deconstructor.work(m_filter)) {
        tryUpgradeErrorWithThreadedError();
        return false;
    }

    //5. Archive current db and use restore db
    FactoryArchiver archiver(factory);
    if (!archiver.work()) {
        return false;
    }
    std::string baseDirectory = Path::getBaseName(factory.database);
    succeed = FileManager::shared()->moveItems(
        Factory::associatedPathsForDatabase(database), baseDirectory);
    if (!succeed) {
        return false;
    }

    //6. Remove all archived dbs.
    if (m_criticalError.code() == Error::Code::OK) {
        FileManager::shared()->removeItem(factory.directory);
    }

    return true;
}

bool FactoryRestorer::restore(const std::string &database)
{
    WCTInnerAssert(m_assembler != nullptr);

    std::string materialPath;
    bool succeed;
    std::tie(succeed, materialPath) =
        Factory::pickMaterialForRestoring(database);
    if (!succeed) {
        tryUpgradeErrorWithThreadedError();
        return false;
    }

    if (!materialPath.empty()) {
        size_t fileSize;
        std::tie(succeed, fileSize) =
            FileManager::shared()->getFileSize(materialPath);
        if (!succeed) {
            tryUpgradeErrorWithThreadedError();
            return false;
        }

        Data materialData(fileSize);
        if (materialData.empty()) {
            tryUpgradeErrorWithThreadedError();
            return false;
        }

        FileHandle fileHandle(materialPath);
        if (!fileHandle.open(FileHandle::Mode::ReadOnly)) {
            tryUpgradeErrorWithThreadedError();
            return false;
        }
        ssize_t read = fileHandle.read(materialData.buffer(), 0, fileSize);
        if (read < 0) {
            tryUpgradeErrorWithThreadedError();
            return false;
        }
        Material material;
        if (material.deserialize(materialData)) {
            Mechanic mechanic(database);
            mechanic.setAssembler(m_assembler);
            //TODO progress
            mechanic.work();
            tryUpgradeError(mechanic.getCriticalError());
            return mechanic.isCriticalErrorFatal();
        } else if (ThreadedErrors::shared()->getThreadedError().code() !=
                   Error::Code::Corrupt) {
            tryUpgradeErrorWithThreadedError();
            return false;
        }
        Error warning;
        warning.level = Error::Level::Warning;
        warning.setCode(Error::Code::Corrupt, "Repair");
        warning.infos.set("Path", materialPath);
        warning.message = "Material is corrupted";
        Reporter::shared()->report(std::move(warning));
    } else {
        Error warning;
        warning.level = Error::Level::Warning;
        warning.setCode(Error::Code::NotFound, "Repair");
        warning.infos.set("Path", database);
        warning.message = "Material not found";
        Reporter::shared()->report(std::move(warning));
    }
    FullCrawler fullCrawler(database);
    fullCrawler.setAssembler(m_assembler);
    //TODO progress
    fullCrawler.work();
    tryUpgradeError(fullCrawler.getCriticalError());
    return fullCrawler.isCriticalErrorFatal();
}

#pragma mark - Assembler
void FactoryRestorer::setAssembler(const std::shared_ptr<Assembler> &assembler)
{
    WCTInnerAssert(m_assembler->getPath().empty());
    m_assembler = assembler;
    m_assembler->setPath(database);
}

} //namespace Repair

} //namespace WCDB
