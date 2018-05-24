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

#include <WCDB/FileManager.hpp>
#include <WCDB/Path.hpp>
#include <WCDB/Reporter.hpp>
#include <WCDB/String.hpp>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

namespace WCDB {

FileManager::FileManager()
{
}

FileManager *FileManager::shared()
{
    static FileManager s_fileManager;
    return &s_fileManager;
}

#pragma mark - Basic
std::pair<bool, size_t> FileManager::getFileSize(const std::string &path)
{
    struct stat temp;
    if (lstat(path.c_str(), &temp) == 0) {
        return {true, (size_t) temp.st_size};
    } else if (errno == ENOENT) {
        return {true, 0};
    }
    setThreadedError(path);
    return {false, 0};
}

std::pair<bool, bool> FileManager::isExists(const std::string &path)
{
    if (access(path.c_str(), F_OK) == 0) {
        return {true, true};
    } else if (errno == ENOENT) {
        return {true, false};
    }
    setThreadedError(path);
    return {false, false};
}

bool FileManager::createHardLink(const std::string &from, const std::string &to)
{
    if (link(from.c_str(), to.c_str()) == 0) {
        return true;
    }
    setThreadedError(to);
    return false;
}

bool FileManager::removeHardLink(const std::string &path)
{
    if (unlink(path.c_str()) == 0 || errno == ENOENT) {
        return true;
    }
    setThreadedError(path);
    return false;
}

bool FileManager::removeFile(const std::string &path)
{
    if (remove(path.c_str()) == 0 || errno == ENOENT) {
        return true;
    }
    setThreadedError(path);
    return false;
}

bool FileManager::createDirectory(const std::string &path)
{
    if (mkdir(path.c_str(), 0755) == 0) {
        return true;
    }
    setThreadedError(path);
    return false;
}

#pragma mark - Combination
std::pair<bool, size_t>
FileManager::getFilesSize(const std::list<std::string> &paths)
{
    size_t size = 0;
    std::pair<bool, size_t> ret;
    for (const auto &path : paths) {
        ret = getFileSize(path);
        if (ret.first) {
            size += ret.second;
        } else {
            return {false, 0};
        }
    }
    return {true, size};
}

bool FileManager::moveFiles(const std::list<std::string> &paths,
                            const std::string &directory)
{
    if (!createDirectoryWithIntermediateDirectories(directory)) {
        return false;
    }
    std::list<std::pair<std::string, std::string>> pairedPaths;
    for (const auto &path : paths) {
        const std::string fileName = Path::getFileName(path);
        std::string newPath = Path::addComponent(directory, fileName);
        pairedPaths.push_back({path, newPath});
    }
    return moveFiles(pairedPaths);
}

bool FileManager::moveFiles(
    const std::list<std::pair<std::string, std::string>> &pairedPaths)
{
    bool result = true;
    std::list<std::string> recovers;
    for (const auto &pairedPath : pairedPaths) {
        std::pair<bool, bool> ret = isExists(pairedPath.first);
        if (!ret.first) {
            break;
        }
        if (ret.second) {
            const std::string &newPath = pairedPath.second;
            if (!removeFile(newPath) ||
                !createHardLink(pairedPath.first, pairedPath.second)) {
                result = false;
                break;
            }
            recovers.push_back(newPath);
        }
    }
    if (result) {
        for (const auto &pairedPath : pairedPaths) {
            if (!removeFile(pairedPath.first)) {
                return false;
            }
        }
    } else {
        for (const auto &recover : recovers) {
            removeHardLink(recover.c_str());
        }
    }
    return result;
}

bool FileManager::removeFiles(const std::list<std::string> &paths)
{
    for (const auto &path : paths) {
        if (!removeFile(path)) {
            return false;
        }
    }
    return true;
}

bool FileManager::createDirectoryWithIntermediateDirectories(
    const std::string &path)
{
    auto ret = isExists(path);
    if (!ret.first) {
        return false;
    }
    if (!ret.second) {
        return createDirectoryWithIntermediateDirectories(
                   Path::getBaseName(path)) &&
               createDirectory(path);
    }
    return true;
}

#pragma mark - Error
void FileManager::setThreadedError(const std::string &path)
{
    Error error;
    error.setSystemCode(errno, Error::Code::IOError);
    error.message = strerror(errno);
    error.infos.set("Path", path);
    Reporter::shared()->report(error);
    SharedThreadedErrorProne::setThreadedError(std::move(error));
}

} //namespace WCDB
