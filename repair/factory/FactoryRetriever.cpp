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
#include <WCDB/FactoryRetriever.hpp>
#include <WCDB/FileHandle.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/FullCrawler.hpp>
#include <WCDB/Mechanic.hpp>
#include <WCDB/Path.hpp>
#include <WCDB/ThreadedErrors.hpp>

namespace WCDB {

namespace Repair {

FactoryRetriever::FactoryRetriever(Factory &factory_)
    : FactoryRelated(factory_)
    , databaseFileName(factory.getDatabaseName())
    , database(Path::addComponent(factory.getRestoreDirectory(),
                                  factory.getDatabaseName()))
{
}

#pragma mark - Restorer
double FactoryRetriever::work()
{
    WCTInnerAssert(m_assembler != nullptr);

    FileManager *fileManager = FileManager::shared();
    bool succeed;

    //1. Remove the old restore db
    std::string restoreDirectory = factory.getRestoreDirectory();
    if (!fileManager->removeItem(restoreDirectory)) {
        tryUpgradeErrorWithSharedThreadedError();
        return -1;
    }
    if (!fileManager->createDirectoryWithIntermediateDirectories(
            factory.getRestoreDirectory())) {
        tryUpgradeErrorWithSharedThreadedError();
        return -1;
    }

    //1.5 calculate weights to deal with the progress and score
    std::list<std::string> workshopDirectories;
    std::tie(succeed, workshopDirectories) = factory.getWorkshopDirectories();
    if (!succeed) {
        tryUpgradeErrorWithSharedThreadedError();
        return -1;
    }

    if (!calculateWeights(workshopDirectories)) {
        return -1;
    }

    //2. Restore from current db. It must be succeed without even non-critical errors.
    if (!restore(factory.database) ||
        getCriticalLevel() != CriticalLevel::None) {
        return -1;
    }

    //3. Restore from all depositor db. It should be succeed without critical errors.
    for (const auto &workshopDirectory : workshopDirectories) {
        if (!restore(Path::addComponent(workshopDirectory, databaseFileName))) {
            return -1;
        }
    }

    //4. Do a Backup on restore db.
    FactoryBackup backup(factory);
    if (!backup.work(database)) {
        tryUpgradeError(backup.getError());
        return -1;
    }

    //5. Archive current db and use restore db
    FactoryDepositor depositor(factory);
    if (!depositor.work()) {
        tryUpgradeError(depositor.getError());
        return -1;
    }
    std::string baseDirectory = Path::getBaseName(factory.database);
    succeed = FileManager::shared()->moveItems(
        Factory::associatedPathsForDatabase(database), baseDirectory);
    if (!succeed) {
        tryUpgradeErrorWithSharedThreadedError();
        return -1;
    }

    //6. Remove all depositor dbs.
    if (getCriticalLevel() < CriticalLevel::Fatal) {
        FileManager::shared()->removeItem(factory.directory);
    }

    finishProgress();

    return getScore();
}

bool FactoryRetriever::restore(const std::string &database)
{
    WCTInnerAssert(m_assembler != nullptr);

    std::string materialPath;
    bool succeed;
    std::tie(succeed, materialPath) =
        Factory::materialForDeserializingForDatabase(database);
    if (!succeed) {
        tryUpgradeErrorWithSharedThreadedError();
        return false;
    }

    if (!materialPath.empty()) {
        Material material;
        if (material.deserialize(materialPath)) {
            Mechanic mechanic(database);
            mechanic.setAssembler(m_assembler);
            mechanic.setProgressCallback(
                [&database, this](double progress, double increment) {
                    updateProgress(database, increment);
                });
            mechanic.work();
            updateScore(database, mechanic.getScore());
            tryUpgradeError(mechanic.getCriticalError());
            return mechanic.getCriticalLevel() >= CriticalLevel::Fatal;
        } else if (ThreadedErrors::shared()->getThreadedError().code() !=
                   Error::Code::Corrupt) {
            tryUpgradeErrorWithSharedThreadedError();
            return false;
        }
        Error warning;
        warning.level = Error::Level::Warning;
        warning.setCode(Error::Code::Corrupt, "Repair");
        warning.infos.set("Path", materialPath);
        warning.message = "Material is corrupted";
        Notifier::shared()->notify(std::move(warning));
    } else {
        Error warning;
        warning.level = Error::Level::Warning;
        warning.setCode(Error::Code::NotFound, "Repair");
        warning.infos.set("Path", database);
        warning.message = "Material not found";
        Notifier::shared()->notify(std::move(warning));
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
    return fullCrawler.getCriticalLevel() < CriticalLevel::Fatal;
}

#pragma mark - Assembler
void FactoryRetriever::setAssembler(const std::shared_ptr<Assembler> &assembler)
{
    WCTInnerAssert(assembler->getPath().empty());
    m_assembler = assembler;
    m_assembler->setPath(database);
}

#pragma mark - Score and Progress
bool FactoryRetriever::calculateWeights(
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

bool FactoryRetriever::calculateWeight(const std::string &database,
                                       size_t &totalSize)
{
    bool succeed;
    size_t fileSize;
    std::tie(succeed, fileSize) = FileManager::shared()->getItemsSize(
        Factory::databasePathsForDatabase(database));
    if (!succeed) {
        tryUpgradeErrorWithSharedThreadedError();
        return false;
    }
    totalSize += fileSize;
    m_weights[database] = fileSize;
    return true;
}

void FactoryRetriever::updateProgress(const std::string &database,
                                      double increment)
{
    increaseProgress(m_weights[database] * increment);
}

void FactoryRetriever::updateScore(const std::string &database,
                                   double increment)
{
    increaseScore(increment * m_weights[database]);
}

} //namespace Repair

} //namespace WCDB
