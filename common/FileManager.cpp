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
#include <dirent.h>
#include <errno.h>
#include <iomanip>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace WCDB {

FileManager::FileManager()
{
}

FileManager *FileManager::shared()
{
    static FileManager *s_fileManager = new FileManager;
    return s_fileManager;
}

#pragma mark - Basic
std::tuple<bool, bool, bool> FileManager::itemExists(const std::string &path)
{
    struct stat s;
    if (stat(path.c_str(), &s) == 0) {
        if (s.st_mode & S_IFDIR) {
            return {true, true, true};
        } else {
            return {true, true, false};
        }
    } else if (errno == ENOENT) {
        return {true, false, false};
    }
    setThreadedError(path);
    return {false, false, false};
}

std::pair<bool, size_t> FileManager::getFileSize(const std::string &file)
{
    struct stat temp;
    if (lstat(file.c_str(), &temp) == 0) {
        return {true, (size_t) temp.st_size};
    } else if (errno == ENOENT) {
        return {true, 0};
    }
    setThreadedError(file);
    return {false, 0};
}

std::pair<bool, size_t>
FileManager::getDirectorySize(const std::string &directory)
{
    size_t totalSize = 0;
    if (enumerateDirectory(
            directory,
            [&totalSize, this](const std::string &path, bool isDirectory) {
                auto intermediate =
                    isDirectory ? getDirectorySize(path) : getFileSize(path);
                totalSize += intermediate.second;
                return intermediate.first;
            })) {
        return {true, totalSize};
    };
    return {false, 0};
}

bool FileManager::enumerateDirectory(
    const std::string &directory,
    const std::function<bool(const std::string &, bool)> &enumeration)
{
    DIR *dir = opendir(directory.c_str());
    if (dir == NULL) {
        if (errno == ENOENT) {
            return true;
        }
        setThreadedError(directory);
        return false;
    }

    std::string path;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0) {
            path = Path::addComponent(directory, entry->d_name);
            if (!enumeration(path, entry->d_type == DT_DIR)) {
                break;
            }
        }
    }
    closedir(dir);

    return true;
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

bool FileManager::removeFile(const std::string &file)
{
    if (remove(file.c_str()) == 0 || errno == ENOENT) {
        return true;
    }
    setThreadedError(file);
    return false;
}

bool FileManager::removeDirectory(const std::string &directory)
{
    DIR *dir = opendir(directory.c_str());
    if (dir == NULL) {
        if (errno == ENOENT) {
            return true;
        }
        setThreadedError(directory);
        return false;
    }

    std::string path;
    struct dirent *entry;
    bool result;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            path = Path::addComponent(directory, entry->d_name);
            if (entry->d_type == DT_DIR) {
                result = removeDirectory(path);
            } else {
                result = removeFile(path);
            }
            if (!result) {
                setThreadedError(path);
                return false;
            }
        }
    }
    closedir(dir);
    return true;
}

bool FileManager::createDirectory(const std::string &path)
{
    if (mkdir(path.c_str(), 0755) == 0) {
        return true;
    }
    setThreadedError(path);
    return false;
}

std::pair<bool, time_t>
FileManager::getFileModifiedTime(const std::string &path)
{
    struct stat result;
    if (stat(path.c_str(), &result) == 0) {
        return {true, result.st_mtime};
    }
    setThreadedError(path);
    return {false, 0};
}

std::pair<bool, std::string> FileManager::getUniqueFileName()
{
    auto t = std::time(nullptr);
    srand((unsigned) t);
    struct tm tm;
    if (!localtime_r(&t, &tm)) {
        setThreadedError(Error::Code::Exceed);
        return {false, String::empty()};
    }
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S_");
    oss << rand();
    return {true, oss.str()};
}

#pragma mark - Combination
std::pair<bool, size_t> FileManager::getItemSize(const std::string &path)
{
    struct stat temp;
    if (lstat(path.c_str(), &temp) == 0) {
        if (temp.st_mode & S_IFDIR) {
            return getDirectorySize(path);
        }
        return getFileSize(path);
    } else if (errno == ENOENT) {
        return {true, 0};
    }
    setThreadedError(path);
    return {false, 0};
}

std::pair<bool, bool> FileManager::fileExists(const std::string &file)
{
    bool succeed, exists, isDirectory;
    std::tie(succeed, exists, isDirectory) = itemExists(file);
    return {succeed, exists && !isDirectory};
}

std::pair<bool, bool> FileManager::directoryExists(const std::string &directory)
{
    bool succeed, exists, isDirectory;
    std::tie(succeed, exists, isDirectory) = itemExists(directory);
    return {succeed, exists && isDirectory};
}

std::pair<bool, size_t>
FileManager::getItemsSize(const std::list<std::string> &paths)
{
    size_t size = 0;
    std::pair<bool, size_t> temp;
    for (const auto &path : paths) {
        temp = getItemSize(path);
        if (!temp.first) {
            return {false, 0};
        }
        size += temp.second;
    }
    return {true, size};
}

bool FileManager::removeItem(const std::string &path)
{
    struct stat temp;
    if (lstat(path.c_str(), &temp) == 0) {
        if (temp.st_mode & S_IFDIR) {
            return removeDirectory(path);
        }
        return removeFile(path);
    } else if (errno == ENOENT) {
        return true;
    }
    setThreadedError(path);
    return false;
}

bool FileManager::removeItems(const std::list<std::string> &items)
{
    for (const auto &item : items) {
        if (!removeItem(item)) {
            return false;
        }
    }
    return true;
}

bool FileManager::moveItems(const std::list<std::string> &paths,
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
    return moveItems(pairedPaths);
}

bool FileManager::moveItems(
    const std::list<std::pair<std::string, std::string>> &pairedPaths)
{
    bool result = true;
    std::list<std::string> recovers;
    for (const auto &pairedPath : pairedPaths) {
        std::pair<bool, bool> ret = fileExists(pairedPath.first);
        if (!ret.first) {
            break;
        }
        if (ret.second) {
            const std::string &newPath = pairedPath.second;
            if (!removeItem(newPath) ||
                !createHardLink(pairedPath.first, pairedPath.second)) {
                result = false;
                break;
            }
            recovers.push_back(newPath);
        }
    }
    if (result) {
        for (const auto &pairedPath : pairedPaths) {
            if (!removeItem(pairedPath.first)) {
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

bool FileManager::createDirectoryWithIntermediateDirectories(
    const std::string &path)
{
    auto ret = fileExists(path);
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

void FileManager::setThreadedError(Error::Code codeIfUnresolved)
{
    Error error;
    error.setSystemCode(errno, codeIfUnresolved);
    error.message = strerror(errno);
    Reporter::shared()->report(error);
    SharedThreadedErrorProne::setThreadedError(std::move(error));
}

} //namespace WCDB
