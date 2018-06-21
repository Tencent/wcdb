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
bool FactoryRetriever::work()
{
    WCTInnerAssert(m_assembler != nullptr);

    FileManager *fileManager = FileManager::shared();
    bool succeed;

    //1. Remove the old restore db
    std::string restoreDirectory = factory.getRestoreDirectory();
    if (!fileManager->removeItem(restoreDirectory)) {
        tryUpgradeErrorWithSharedThreadedError();
        return false;
    }
    if (!fileManager->createDirectoryWithIntermediateDirectories(
            factory.getRestoreDirectory())) {
        tryUpgradeErrorWithSharedThreadedError();
        return false;
    }

    //1.5 calculate weights to deal with the progress and score
    std::list<std::string> workshopDirectories;
    std::tie(succeed, workshopDirectories) = factory.getWorkshopDirectories();
    if (!succeed) {
        tryUpgradeErrorWithSharedThreadedError();
        return false;
    }

    if (!calculateWeights(workshopDirectories)) {
        return false;
    }

    //2. Restore from current db. It must be succeed without even non-critical errors.
    if (!restore(factory.database) ||
        getCriticalLevel() != CriticalLevel::None) {
        return false;
    }

    //3. Restore from all depositor db. It should be succeed without critical errors.
    for (const auto &workshopDirectory : workshopDirectories) {
        if (!restore(Path::addComponent(workshopDirectory, databaseFileName))) {
            return false;
        }
    }

    //4. Do a Backup on restore db.
    FactoryBackup backup(factory);
    if (!backup.work(database)) {
        tryUpgradeError(backup.getError());
        return false;
    }

    //5. Archive current db and use restore db
    FactoryDepositor depositor(factory);
    if (!depositor.work()) {
        tryUpgradeError(depositor.getError());
        return false;
    }
    std::string baseDirectory = Path::getBaseName(factory.database);
    succeed = FileManager::shared()->moveItems(
        Factory::associatedPathsForDatabase(database), baseDirectory);
    if (!succeed) {
        tryUpgradeErrorWithSharedThreadedError();
        return false;
    }

    //6. Remove all depositor dbs.
    if (getCriticalLevel() < CriticalLevel::Fatal) {
        FileManager::shared()->removeItem(factory.directory);
    }

    finishProgress();

    return true;
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
                    increaseProgress(database, increment);
                });
            mechanic.work();
            increaseScore(database, mechanic.getScore());
            tryUpgradeError(mechanic.getCriticalError());
            return mechanic.getCriticalLevel() >= CriticalLevel::Fatal;
        } else if (ThreadedErrors::shared()->getThreadedError().code() !=
                   Error::Code::Corrupt) {
            tryUpgradeErrorWithSharedThreadedError();
            return false;
        }
    } else {
        Error warning;
        warning.level = Error::Level::Warning;
        warning.setCode(Error::Code::NotFound, "Repair");
        warning.infos.set("Path", database);
        warning.message = "Material is not found";
        Notifier::shared()->notify(std::move(warning));
    }
    FullCrawler fullCrawler(database);
    fullCrawler.setAssembler(m_assembler);
    fullCrawler.setProgressCallback(
        [&database, this](double progress, double increment) {
            increaseProgress(database, increment);
        });
    fullCrawler.work();
    increaseScore(database, fullCrawler.getScore());
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
        element.second = element.second / (int) totalSize;
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
    m_weights[database] = (int) fileSize;
    return true;
}

void FactoryRetriever::increaseProgress(const std::string &database,
                                        double increment)
{
    Progress::increaseProgress(m_weights[database].value() * increment);
}

void FactoryRetriever::increaseScore(const std::string &database,
                                     const Fraction &increment)
{
    Scoreable::increaseScore(increment * m_weights[database]);
}

} //namespace Repair

} //namespace WCDB
