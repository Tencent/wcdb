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
#include <WCDB/FactoryBackup.hpp>
#include <WCDB/FactoryRestorer.hpp>
#include <WCDB/FileHandle.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/FullCrawler.hpp>
#include <WCDB/Mechanic.hpp>
#include <WCDB/Path.hpp>
#include <WCDB/ThreadedErrors.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Backup
FactoryRestorer::Backup::Backup(const FactoryRestorer &restorer)
    : FactoryBackup(restorer.factory), m_restorer(restorer)
{
}

bool FactoryRestorer::Backup::work(const Filter &shouldTableDeconstructed)
{
    return doWork(shouldTableDeconstructed, m_restorer.database);
}

FactoryRestorer::FactoryRestorer(const Factory &factory)
    : FactoryRelated(factory)
    , databaseFileName(Path::getFileName(factory.database))
    , database(
          Path::addComponent(factory.getRestoreDirectory(), databaseFileName))
    , m_score(0)
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

    //1.5 calculate weights to deal with the progress and score
    std::list<std::string> workshopDirectories;
    std::tie(succeed, workshopDirectories) = factory.getWorkshopDirectories();
    if (!succeed) {
        tryUpgradeErrorWithThreadedError();
    }

    if (!calculateWeights(workshopDirectories)) {
        return false;
    }

    //2. Restore from current db. It must be succeed without even non-critical errors.
    if (!restore(factory.database) ||
        m_criticalError.code() != Error::Code::OK) {
        return false;
    }

    //3. Restore from all depositor db. It should be succeed without critical errors.
    for (const auto &workshopDirectory : workshopDirectories) {
        if (!restore(Path::addComponent(workshopDirectory, databaseFileName))) {
            return false;
        }
    }

    //4. Do a Backup on restore db.
    FactoryRestorer::Backup Backup(*this);
    if (!Backup.work(m_filter)) {
        tryUpgradeErrorWithThreadedError();
        return false;
    }

    //5. Archive current db and use restore db
    FactoryDepositor depositor(factory);
    if (!depositor.work()) {
        return false;
    }
    std::string baseDirectory = Path::getBaseName(factory.database);
    succeed = FileManager::shared()->moveItems(
        Factory::associatedPathsForDatabase(database), baseDirectory);
    if (!succeed) {
        return false;
    }

    //6. Remove all depositor dbs.
    if (m_criticalError.code() == Error::Code::OK) {
        FileManager::shared()->removeItem(factory.directory);
    }

    finishProgress();

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
            mechanic.setProgressCallback(
                [&database, this](double progress, double increment) {
                    updateProgress(database, increment);
                });
            mechanic.work();
            updateScore(database, mechanic.getScore());
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
    fullCrawler.setProgressCallback(
        [&database, this](double progress, double increment) {
            updateProgress(database, increment);
        });
    fullCrawler.work();
    updateScore(database, fullCrawler.getScore());
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

#pragma mark - Score and Progress
double FactoryRestorer::getScore() const
{
    return m_score;
}

bool FactoryRestorer::calculateWeights(
    const std::list<std::string> &workshopDirectories)
{
    size_t totalSize = 0;

    //Origin database
    if (!calculateWeight(factory.database, totalSize)) {
        return false;
    }

    //Materials
    for (const auto &workshopDirectory : workshopDirectories) {
        std::string database =
            Path::addComponent(workshopDirectory, databaseFileName);
        if (!calculateWeight(database, totalSize)) {
            return false;
        }
    }

    //Resolve to percentage
    for (auto &element : m_weights) {
        element.second = element.second / totalSize;
    }
    return true;
}

bool FactoryRestorer::calculateWeight(const std::string &database,
                                      size_t &totalSize)
{
    bool succeed;
    size_t fileSize;
    std::tie(succeed, fileSize) = FileManager::shared()->getItemsSize(
        Factory::databasePathsForDatabase(database));
    if (!succeed) {
        return false;
    }
    totalSize += fileSize;
    m_weights[database] = fileSize;
    return true;
}

void FactoryRestorer::updateProgress(const std::string &database,
                                     double increment)
{
    increaseProgress(m_weights[database] * increment);
}

void FactoryRestorer::updateScore(const std::string &database, double increment)
{
    m_score += increment * m_weights[database];
}

} //namespace Repair

} //namespace WCDB
