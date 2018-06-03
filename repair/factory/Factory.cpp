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

namespace WCDB {

namespace Repair {

#pragma mark - Factory
Factory::Factory(const std::string &database_)
    : database(database_)
    , directory(Path::addExtention(database_, ".factory"))
    , m_materials(*this)
{
}

std::list<std::string> Factory::getAssociatedPaths() const
{
    return {
        database,
        Path::addExtention(database, "-journal"),
        Path::addExtention(database, "-wal"),
        Path::addExtention(database, "-shm"),
        getFirstMaterialPath(),
        getLastMaterialPath(),
    };
}

std::string Factory::getFirstMaterialPath() const
{
    return firstMaterialPathForDatabase(database);
}

std::string Factory::getLastMaterialPath() const
{
    return lastMaterialPathForDatabase(database);
}

FactoryArchiver Factory::archiver() const
{
    return FactoryArchiver(*this);
}

FactoryRestorer Factory::restorer() const
{
    return FactoryRestorer(*this);
}

std::string Factory::firstMaterialPathForDatabase(const std::string &database)
{
    return Path::addExtention(database, "-first.material");
}

std::string Factory::lastMaterialPathForDatabase(const std::string &database)
{
    return Path::addExtention(database, "-last.material");
}

std::pair<bool, std::string>
Factory::pickMaterailForRestoring(const std::string &database)
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

FactoryMaterials &Factory::materials()
{
    return m_materials;
}

} //namespace Repair

} //namespace WCDB
