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

#include <WCDB/database.hpp>
#include <WCDB/file.hpp>
#include <WCDB/path.hpp>
#include <WCDB/utility.hpp>

namespace WCDB {

bool Database::removeFiles(Error &error)
{
    if (!isBlockaded() || isOpened()) {
        WCDB::Error::Warning(
            "Removing files on an opened database may cause unknown results");
    }
    return File::removeFiles(getPaths(), error);
}

size_t Database::getFilesSize(Error &error)
{
    if (!isBlockaded() || isOpened()) {
        WCDB::Error::Warning("Getting files size on an opened database may get "
                             "incorrect results");
    }
    return File::getFilesSize(getPaths(), error);
}

bool Database::moveFiles(const std::string &directory, Error &error)
{
    if (!isBlockaded() || isOpened()) {
        WCDB::Error::Warning("Moving files on an opened database may cause a "
                             "corrupted database");
    }
    return File::moveFiles(getPaths(), directory, error);
}

bool Database::moveFilesToDirectoryWithExtraFiles(
    const std::string &directory,
    const std::list<std::string> &extraFiles,
    Error &error)
{
    if (!isBlockaded() || isOpened()) {
        WCDB::Error::Warning("Moving files on an opened database may cause a "
                             "corrupted database");
    }
    std::list<std::string> paths = getPaths();
    paths.insert(paths.end(), extraFiles.begin(), extraFiles.end());
    return File::moveFiles(paths, directory, error);
}

const std::array<std::string, 5> &Database::subfixs()
{
    static const std::array<std::string, 5> s_subfixs = {
        "", //db file
        "-wal", "-journal", "-shm", Handle::backupSuffix,
    };
    return s_subfixs;
}

const std::list<std::string> Database::getPaths() const
{
    std::list<std::string> paths;
    for (const auto &subfix : Database::subfixs()) {
        paths.push_back(Path::addExtention(getPath(), subfix));
    }
    return paths;
}

} //namespace WCDB
