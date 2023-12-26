//
// Created by qiuwenchen on 2023/12/15.
//

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

#include "FactoryVacuum.hpp"
#include "CoreConst.h"
#include "Factory.hpp"
#include "FileManager.hpp"
#include "FullCrawler.hpp"
#include "Notifier.hpp"
#include "Path.hpp"

namespace WCDB {

namespace Repair {

FactoryVacuum::FactoryVacuum(const Factory &factory_)
: FactoryRelated(factory_)
, directory(factory.getVacuumDirectory())
, database(Path::addComponent(directory, factory.getDatabaseName()))
{
}

FactoryVacuum::~FactoryVacuum() = default;

bool FactoryVacuum::work()
{
    auto exists = FileManager::fileExists(database);
    if (!exists.succeed()) {
        assignWithSharedThreadedError();
        return exit(false);
    }
    if (!exists.value()) {
        return exit(true);
    }

    exists = FileManager::fileExists(factory.database);
    if (!exists.succeed()) {
        assignWithSharedThreadedError();
        return exit(false);
    }
    if (exists.value()) {
        Error error(Error::Code::Misuse, Error::Level::Warning, "Database already exists when vacuum.");
        error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
        error.infos.insert_or_assign(ErrorStringKeyPath, factory.database);
        Notifier::shared().notify(error);
        return exit(true);
    }

    if (!FileManager::removeItems(Factory::associatedPathsForDatabase(factory.database))) {
        assignWithSharedThreadedError();
        return exit(false);
    }

    std::list<StringView> toMove = Factory::associatedPathsForDatabase(database);
    toMove.reverse();
    if (!FileManager::moveItems(toMove, Path::getDirectory(factory.database))) {
        assignWithSharedThreadedError();
        return exit(false);
    }

    return exit(true);
}

bool FactoryVacuum::prepare()
{
    WCTRemedialAssert(
    m_vacuumDelegate != nullptr, "Vacuum delegate is not available.", return false;);

    // 1. create temp directory for acquisition
    StringView tempDirectory = Path::addComponent(directory, "temp");
    StringView tempDatabase
    = Path::addComponent(tempDirectory, factory.getDatabaseName());

    if (!FileManager::removeItem(tempDirectory)
        || !FileManager::createDirectoryWithIntermediateDirectories(tempDirectory)) {
        assignWithSharedThreadedError();
        return exit(false);
    }

    // 2. Copy all data into temp database
    m_vacuumDelegate->setVacuumDatabase(tempDatabase);
    m_vacuumDelegate->setOriginalDatabase(factory.database);
    m_vacuumDelegate->setProgressCallback(std::bind(
    &FactoryVacuum::increaseProgress, this, std::placeholders::_1, std::placeholders::_2));
    ;
    if (!m_vacuumDelegate->executeVacuum()) {
        setError(m_vacuumDelegate->getVacuumError());
        return exit(false);
    }

    // 3. move the assembled database to vacuum directory.
    std::list<StringView> toRemove = Factory::associatedPathsForDatabase(database);
    toRemove.reverse(); // move from end to start, which can avoid issues that unexpected crash happens before all files moved.
    if (!FileManager::removeItems(toRemove)) {
        assignWithSharedThreadedError();
        return exit(false);
    }
    std::list<StringView> toMove = Factory::associatedPathsForDatabase(tempDatabase);
    toMove.reverse();
    if (!FileManager::moveItems(toMove, directory)) {
        assignWithSharedThreadedError();
        return exit(false);
    }
    FileManager::removeItem(tempDirectory);

    // 4. remove the original database
    if (!FileManager::removeItems(Factory::associatedPathsForDatabase(factory.database))) {
        assignWithSharedThreadedError();
        return exit(false);
    }

    return true;
}

bool FactoryVacuum::increaseProgress(double, double increment)
{
    return Progress::increaseProgress(increment);
}

bool FactoryVacuum::exit(bool result)
{
    FileManager::removeItem(directory);
    factory.removeDirectoryIfEmpty();
    if (result) {
        finishProgress();
    }
    return result;
}

} // namespace Repair

} // namespace WCDB
