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
Factory::Factory(const String &database_)
: database(database_), directory(Path::addExtention(database_, ".factory"))
{
}

std::pair<bool, std::list<String>> Factory::getWorkshopDirectories() const
{
    std::list<String> workshopDirectories;
    if (FileManager::enumerateDirectory(
        directory, [&workshopDirectories](const String &root, const String &subpath, bool isDirectory) -> bool {
            if (isDirectory && subpath != restoreDirectoryName && subpath != renewDirectoryName) {
                workshopDirectories.push_back(Path::addComponent(root, subpath));
            }
            return true;
        })) {
        return { true, workshopDirectories };
    }
    return { false, {} };
}

std::pair<bool, String> Factory::getUniqueWorkshopDiectory() const
{
    bool succeed = false;
    String path;
    do {
        Time time = Time::now();
        String fileName = time.stringify();
        if (fileName.empty()) {
            break;
        }
        path = Path::addComponent(directory, fileName);

        bool exists = false;
        std::tie(succeed, exists) = FileManager::directoryExists(path);
        if (exists) {
            succeed = false;
        }
    } while (false); //try repeatly
    return { succeed, succeed ? path : String::null() };
}

bool Factory::containsDeposited() const
{
    bool result = false;
    String databaseName = getDatabaseName();
    FileManager::enumerateDirectory(
    directory,
    [&result, &databaseName](const String &root, const String &subpath, bool isDirectory) -> bool {
        if (isDirectory && subpath != restoreDirectoryName && subpath != renewDirectoryName) {
            String databasePath
            = Path::addComponent(Path::addComponent(root, subpath), databaseName);
            bool succeed, exists;
            std::tie(succeed, exists) = FileManager::fileExists(databasePath);
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
    m_filter = tableShouldBeBackedUp;
}

Factory::Filter Factory::getFilter() const
{
    return m_filter;
}

FactoryDepositor Factory::depositor() const
{
    return FactoryDepositor(*this);
}

FactoryRetriever Factory::retriever() const
{
    return FactoryRetriever(*this);
}

FactoryBackup Factory::backup() const
{
    return FactoryBackup(*this);
}

FactoryRenewer Factory::renewer() const
{
    return FactoryRenewer(*this);
}

#pragma mark - Helper
bool Factory::removeDeposited() const
{
    std::list<String> depositedPath;
    FileManager::enumerateDirectory(
    directory, [&depositedPath](const String &root, const String &subpath, bool isDirectory) -> bool {
        if (isDirectory && subpath != renewDirectoryName && subpath != restoreDirectoryName) {
            depositedPath.push_back(Path::addComponent(root, subpath));
        }
        return true;
    });
    if (FileManager::removeItems(depositedPath)) {
        removeDirectoryIfEmpty();
        return true;
    }
    return false;
}

bool Factory::removeDirectoryIfEmpty() const
{
    bool canRemove = true;
    bool succeed = FileManager::enumerateDirectory(
    directory, [&canRemove](const String &root, const String &subpath, bool isDirectory) -> bool {
        WCDB_UNUSED(root)
        if (subpath == restoreDirectoryName || !isDirectory) {
            return true;
        }
        canRemove = false;
        return false;
    });
    if (!succeed) {
        return false;
    }
    if (canRemove) {
        return FileManager::removeItem(directory);
    }
    return true;
}

String Factory::firstMaterialPathForDatabase(const String &database)
{
    return Path::addExtention(database, "-first.material");
}

String Factory::lastMaterialPathForDatabase(const String &database)
{
    return Path::addExtention(database, "-last.material");
}

String Factory::getRestoreDirectory() const
{
    return Path::addComponent(directory, restoreDirectoryName);
}

String Factory::getRenewDirectory() const
{
    return Path::addComponent(directory, renewDirectoryName);
}

String Factory::getDatabaseName() const
{
    return Path::getFileName(database);
}

std::list<String> Factory::associatedPathsForDatabase(const String &database)
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

std::list<String> Factory::databasePathsForDatabase(const String &database)
{
    return {
        database,
        Path::addExtention(database, "-journal"),
        Path::addExtention(database, "-wal"),
        Path::addExtention(database, "-shm"),
    };
}

std::pair<bool, String> Factory::materialForSerializingForDatabase(const String &database)
{
    //If all materials exist, return the old one.
    //Otherwise, return the one that does not exist.
    bool succeed;
    String materialPath;

    do {
        Time time = Time::now();

        String firstMaterialPath = Factory::firstMaterialPathForDatabase(database);
        Time firstMaterialModifiedTime, lastMaterialModifiedTime;
        std::tie(succeed, firstMaterialModifiedTime)
        = getModifiedTimeOr0IfNotExists(firstMaterialPath);
        if (!succeed) {
            break;
        }
        if (firstMaterialModifiedTime.empty()
            || firstMaterialModifiedTime.seconds() == time.seconds()) {
            materialPath = std::move(firstMaterialPath);
            break;
        }

        String lastMaterialPath = Factory::lastMaterialPathForDatabase(database);
        std::tie(succeed, lastMaterialModifiedTime)
        = getModifiedTimeOr0IfNotExists(lastMaterialPath);
        if (!succeed) {
            break;
        }
        if (lastMaterialModifiedTime.empty()
            || lastMaterialModifiedTime.seconds() == time.seconds()) {
            materialPath = std::move(lastMaterialPath);
            break;
        }

        if (firstMaterialModifiedTime > lastMaterialModifiedTime) {
            materialPath = std::move(lastMaterialPath);
        } else {
            materialPath = std::move(firstMaterialPath);
        }
    } while (false);
    return { succeed, std::move(materialPath) };
}

std::pair<bool, std::list<String>>
Factory::materialsForDeserializingForDatabase(const String &database)
{
    //If all materials exist, return the new one.
    //If all materials do not exist, return empty.
    //Otherwise, return the existing one.
    bool succeed;
    std::list<String> materialPaths;

    do {
        String firstMaterialPath = Factory::firstMaterialPathForDatabase(database);
        String lastMaterialPath = Factory::lastMaterialPathForDatabase(database);
        Time firstMaterialModifiedTime, lastMaterialModifiedTime;

        std::tie(succeed, firstMaterialModifiedTime)
        = getModifiedTimeOr0IfNotExists(firstMaterialPath);
        if (!succeed) {
            break;
        }
        std::tie(succeed, lastMaterialModifiedTime)
        = getModifiedTimeOr0IfNotExists(lastMaterialPath);
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
    return { succeed, std::move(materialPaths) };
}

std::pair<bool, Time> Factory::getModifiedTimeOr0IfNotExists(const String &path)
{
    bool succeed, exists;
    Time modifiedTime;
    do {
        std::tie(succeed, exists) = FileManager::fileExists(path);
        if (!succeed || !exists) {
            break;
        }
        std::tie(succeed, modifiedTime) = FileManager::getFileModifiedTime(path);
    } while (false);
    return { succeed, modifiedTime };
}

} //namespace Repair

} //namespace WCDB
