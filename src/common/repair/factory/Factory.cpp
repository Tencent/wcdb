//
// Created by sanhuazhang on 2018/05/31
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

#include "Factory.hpp"
#include "Assemble.hpp"
#include "Assertion.hpp"
#include "FileManager.hpp"
#include "Material.hpp"
#include "Path.hpp"
#include "StringView.hpp"
#include "Time.hpp"

namespace WCDB {

namespace Repair {

#pragma mark - Factory
Factory::Factory(const UnsafeStringView &database_)
: database(database_), directory(factoryPathForDatabase(database_))
{
}

Optional<std::list<StringView>> Factory::getWorkshopDirectories() const
{
    Optional<std::list<StringView>> result;
    std::list<StringView> workshopDirectories;
    if (FileManager::enumerateDirectory(
        directory,
        [&workshopDirectories](
        const UnsafeStringView &root, const UnsafeStringView &subpath, bool isDirectory) -> bool {
            if (isDirectory && isWorkshopDirectory(subpath)) {
                workshopDirectories.push_back(Path::addComponent(root, subpath));
            }
            return true;
        })) {
        result = std::move(workshopDirectories);
    }
    return result;
}

Optional<StringView> Factory::getUniqueWorkshopDiectory() const
{
    StringView path;
    do {
        Time time = Time::now();
        auto fileName = time.stringify();
        if (!fileName.succeed()) {
            return NullOpt;
        }
        path = Path::addComponent(directory, fileName.value());

        auto exists = FileManager::directoryExists(path);
        if (!exists.succeed()) {
            return NullOpt;
        }
        if (!exists.value()) {
            break;
        }
    } while (true); //try repeatly
    return path;
}

bool Factory::containsDeposited() const
{
    bool contains = false;
    StringView databaseName = getDatabaseName();
    FileManager::enumerateDirectory(
    directory,
    [&contains, &databaseName](
    const UnsafeStringView &root, const UnsafeStringView &subpath, bool isDirectory) -> bool {
        if (isDirectory && isWorkshopDirectory(subpath)) {
            StringView databasePath
            = Path::addComponent(Path::addComponent(root, subpath), databaseName);
            auto exist = FileManager::fileExists(databasePath);
            if (exist.succeed() && exist.value()) {
                contains = true;
                return false;
            }
        }
        return true;
    });
    return contains;
}

bool Factory::isWorkshopDirectory(const UnsafeStringView &dir)
{
    return dir != restoreDirectoryName && dir != renewDirectoryName && dir != vacuumDirectoryName;
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

FactoryVacuum Factory::vacuumer() const
{
    return FactoryVacuum(*this);
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
    std::list<StringView> depositedPath;
    if (!FileManager::enumerateDirectory(
        directory,
        [&depositedPath](
        const UnsafeStringView &root, const UnsafeStringView &subpath, bool isDirectory) -> bool {
            if (isDirectory && isWorkshopDirectory(subpath)) {
                depositedPath.push_back(Path::addComponent(root, subpath));
            }
            return true;
        })) {
        return false;
    }
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
    directory,
    [&canRemove](const UnsafeStringView &root, const UnsafeStringView &subpath, bool isDirectory) -> bool {
        WCDB_UNUSED(root);
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

StringView Factory::incrementalMaterialPathForDatabase(const UnsafeStringView &database)
{
    return Path::addExtention(database, "-incremental.material");
}

StringView Factory::firstMaterialPathForDatabase(const UnsafeStringView &database)
{
    return Path::addExtention(database, "-first.material");
}

StringView Factory::lastMaterialPathForDatabase(const UnsafeStringView &database)
{
    return Path::addExtention(database, "-last.material");
}

StringView Factory::factoryPathForDatabase(const UnsafeStringView &database)
{
    return Path::addExtention(database, ".factory");
}

StringView Factory::getRestoreDirectory() const
{
    return Path::addComponent(directory, restoreDirectoryName);
}

StringView Factory::getRenewDirectory() const
{
    return Path::addComponent(directory, renewDirectoryName);
}

StringView Factory::getVacuumDirectory() const
{
    return Path::addComponent(directory, vacuumDirectoryName);
}

StringView Factory::getDatabaseName() const
{
    return Path::getFileName(database);
}

std::list<StringView> Factory::associatedPathsForDatabase(const UnsafeStringView &database)
{
    return {
        StringView(database),
        Path::addExtention(database, "-journal"),
        Path::addExtention(database, "-wal"),
        Path::addExtention(database, "-shm"),
        incrementalMaterialPathForDatabase(database),
        firstMaterialPathForDatabase(database),
        lastMaterialPathForDatabase(database),
    };
}

std::list<StringView> Factory::databasePathsForDatabase(const UnsafeStringView &database)
{
    return {
        StringView(database),
        Path::addExtention(database, "-journal"),
        Path::addExtention(database, "-wal"),
        Path::addExtention(database, "-shm"),
    };
}

Optional<StringView>
Factory::materialForSerializingForDatabase(const UnsafeStringView &database)
{
    //If all materials exist, return the old one.
    //Otherwise, return the one that does not exist.
    Time now = Time::now();

    StringView firstMaterialPath = Factory::firstMaterialPathForDatabase(database);
    auto optionalFirstMaterialModifiedTime
    = getModifiedTimeOr0IfNotExists(firstMaterialPath);
    if (!optionalFirstMaterialModifiedTime.succeed()) {
        return NullOpt;
    }
    auto &firstMaterialModifiedTime = optionalFirstMaterialModifiedTime.value();
    if (firstMaterialModifiedTime.empty()
        || firstMaterialModifiedTime.seconds() > now.seconds()) {
        return firstMaterialPath;
    }

    StringView lastMaterialPath = Factory::lastMaterialPathForDatabase(database);
    auto optionalLastMaterialModifiedTime = getModifiedTimeOr0IfNotExists(lastMaterialPath);
    if (!optionalLastMaterialModifiedTime.succeed()) {
        return NullOpt;
    }
    auto &lastMaterialModifiedTime = optionalLastMaterialModifiedTime.value();
    if (lastMaterialModifiedTime.empty()
        || lastMaterialModifiedTime.seconds() > now.seconds()) {
        return lastMaterialPath;
    }

    return firstMaterialModifiedTime > lastMaterialModifiedTime ? lastMaterialPath : firstMaterialPath;
}

Optional<StringView> Factory::latestMaterialForDatabase(const UnsafeStringView &database)
{
    Time now = Time::now();

    StringView firstMaterialPath = Factory::firstMaterialPathForDatabase(database);
    auto optionalFirstMaterialModifiedTime
    = getModifiedTimeOr0IfNotExists(firstMaterialPath);
    if (!optionalFirstMaterialModifiedTime.succeed()) {
        return NullOpt;
    }
    auto &firstMaterialModifiedTime = optionalFirstMaterialModifiedTime.value();
    StringView lastMaterialPath = Factory::lastMaterialPathForDatabase(database);
    auto optionalLastMaterialModifiedTime = getModifiedTimeOr0IfNotExists(lastMaterialPath);
    if (!optionalLastMaterialModifiedTime.succeed()) {
        return NullOpt;
    }
    auto &lastMaterialModifiedTime = optionalLastMaterialModifiedTime.value();
    if (lastMaterialModifiedTime.empty() && firstMaterialModifiedTime.empty()) {
        return "";
    }
    if (firstMaterialModifiedTime.empty()
        || firstMaterialModifiedTime.seconds() > now.seconds()) {
        return lastMaterialPath;
    }
    if (lastMaterialModifiedTime.empty()
        || lastMaterialModifiedTime.seconds() > now.seconds()) {
        return firstMaterialPath;
    }
    return firstMaterialModifiedTime < lastMaterialModifiedTime ? lastMaterialPath : firstMaterialPath;
}

Optional<std::list<StringView>>
Factory::materialsForDeserializingForDatabase(const UnsafeStringView &database)
{
    //If all materials exist, return the new one.
    //If all materials do not exist, return empty.
    //Otherwise, return the existing one.
    std::list<StringView> materialPaths;

    StringView firstMaterialPath = Factory::firstMaterialPathForDatabase(database);
    auto optionalFirstMaterialModifiedTime
    = getModifiedTimeOr0IfNotExists(firstMaterialPath);
    if (!optionalFirstMaterialModifiedTime.succeed()) {
        return NullOpt;
    }
    Time &firstMaterialModifiedTime = optionalFirstMaterialModifiedTime.value();
    if (!firstMaterialModifiedTime.empty()) {
        materialPaths.push_back(firstMaterialPath);
    }

    StringView lastMaterialPath = Factory::lastMaterialPathForDatabase(database);
    auto optionalLastMaterialModifiedTime = getModifiedTimeOr0IfNotExists(lastMaterialPath);
    if (!optionalLastMaterialModifiedTime.succeed()) {
        return NullOpt;
    }
    Time &lastMaterialModifiedTime = optionalLastMaterialModifiedTime.value();
    if (!lastMaterialModifiedTime.empty()) {
        materialPaths.push_back(lastMaterialPath);
    }

    if (firstMaterialModifiedTime < lastMaterialModifiedTime) {
        materialPaths.reverse();
    }
    return materialPaths;
}

Optional<Time> Factory::getModifiedTimeOr0IfNotExists(const UnsafeStringView &path)
{
    Optional<Time> modifiedTime;
    auto exists = FileManager::fileExistsAndNotEmpty(path);
    if (exists.succeed()) {
        if (exists.value()) {
            modifiedTime = FileManager::getFileModifiedTime(path);
        } else {
            modifiedTime = Time();
        }
    }
    return modifiedTime;
}

} //namespace Repair

} //namespace WCDB
