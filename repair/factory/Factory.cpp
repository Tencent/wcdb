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

#include <WCDB/Factory.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/Path.hpp>
#include <WCDB/ThreadedErrors.hpp>

namespace WCDB {

namespace Repair {

Factory::Factory(const std::string &database_)
    : database(database_), directory(Path::addExtention(database_, ".factory"))
{
}

bool Factory::archive()
{
    FileManager *fileManager = FileManager::shared();
    auto intermediate = fileManager->getUniqueFileName();
    if (!intermediate.first) {
        assignWithSharedThreadedError();
        return false;
    }
    std::string archivedDirectory =
        Path::addComponent(directory, intermediate.second);
    if (!fileManager->createDirectoryWithIntermediateDirectories(
            archivedDirectory) ||
        !fileManager->moveItems(getAssociatedPaths(), archivedDirectory)) {
        assignWithSharedThreadedError();
        return false;
    }
    return true;
}

std::list<std::string> Factory::getAssociatedPaths()
{
    return {
        database,
        Path::addExtention(database, "-journal"),
        Path::addExtention(database, "-wal"),
        Path::addExtention(database, "-shm"),
        Path::addExtention(database, "-first.material"),
        Path::addExtention(database, "-last.material"),
    };
}

const Error &Factory::getError() const
{
    return m_error;
}

void Factory::assignWithSharedThreadedError()
{
    m_error = std::move(ThreadedErrors::shared()->moveThreadedError());
}

} //namespace Repair

} //namespace WCDB
