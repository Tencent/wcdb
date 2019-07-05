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
#include <WCDB/CoreConst.h>
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
    StringView restoreDirectory = factory.getRestoreDirectory();
    if (!FileManager::removeItem(restoreDirectory)) {
        setCriticalErrorWithSharedThreadedError();
        return exit(false);
    }
    if (!FileManager::createDirectoryWithIntermediateDirectories(factory.getRestoreDirectory())) {
        setCriticalErrorWithSharedThreadedError();
        return exit(false);
    }

    //1.5 calculate weights to deal with the progress and score
    auto optionalWorkshopDirectories = factory.getWorkshopDirectories();
    if (!optionalWorkshopDirectories.has_value()) {
        setCriticalErrorWithSharedThreadedError();
        return exit(false);
    }
    std::list<StringView> &workshopDirectories = optionalWorkshopDirectories.value();

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

    reportSummary(SteadyClock::timeIntervalSinceSteadyClockToNow(before));

    //4. Do a backup on restore db.
    FactoryBackup backup(factory);
    backup.setWriteLocker(m_writeLocker);
    backup.setReadLocker(m_readLocker);
    if (!backup.work(database)) {
        setCriticalError(backup.getError());
        return exit(false);
    }

    m_writeLocker->finish();
    m_readLocker->finish();
    m_assembler->finish();

    //5. Archive current db and use restore db
    FactoryDepositor depositor(factory);
    if (!depositor.work()) {
        setCriticalError(depositor.getError());
        return exit(false);
    }
    StringView baseDirectory = Path::getDirectoryName(factory.database);
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

bool FactoryRetriever::restore(const UnsafeStringView &databasePath)
{
    // material paths is already ordered by modified time
    auto optionalMaterialPaths = Factory::materialsForDeserializingForDatabase(databasePath);
    if (!optionalMaterialPaths.has_value()) {
        setCriticalErrorWithSharedThreadedError();
        return false;
    }

    std::list<StringView> &materialPaths = optionalMaterialPaths.value();
    bool useMaterial = false;
    Fraction score;
    if (!materialPaths.empty()) {
        Material material;
        Time materialTime;
        StringView path;
        for (const auto &materialPath : materialPaths) {
            useMaterial = material.deserialize(materialPath);
            if (useMaterial) {
                auto optionalMaterialTime = FileManager::getFileModifiedTime(materialPath);
                if (!optionalMaterialTime.has_value()) {
                    setCriticalErrorWithSharedThreadedError();
                    return false;
                }
                materialTime = std::move(optionalMaterialTime.value());
                break;
            }
            if (!ThreadedErrors::shared().getThreadedError().isCorruption()) {
                setCriticalErrorWithSharedThreadedError();
                return false;
            }
        }
        if (useMaterial) {
            Mechanic mechanic(databasePath);
            m_assembler->markDuplicatedAsIgnorable(false);
            mechanic.setAssembler(m_assembler);
            mechanic.setMaterial(&material);
            mechanic.setProgressCallback(std::bind(&FactoryRetriever::increaseProgress,
                                                   this,
                                                   databasePath,
                                                   true,
                                                   std::placeholders::_1,
                                                   std::placeholders::_2));
            SteadyClock before = SteadyClock::now();
            bool result = mechanic.work();
            if (!result) {
                WCTAssert(isErrorCritial());
                setCriticalError(mechanic.getError());
                return false;
            } else {
                tryUpgradeError(mechanic.getError());
            }
            score = mechanic.getScore();
            reportMechanic(mechanic.getScore(),
                           databasePath,
                           SteadyClock::timeIntervalSinceSteadyClockToNow(before),
                           materialTime);
        }
    } else {
        Error warning(Error::Code::NotFound, Error::Level::Warning, "Material is not found");
        warning.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
        warning.infos.insert_or_assign(ErrorStringKeyPath, databasePath);
        Notifier::shared().notify(warning);
    }

    FullCrawler fullCrawler(databasePath);
    m_assembler->markDuplicatedAsIgnorable(useMaterial);
    fullCrawler.setAssembler(m_assembler);
    fullCrawler.setProgressCallback(std::bind(&FactoryRetriever::increaseProgress,
                                              this,
                                              databasePath,
                                              useMaterial,
                                              std::placeholders::_1,
                                              std::placeholders::_2));
    SteadyClock before = SteadyClock::now();
    if (fullCrawler.work()) {
        reportFullCrawler(fullCrawler.getScore(),
                          databasePath,
                          SteadyClock::timeIntervalSinceSteadyClockToNow(before));
        score = std::max(score, fullCrawler.getScore());
    } else if (!useMaterial) {
        WCTAssert(isErrorCritial());
        setCriticalError(fullCrawler.getError());
        return false;
    }
    tryUpgradeError(fullCrawler.getError());

    increaseScore(databasePath, score);
    return true;
}

