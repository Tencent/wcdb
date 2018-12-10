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
#include <WCDB/Notifier.hpp>
#include <WCDB/Path.hpp>
#include <WCDB/ThreadedErrors.hpp>
#include <numeric>

namespace WCDB {

namespace Repair {

FactoryRetriever::FactoryRetriever(const Factory &factory_)
: FactoryRelated(factory_)
, databaseFileName(factory.getDatabaseName())
, database(Path::addComponent(factory.getRestoreDirectory(), factory.getDatabaseName()))
, m_totalSize(0)
{
}

#pragma mark - Retriever
bool FactoryRetriever::work()
{
    WCTRemedialAssert(m_assembler != nullptr, "Assembler is not available.", return false;);
    m_assembler->setPath(database);

    bool succeed;
    //1. Remove the old restore db
    String restoreDirectory = factory.getRestoreDirectory();
    if (!FileManager::removeItem(restoreDirectory)) {
        setCriticalErrorWithSharedThreadedError();
        return exit(false);
    }
    if (!FileManager::createDirectoryWithIntermediateDirectories(factory.getRestoreDirectory())) {
        setCriticalErrorWithSharedThreadedError();
        return exit(false);
    }

    //1.5 calculate weights to deal with the progress and score
    std::list<String> workshopDirectories;
    std::tie(succeed, workshopDirectories) = factory.getWorkshopDirectories();
    if (!succeed) {
        setCriticalErrorWithSharedThreadedError();
        return exit(false);
    }

    if (!calculateSizes(workshopDirectories)) {
        return exit(false);
    }

    SteadyClock before = SteadyClock::now();
    //2. Restore from current db. It must be succeed without even non-critical errors.
    if (!restore(factory.database) || getErrorSeverity() >= Severity::Normal) {
        return exit(false);
    }

    //3. Restore from all depositor db. It should be succeed without critical errors.
    for (const auto &workshopDirectory : workshopDirectories) {
        if (!restore(Path::addComponent(workshopDirectory, databaseFileName))) {
            return exit(false);
        }
    }

    SteadyClock after = SteadyClock::now();
    reportSummary(after - before);

    //4. Do a backup on restore db.
    FactoryBackup backup(factory);
    backup.setWriteLocker(m_writeLocker);
    backup.setReadLocker(m_readLocker);
    if (!backup.work(database)) {
        setCriticalError(backup.getError());
        return exit(false);
    }

    //5. Archive current db and use restore db
    FactoryDepositor depositor(factory);
    if (!depositor.work()) {
        setCriticalError(depositor.getError());
        return exit(false);
    }
    String baseDirectory = Path::getDirectoryName(factory.database);
    succeed = FileManager::moveItems(
    Factory::associatedPathsForDatabase(database), baseDirectory);
    if (!succeed) {
        setCriticalErrorWithSharedThreadedError();
        return exit(false);
    }

    //6. Remove all deposited dbs if error is ignorable.
    if (getErrorSeverity() <= UpgradeableErrorProne::Severity::Normal) {
        FileManager::removeItem(factory.directory);
    }

    return exit(true);
}

bool FactoryRetriever::exit(bool result)
{
    factory.removeDirectoryIfEmpty();
    finishProgress();
    return result;
}

bool FactoryRetriever::restore(const String &database)
{
    std::list<String> materialPaths;
    bool succeed;
    std::tie(succeed, materialPaths)
    = Factory::materialsForDeserializingForDatabase(database);
    if (!succeed) {
        setCriticalErrorWithSharedThreadedError();
        return false;
    }

    bool useMaterial = false;
    Fraction score;
    if (!materialPaths.empty()) {
        Material material;
        Time materialTime;
        String path;
        for (const auto &materialPath : materialPaths) {
            useMaterial = material.deserialize(materialPath);
            if (useMaterial) {
                std::tie(succeed, materialTime)
                = FileManager::getFileModifiedTime(materialPath);
                if (!succeed) {
                    setCriticalErrorWithSharedThreadedError();
                    return false;
                }
                break;
            }
            if (!ThreadedErrors::shared()->getThreadedError().isCorruption()) {
                setCriticalErrorWithSharedThreadedError();
                return false;
            }
        }
        if (useMaterial) {
            Mechanic mechanic(database);
            m_assembler->markAsDuplicated(false);
            mechanic.setAssembler(m_assembler);
            mechanic.setMaterial(&material);
            mechanic.setProgressCallback(std::bind(&FactoryRetriever::increaseProgress,
                                                   this,
                                                   database,
                                                   true,
                                                   std::placeholders::_1,
                                                   std::placeholders::_2));
            SteadyClock before = SteadyClock::now();
            bool result = mechanic.work();
            SteadyClock after = SteadyClock::now();
            if (!result) {
                WCTInnerAssert(isErrorCritial());
                setCriticalError(mechanic.getError());
                return false;
            } else {
                tryUpgradeError(mechanic.getError());
            }
            score = mechanic.getScore();
            reportMechanic(mechanic.getScore(), database, after - before, materialTime);
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
    m_assembler->markAsDuplicated(useMaterial);
    fullCrawler.setAssembler(m_assembler);
    fullCrawler.setProgressCallback(std::bind(&FactoryRetriever::increaseProgress,
                                              this,
                                              database,
                                              useMaterial,
                                              std::placeholders::_1,
                                              std::placeholders::_2));
    SteadyClock before = SteadyClock::now();
    if (fullCrawler.work()) {
        SteadyClock after = SteadyClock::now();
        reportFullCrawler(fullCrawler.getScore(), database, after - before);
        score = std::max(score, fullCrawler.getScore());
    } else if (!useMaterial) {
        WCTInnerAssert(isErrorCritial());
        setCriticalError(fullCrawler.getError());
        return false;
    }
    tryUpgradeError(fullCrawler.getError());

    increaseScore(database, score);
    return true;
}

#pragma mark - Report
void FactoryRetriever::reportMechanic(const Fraction &score,
                                      const String &path,
                                      const SteadyClock &cost,
                                      const Time &material)
{
    Error error;
    error.setCode(Error::Code::Notice, "Repair");
    error.level = Error::Level::Notice;
    error.message = "Mechanic Retrieve Report.";
    error.infos.set("Path", path);
    error.infos.set("Score", score.value());
    error.infos.set("Material", material.stringify());
    finishReportOfPerformance(error, path, cost);
    error.infos.set("Weight", getWeight(path).value());
    Notifier::shared()->notify(error);
}

void FactoryRetriever::reportFullCrawler(const Fraction &score,
                                         const String &path,
                                         const SteadyClock &cost)
{
    Error error;
    error.setCode(Error::Code::Notice, "Repair");
    error.level = Error::Level::Notice;
    error.message = "Crawler Retrieve Report.";
    error.infos.set("Path", path);
    error.infos.set("Score", score.value());
    finishReportOfPerformance(error, path, cost);
    error.infos.set("Weight", getWeight(path).value());
    Notifier::shared()->notify(error);
}

void FactoryRetriever::reportSummary(const SteadyClock &cost)
{
    Error error;
    error.setCode(Error::Code::Notice, "Repair");
    error.level = Error::Level::Notice;
    error.message = "Retrieve Report Summary.";
    error.infos.set("Path", database);
    error.infos.set("Cost", cost.seconds());
    error.infos.set("Score", getScore().value());
    Notifier::shared()->notify(error);
}

void FactoryRetriever::finishReportOfPerformance(Error &error,
                                                 const String &database,
                                                 const SteadyClock &cost)
{
    double seconds = cost.seconds();
    assert(m_sizes.find(database) != m_sizes.end());
    size_t size = m_sizes[database];
    double sizeInMB = (double) size / 1024 / 1024;
    double speed = sizeInMB > 0 ? sizeInMB / seconds : 0;
    error.infos.set("Cost", seconds);
    error.infos.set("Size", sizeInMB);
    error.infos.set("Speed", speed);
}

#pragma mark - Score and Progress
bool FactoryRetriever::calculateSizes(const std::list<String> &workshopDirectories)
{
    //Origin database
    if (!calculateSize(factory.database)) {
        return false;
    }

    //Materials
    for (const auto &workshopDirectory : workshopDirectories) {
        String database = Path::addComponent(workshopDirectory, databaseFileName);
        if (!calculateSize(database)) {
            return false;
        }
    }

    m_totalSize = std::accumulate(
    m_sizes.begin(),
    m_sizes.end(),
    m_totalSize,
    [](const size_t previous, const std::pair<String, size_t> &element) {
        return previous + element.second;
    });
    return true;
}

bool FactoryRetriever::calculateSize(const String &database)
{
    bool succeed;
    size_t fileSize;
    std::tie(succeed, fileSize)
    = FileManager::getItemsSize(Factory::databasePathsForDatabase(database));
    if (!succeed) {
        setCriticalErrorWithSharedThreadedError();
        return false;
    }
    m_sizes[database] = fileSize;
    return true;
}

Fraction FactoryRetriever::getWeight(const String &database)
{
    assert(m_sizes.find(database) != m_sizes.end());
    return Fraction(m_sizes[database], m_totalSize);
}

void FactoryRetriever::increaseProgress(const String &database,
                                        bool useMaterial,
                                        double progress,
                                        double increment)
{
    if (useMaterial) {
        increment *= 0.5;
    }
    Progress::increaseProgress(getWeight(database).value() * increment);
}

void FactoryRetriever::increaseScore(const String &database, const Fraction &increment)
{
    Scoreable::increaseScore(getWeight(database) * increment);
}

} //namespace Repair

} //namespace WCDB
