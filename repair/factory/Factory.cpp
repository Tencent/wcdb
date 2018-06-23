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
            return {false, String::empty()};
        }
        std::string fileName;
        std::tie(succeed, fileName) = time.stringify();
        if (!succeed) {
            return {false, String::empty()};
        }
        path = Path::addComponent(directory, fileName);

        bool exists = false;
        bool isDirectory = false;
        std::tie(succeed, exists, isDirectory) = fileManager->itemExists(path);
        if (!succeed) {
            return {false, String::empty()};
        }
        if (!exists) {
            break;
        }
    } while (true);
    WCTInnerAssert(!path.empty());
    return {true, path};
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
        std::string firstMaterialPath =
            Factory::firstMaterialPathForDatabase(database);
        Time firstMaterialModifiedTime, lastMaterialModifiedTime;
        std::tie(succeed, firstMaterialModifiedTime) =
            getModifiedTimeOr0IfNotExists(firstMaterialPath);
        if (!succeed) {
            break;
        }
        if (firstMaterialModifiedTime.empty()) {
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
        if (lastMaterialModifiedTime.empty()) {
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

std::pair<bool, std::string>
Factory::materialForDeserializingForDatabase(const std::string &database)
{
    //If all materials exist, return the new one.
    //If all materials do not exist, return empty.
    //Otherwise, return the existing one.
    bool succeed;
    std::string materialPath;

    do {
        std::string firstMaterialPath =
            Factory::firstMaterialPathForDatabase(database);
        Time firstMaterialModifiedTime, lastMaterialModifiedTime;
        std::tie(succeed, firstMaterialModifiedTime) =
            getModifiedTimeOr0IfNotExists(firstMaterialPath);
        if (!succeed) {
            break;
        }

        std::string lastMaterialPath =
            Factory::lastMaterialPathForDatabase(database);
        std::tie(succeed, lastMaterialModifiedTime) =
            getModifiedTimeOr0IfNotExists(lastMaterialPath);
        if (!succeed) {
            break;
        }

        if (!firstMaterialModifiedTime.empty() ||
            !lastMaterialModifiedTime.empty()) {
            if (firstMaterialModifiedTime > lastMaterialModifiedTime) {
                materialPath = std::move(firstMaterialPath);
            } else {
                materialPath = std::move(lastMaterialPath);
            }
        }
    } while (false);
    return {succeed, std::move(materialPath)};
}

std::pair<bool, Time>
Factory::getModifiedTimeOr0IfNotExists(const std::string &path)
{
    FileManager *fileManager = FileManager::shared();
    bool succeed, exists, isDirectory;

    Time modifiedTime;
    do {
        std::tie(succeed, exists, isDirectory) = fileManager->itemExists(path);
        if (!succeed || !exists) {
            break;
        }
        if (isDirectory && !fileManager->removeItem(path)) {
            succeed = false;
            break;
        }
        std::tie(succeed, modifiedTime) =
            fileManager->getFileModifiedTime(path);
    } while (false);
    return {succeed, modifiedTime};
}

} //namespace Repair

} //namespace WCDB
