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
        setCriticalErrorWithSharedThreadedError();
        return false;
    }
    if (!fileManager->createDirectoryWithIntermediateDirectories(
            factory.getRestoreDirectory())) {
        setCriticalErrorWithSharedThreadedError();
        return false;
    }

    //1.5 calculate weights to deal with the progress and score
    std::list<std::string> workshopDirectories;
    std::tie(succeed, workshopDirectories) = factory.getWorkshopDirectories();
    if (!succeed) {
        setCriticalErrorWithSharedThreadedError();
        return false;
    }

    if (!calculateWeights(workshopDirectories)) {
        return false;
    }

    //2. Restore from current db. It must be succeed without even non-critical errors.
    if (!restore(factory.database) || getErrorSeverity() != Severity::None) {
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
        setCriticalError(backup.getError());
        return false;
    }

    //5. Archive current db and use restore db
    FactoryDepositor depositor(factory);
    if (!depositor.work()) {
        setCriticalError(depositor.getError());
        return false;
    }
    std::string baseDirectory = Path::getBaseName(factory.database);
    succeed = FileManager::shared()->moveItems(
        Factory::associatedPathsForDatabase(database), baseDirectory);
    if (!succeed) {
        setCriticalErrorWithSharedThreadedError();
        return false;
    }

    //6. Remove all deposited dbs if error is ignorable.
    if (isErrorIgnorable()) {
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
        setCriticalErrorWithSharedThreadedError();
        return false;
    }

    if (!materialPath.empty()) {
        Material material;
        if (material.deserialize(materialPath)) {
            Mechanic mechanic(database);
            mechanic.setAssembler(m_assembler);
            mechanic.setMaterial(&material);
            mechanic.setProgressCallback(
                [&database, this](double progress, double increment) {
                    increaseProgress(database, increment);
                });
            bool result = mechanic.work();
            tryUpgradeError(mechanic.getError());
            if (!result) {
                WCTInnerAssert(isErrorCritial());
                return false;
            }
            increaseScore(database, mechanic.getScore());
            return true;
        } else if (ThreadedErrors::shared()->getThreadedError().code() !=
                   Error::Code::Corrupt) {
            //TODO use old material to restore while page hash can be verified.
            setCriticalErrorWithSharedThreadedError();
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
    bool result = fullCrawler.work();
    tryUpgradeError(fullCrawler.getError());
    if (!result) {
        WCTInnerAssert(isErrorCritial());
        return false;
    }
    increaseScore(database, fullCrawler.getScore());
    return true;
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
        setCriticalErrorWithSharedThreadedError();
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

#pragma mark - Error
void FactoryRetriever::onErrorCritical()
{
    finishProgress();
}

} //namespace Repair

} //namespace WCDB
