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
#include <WCDB/Time.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Factory
Factory::Factory(const std::string &database_)
    : database(database_), directory(Path::addExtention(database_, ".factory"))
{
}

std::pair<bool, std::list<std::string>> Factory::getWorkshopDirectories() const
{
    std::list<std::string> workshopDirectories;
    std::string restoreDirectory = getRestoreDirectory();
    std::string renewDirectory = getRenewDirectory();
    if (FileManager::shared()->enumerateDirectory(
            directory,
            [&workshopDirectories, &restoreDirectory, &renewDirectory](
                const std::string &path, bool isDirectory) -> bool {
                if (isDirectory && path != restoreDirectory &&
                    path != renewDirectory) {
                    workshopDirectories.push_back(path);
                }
                return true;
            })) {
        return {true, workshopDirectories};
    }
    return {false, {}};
}

std::pair<bool, std::string> Factory::getUniqueWorkshopDiectory() const
{
    bool succeed;
    FileManager *fileManager = FileManager::shared();
    std::string path;
    do {
        Time time;
        if (!time.now()) {
            break;
        }
        std::string fileName;
        std::tie(succeed, fileName) = time.stringify();
        if (!succeed) {
            break;
        }
        path = Path::addComponent(directory, fileName);

        bool exists = false;
        std::tie(succeed, exists) = fileManager->directoryExists(path);
        if (exists) {
            succeed = false;
        }
    } while (false); //try repeatly
    return {succeed, succeed ? path : String::empty()};
}

bool Factory::canRetrieve() const
{
    bool result = false;
    std::string databaseName = getDatabaseName();
    FileManager::shared()->enumerateDirectory(
        directory,
        [&result, &databaseName](const std::string &subpath,
                                 bool isDirectory) -> bool {
            if (isDirectory) {
                bool succeed, exists;
                std::tie(succeed, exists) = FileManager::shared()->fileExists(
                    Path::addComponent(subpath, databaseName));
                if (exists) {
                    result = true;
                    return false;
                }
            }
            return true;
        });
    return result;
}

#pragma mark - Factory Related
void Factory::filter(const Filter &tableShouldBeBackedUp)
{
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    m_filter = tableShouldBeBackedUp;
}

Factory::Filter Factory::getFilter() const
{
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    return m_filter;
}

FactoryDepositor Factory::depositor()
{
    return FactoryDepositor(*this);
}

FactoryRetriever Factory::retriever()
{
    return FactoryRetriever(*this);
}

FactoryBackup Factory::backup()
{
    return FactoryBackup(*this);
}

FactoryRenewer Factory::renewer()
{
    return FactoryRenewer(*this);
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

std::string Factory::getRenewDirectory() const
{
    return Path::addComponent(directory, "renew");
}

std::string Factory::getDatabaseName() const
{
    return Path::getFileName(database);
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
Factory::materialForSerializingForDatabase(const std::string &database)
{
    //If all materials exist, return the old one.
    //Otherwise, return the one that does not exist.
    bool succeed;
    std::string materialPath;

    do {
        Time time;
        if (!time.now()) {
            break;
        }

        std::string firstMaterialPath =
            Factory::firstMaterialPathForDatabase(database);
        Time firstMaterialModifiedTime, lastMaterialModifiedTime;
        std::tie(succeed, firstMaterialModifiedTime) =
            getModifiedTimeOr0IfNotExists(firstMaterialPath);
        if (!succeed) {
            break;
        }
        if (firstMaterialModifiedTime.empty() ||
            firstMaterialModifiedTime.second() == time.second()) {
            materialPath = std::move(firstMaterialPath);
            break;
        }

        std::string lastMaterialPath =
            Factory::lastMaterialPathForDatabase(database);
        std::tie(succeed, lastMaterialModifiedTime) =
            getModifiedTimeOr0IfNotExists(lastMaterialPath);
        if (!succeed) {
            break;
        }
        if (lastMaterialModifiedTime.empty() ||
            lastMaterialModifiedTime.second() == time.second()) {
            materialPath = std::move(lastMaterialPath);
            break;
        }

        if (firstMaterialModifiedTime > lastMaterialModifiedTime) {
            materialPath = std::move(lastMaterialPath);
        } else {
            materialPath = std::move(firstMaterialPath);
        }
    } while (false);
    return {succeed, std::move(materialPath)};
}

std::pair<bool, std::list<std::string>>
Factory::materialsForDeserializingForDatabase(const std::string &database)
{
    //If all materials exist, return the new one.
    //If all materials do not exist, return empty.
    //Otherwise, return the existing one.
    bool succeed;
    std::list<std::string> materialPaths;

    do {
        std::string firstMaterialPath =
            Factory::firstMaterialPathForDatabase(database);
        std::string lastMaterialPath =
            Factory::lastMaterialPathForDatabase(database);
        Time firstMaterialModifiedTime, lastMaterialModifiedTime;

        std::tie(succeed, firstMaterialModifiedTime) =
            getModifiedTimeOr0IfNotExists(firstMaterialPath);
        if (!succeed) {
            break;
        }
        std::tie(succeed, lastMaterialModifiedTime) =
            getModifiedTimeOr0IfNotExists(lastMaterialPath);
        if (!succeed) {
            break;
        }

        if (!firstMaterialModifiedTime.empty()) {
            materialPaths.push_back(firstMaterialPath);
        }

        if (!lastMaterialModifiedTime.empty()) {
            materialPaths.push_back(lastMaterialPath);
        }

        if (firstMaterialModifiedTime < lastMaterialModifiedTime) {
            materialPaths.reverse();
        }
    } while (false);
    if (!succeed) {
        materialPaths.clear();
    }
    return {succeed, std::move(materialPaths)};
}

std::pair<bool, Time>
Factory::getModifiedTimeOr0IfNotExists(const std::string &path)
{
    FileManager *fileManager = FileManager::shared();
    bool succeed, exists;
    Time modifiedTime;
    do {
        std::tie(succeed, exists) = fileManager->fileExists(path);
        if (!succeed || !exists) {
            break;
        }
        std::tie(succeed, modifiedTime) =
            fileManager->getFileModifiedTime(path);
    } while (false);
    return {succeed, modifiedTime};
}

} //namespace Repair

} //namespace WCDB