#pragma mark - Report
void FactoryRetriever::reportMechanic(const Fraction &score,
                                      const UnsafeStringView &path,
                                      double cost,
                                      const Time &material)
{
    Error error(Error::Code::Notice, Error::Level::Notice, "Mechanic Retrieve Report.");
    error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
    error.infos.insert_or_assign(ErrorStringKeyPath, path);
    error.infos.insert_or_assign("Score", score.value());
    auto optionalMaterial = material.stringify();
    if (optionalMaterial.has_value()) {
        error.infos.insert_or_assign("Material", optionalMaterial.value());
    }
    finishReportOfPerformance(error, path, cost);
    error.infos.insert_or_assign(
    "Weight", StringView::formatted("%f%%", getWeight(path).value() * 100.0f));
    Notifier::shared().notify(error);
}

void FactoryRetriever::reportFullCrawler(const Fraction &score,
                                         const UnsafeStringView &path,
                                         double cost)
{
    Error error(Error::Code::Notice, Error::Level::Notice, "Crawler Retrieve Report.");
    error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
    error.infos.insert_or_assign(ErrorStringKeyPath, path);
    error.infos.insert_or_assign("Score", score.value());
    finishReportOfPerformance(error, path, cost);
    error.infos.insert_or_assign(
    "Weight", StringView::formatted("%f%%", getWeight(path).value() * 100.0f));
    Notifier::shared().notify(error);
}

void FactoryRetriever::reportSummary(double cost)
{
    Error error(Error::Code::Notice, Error::Level::Notice, "Summary Retrieve Report.");
    error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
    error.infos.insert_or_assign(ErrorStringKeyPath, database);
    error.infos.insert_or_assign("Cost", StringView::formatted("%f%%", cost));
    error.infos.insert_or_assign("Score", getScore().value());
    Notifier::shared().notify(error);
}

void FactoryRetriever::finishReportOfPerformance(Error &error,
                                                 const UnsafeStringView &databasePath,
                                                 double cost)
{
    assert(m_sizes.find(databasePath) != m_sizes.end());
    size_t size = m_sizes[databasePath];
    double sizeInMB = (double) size / 1024 / 1024;
    double speed = cost > 0 ? sizeInMB / cost : 0;
    error.infos.insert_or_assign("Cost", StringView::formatted("%f s", cost));
    error.infos.insert_or_assign("Size", StringView::formatted("%f MB", sizeInMB));
    error.infos.insert_or_assign("Speed", StringView::formatted("%f MB/s", speed));
}

#pragma mark - Score and Progress
bool FactoryRetriever::calculateSizes(const std::list<StringView> &workshopDirectories)
{
    //Origin database
    if (!calculateSize(factory.database)) {
        return false;
    }

    //Materials
    for (const auto &workshopDirectory : workshopDirectories) {
        StringView databasePath = Path::addComponent(workshopDirectory, databaseFileName);
        if (!calculateSize(databasePath)) {
            return false;
        }
    }

    m_totalSize = std::accumulate(
    m_sizes.begin(), m_sizes.end(), m_totalSize, FactoryRetriever::iterateSize);
    return true;
}

size_t FactoryRetriever::iterateSize(const size_t previous,
                                     const std::pair<StringView, size_t> &element)
{
    return previous + element.second;
}

bool FactoryRetriever::calculateSize(const UnsafeStringView &databasePath)
{
    auto fileSize
    = FileManager::getItemsSize(Factory::databasePathsForDatabase(databasePath));
    if (!fileSize.has_value()) {
        setCriticalErrorWithSharedThreadedError();
        return false;
    }
    m_sizes[databasePath] = fileSize.value();
    return true;
}

Fraction FactoryRetriever::getWeight(const UnsafeStringView &databasePath)
{
    assert(m_sizes.find(databasePath) != m_sizes.end());
    return Fraction(m_sizes[databasePath], m_totalSize > 0 ? m_totalSize : 1);
}

void FactoryRetriever::increaseProgress(const UnsafeStringView &databasePath,
                                        bool useMaterial,
                                        double progress,
                                        double increment)
{
    WCDB_UNUSED(progress)
    if (useMaterial) {
        increment *= 0.5;
    }
    Progress::increaseProgress(getWeight(databasePath).value() * increment);
}

void FactoryRetriever::increaseScore(const UnsafeStringView &databasePath,
                                     const Fraction &increment)
{
    Scoreable::increaseScore(getWeight(databasePath) * increment);
}

} //namespace Repair

} //namespace WCDB
