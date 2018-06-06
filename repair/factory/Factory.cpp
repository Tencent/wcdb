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

#include <WCDB/Assembler.hpp>
#include <WCDB/Assertion.hpp>
#include <WCDB/Factory.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/Material.hpp>
#include <WCDB/Path.hpp>
#include <WCDB/String.hpp>
#include <WCDB/ThreadedErrors.hpp>
#include <iomanip>

namespace WCDB {

namespace Repair {

#pragma mark - Factory
Factory::Factory(const std::string &database_)
    : database(database_)
    , directory(Path::addExtention(database_, ".factory"))
    , meta(*this)
{
}

std::pair<bool, std::list<std::string>> Factory::getWorkshopDirectories() const
{
    std::list<std::string> workshopDirectories;
    std::string restoreDirectory = getRestoreDirectory();
    if (FileManager::shared()->enumerateDirectory(
            directory,
            [&workshopDirectories, &restoreDirectory](
                const std::string &path, bool isDirectory) -> bool {
                if (isDirectory && path != restoreDirectory) {
                    workshopDirectories.push_back(path);
                }
                return true;
            })) {
        return {true, workshopDirectories};
    }
    return {false, {}};
}

std::pair<bool, std::string> Factory::generateWorkshopDiectory() const
{
    auto t = std::time(nullptr);
    struct tm tm;
    if (!localtime_r(&t, &tm)) {
        setThreadedError(Error::Code::Exceed);
        return {false, String::empty()};
    }
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S_");
    oss << rand();
    return {true, Path::addComponent(directory, oss.str())};
}

#pragma mark - Factory Derived
FactoryDepositor Factory::depositor() const
{
    return FactoryDepositor(*this);
}

FactoryRestorer Factory::restorer() const
{
    return FactoryRestorer(*this);
}

FactoryBackup Factory::backup() const
{
    return FactoryBackup(*this);
}

#pragma mark - Helper
std::string Factory::firstMaterialPathForDatabase(const std::string &database)
{
    return Path::addExtention(database, "-first.material");
}

std::string Factory::lastMaterialPathForDatabase(const std::string &database)
{
    return Path::addExtention(database, "-last.material");
}

std::string Factory::getRestoreDirectory() const
{
    return Path::addComponent(directory, "restore");
}

std::list<std::string>
Factory::associatedPathsForDatabase(const std::string &database)
{
    return {
        database,
        Path::addExtention(database, "-journal"),
        Path::addExtention(database, "-wal"),
        Path::addExtention(database, "-shm"),
        firstMaterialPathForDatabase(database),
        lastMaterialPathForDatabase(database),
    };
}

std::list<std::string>
Factory::databasePathsForDatabase(const std::string &database)
{
    return {
        database, Path::addExtention(database, "-journal"),
        Path::addExtention(database, "-wal"),
        Path::addExtention(database, "-shm"),
    };
}

std::pair<bool, std::string>
Factory::pickMaterialForRestoring(const std::string &database)
{
    //If all materials exist, return the new one.
    //If all materials do not exist, return empty.
    //Otherwise, return the existing one.
    FileManager *fileManager = FileManager::shared();
    std::string firstMaterialPath = firstMaterialPathForDatabase(database);
    std::string lastMaterialPath = lastMaterialPathForDatabase(database);
    bool succeed, firstMaterialExists, lastMaterialExists;
    std::tie(succeed, firstMaterialExists) =
        fileManager->fileExists(firstMaterialPath);
    if (!succeed) {
        return {false, String::empty()};
    }
    std::tie(succeed, lastMaterialExists) =
        fileManager->fileExists(lastMaterialPath);
    if (!succeed) {
        return {false, String::empty()};
    }
    if (firstMaterialExists && lastMaterialExists) {
        time_t firstMaterialModifiedTime, lastMaterialModifiedTime;
        bool succeed;
        std::tie(succeed, firstMaterialModifiedTime) =
            fileManager->getFileModifiedTime(firstMaterialPath);
        if (!succeed) {
            return {false, String::empty()};
        }
        std::tie(succeed, lastMaterialModifiedTime) =
            fileManager->getFileModifiedTime(lastMaterialPath);
        if (!succeed) {
            return {false, String::empty()};
        }
        return {true, firstMaterialModifiedTime > lastMaterialModifiedTime
                          ? std::move(firstMaterialPath)
                          : std::move(lastMaterialPath)};
    } else if (firstMaterialExists && !lastMaterialExists) {
        return {true, std::move(firstMaterialPath)};
    } else if (!firstMaterialExists && lastMaterialExists) {
        return {true, std::move(lastMaterialPath)};
    } else {
        return {true, String::empty()};
    }
}

std::pair<bool, std::string>
Factory::pickMaterialForOverwriting(const std::string &database)
{
    //If all materials exist, return the old one.
    //If all materials do not exist, return the first one.
    //Otherwise, return the one that does not exist.
    FileManager *fileManager = FileManager::shared();
    std::string firstMaterialPath = firstMaterialPathForDatabase(database);
    bool succeed, firstMaterialExists, lastMaterialExists;
    std::tie(succeed, firstMaterialExists) =
        fileManager->fileExists(firstMaterialPath);
    if (!firstMaterialExists) {
        if (succeed) {
            return {true, firstMaterialPath};
        }
        return {false, String::empty()};
    }

    std::string lastMaterialPath = lastMaterialPathForDatabase(database);
    std::tie(succeed, lastMaterialExists) =
        fileManager->fileExists(lastMaterialPath);
    if (!lastMaterialExists) {
        if (succeed) {
            return {true, lastMaterialPath};
        }
        return {false, String::empty()};
    }
    time_t firstMaterialModifiedTime, lastMaterialModifiedTime;
    std::tie(succeed, firstMaterialModifiedTime) =
        fileManager->getFileModifiedTime(firstMaterialPath);
    if (!succeed) {
        return {false, String::empty()};
    }
    std::tie(succeed, lastMaterialModifiedTime) =
        fileManager->getFileModifiedTime(lastMaterialPath);
    if (!succeed) {
        return {false, String::empty()};
    }
    return {true, firstMaterialModifiedTime > lastMaterialModifiedTime
                      ? std::move(lastMaterialPath)
                      : std::move(firstMaterialPath)};
}

} //namespace Repair

} //namespace WCDB
