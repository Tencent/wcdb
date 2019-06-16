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

#include <WCDB/CoreConst.h>
#include <WCDB/FileManager.hpp>
#include <WCDB/Notifier.hpp>
#include <WCDB/Path.hpp>
#include <WCDB/StringView.hpp>
#include <WCDB/UnsafeData.hpp>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace WCDB {

#pragma mark - Basic
std::tuple<bool, bool, bool> FileManager::itemExists(const UnsafeStringView &path)
{
    struct stat s;
    if (stat(path.data(), &s) == 0) {
        if ((s.st_mode & S_IFMT) == S_IFDIR) {
            return { true, true, true };
        } else {
            return { true, true, false };
        }
    } else if (errno == ENOENT) {
        return { true, false, false };
    }
    setThreadedError(path);
    return { false, false, false };
}

std::pair<bool, size_t> FileManager::getFileSize(const UnsafeStringView &file)
{
    struct stat temp;
    if (stat(file.data(), &temp) == 0) {
        return { true, (size_t) temp.st_size };
    } else if (errno == ENOENT) {
        return { true, 0 };
    }
    setThreadedError(file);
    return { false, 0 };
}

std::pair<bool, size_t> FileManager::getDirectorySize(const UnsafeStringView &directory)
{
    bool succeed = true;
    size_t totalSize = 0;
    if (enumerateDirectory(
        directory,
        [&succeed, &totalSize](
        const UnsafeStringView &directory, const UnsafeStringView &subpath, bool isDirectory) {
            StringView path = Path::addComponent(directory, subpath);
            auto intermediate = isDirectory ? getDirectorySize(path) : getFileSize(path);
            if (!intermediate.first) {
                succeed = false;
                return false;
            }
            totalSize += intermediate.second;
            return true;
        })
        && succeed) {
        return { true, totalSize };
    };
    return { false, 0 };
}

bool FileManager::enumerateDirectory(
const UnsafeStringView &directory,
const std::function<bool(const UnsafeStringView &, const UnsafeStringView &, bool)> &enumeration)
{
    DIR *dir = opendir(directory.data());
    if (dir == NULL) {
        if (errno == ENOENT) {
            return true;
        }
        setThreadedError(directory);
        return false;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            if (!enumeration(directory, entry->d_name, entry->d_type == DT_DIR)) {
                break;
            }
        }
    }
    closedir(dir);

    return true;
}

bool FileManager::createFileHardLink(const UnsafeStringView &from, const UnsafeStringView &to)
{
    if (link(from.data(), to.data()) == 0) {
        return true;
    }
    setThreadedError(to);
    return false;
}

bool FileManager::removeFileHardLink(const UnsafeStringView &path)
{
    if (unlink(path.data()) == 0 || errno == ENOENT) {
        return true;
    }
    setThreadedError(path);
    return false;
}

bool FileManager::removeFile(const UnsafeStringView &file)
{
    if (remove(file.data()) == 0 || errno == ENOENT) {
        return true;
    }
    setThreadedError(file);
    return false;
}

bool FileManager::removeDirectory(const UnsafeStringView &directory)
{
    DIR *dir = opendir(directory.data());
    if (dir == NULL) {
        if (errno == ENOENT) {
            return true;
        }
        setThreadedError(directory);
        return false;
    }

    StringView path;
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

    if (remove(directory.data()) != 0) {
        setThreadedError(directory);
        return false;
    }
    return true;
}

bool FileManager::createDirectory(const UnsafeStringView &path)
{
    constexpr const int mask = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
    static_assert(mask == 0755, "");
    if (mkdir(path.data(), mask) == 0) {
        return true;
    }
    setThreadedError(path);
    return false;
}

std::pair<bool, Time> FileManager::getFileModifiedTime(const UnsafeStringView &path)
{
    struct stat result;
    if (stat(path.data(), &result) == 0) {
        return { true, Time(result.st_mtimespec) };
    }
    setThreadedError(path);
    return { false, {} };
}

std::pair<bool, Time> FileManager::getFileCreatedTime(const UnsafeStringView &path)
{
    struct stat result;
    if (stat(path.data(), &result) == 0) {
        return { true, Time(result.st_ctimespec) };
    }
    setThreadedError(path);
    return { false, Time() };
}

std::pair<bool, uint32_t> FileManager::getFileIdentifier(const UnsafeStringView &path)
{
    struct stat result;
    if (stat(path.data(), &result) == 0) {
        int size = sizeof(result.st_dev) + sizeof(result.st_ino);
        unsigned char buffer[size];
        memcpy(buffer, &result.st_dev, sizeof(result.st_dev));
        memcpy(buffer + sizeof(result.st_dev), &result.st_ino, sizeof(result.st_ino));
        return { true, UnsafeData(buffer, size).hash() };
    }
    return { false, 0 };
}

bool FileManager::createFile(const UnsafeStringView &path)
{
    constexpr const int mask = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    static_assert(mask == 0644, "");
    int fd = open(path.data(), O_CREAT | O_RDWR, mask);
    if (fd != -1) {
        close(fd);
        return true;
    }
    setThreadedError(path);
    return false;
}

#pragma mark - Combination
std::pair<bool, size_t> FileManager::getItemSize(const UnsafeStringView &path)
{
    struct stat temp;
    if (stat(path.data(), &temp) == 0) {
        if ((temp.st_mode & S_IFMT) == S_IFDIR) {
            return getDirectorySize(path);
        }
        return getFileSize(path);
    } else if (errno == ENOENT) {
        return { true, 0 };
    }
    setThreadedError(path);
    return { false, 0 };
}

std::pair<bool, bool> FileManager::fileExists(const UnsafeStringView &file)
{
    bool succeed, exists, isDirectory;
    std::tie(succeed, exists, isDirectory) = itemExists(file);
    return { succeed, exists && !isDirectory };
}

std::pair<bool, bool> FileManager::directoryExists(const UnsafeStringView &directory)
{
    bool succeed, exists, isDirectory;
    std::tie(succeed, exists, isDirectory) = itemExists(directory);
    return { succeed, exists && isDirectory };
}

std::pair<bool, size_t> FileManager::getItemsSize(const std::list<StringView> &paths)
{
    size_t size = 0;
    std::pair<bool, size_t> temp;
    for (const auto &path : paths) {
        temp = getItemSize(path);
        if (!temp.first) {
            return { false, 0 };
        }
        size += temp.second;
    }
    return { true, size };
}

bool FileManager::removeItem(const UnsafeStringView &path)
{
    struct stat temp;
    if (stat(path.data(), &temp) == 0) {
        if ((temp.st_mode & S_IFMT) == S_IFDIR) {
            return removeDirectory(path);
        }
        return removeFile(path);
    } else if (errno == ENOENT) {
        return true;
    }
    setThreadedError(path);
    return false;
}

bool FileManager::removeItems(const std::list<StringView> &items)
{
    for (const auto &item : items) {
        if (!removeItem(item)) {
            return false;
        }
    }
    return true;
}

bool FileManager::moveItems(const std::list<StringView> &paths, const UnsafeStringView &directory)
{
    if (!createDirectoryWithIntermediateDirectories(directory)) {
        return false;
    }
    std::list<std::pair<StringView, StringView>> pairedPaths;
    for (const auto &path : paths) {
        const StringView fileName = Path::getFileName(path);
        StringView newPath = Path::addComponent(directory, fileName);
        pairedPaths.push_back({ path, newPath });
    }
    return moveItems(pairedPaths);
}

bool FileManager::moveItems(const std::list<std::pair<StringView, StringView>> &pairedPaths)
{
    bool result = true;
    std::list<StringView> recovers;
    for (const auto &pairedPath : pairedPaths) {
        bool succeed, exists, isDirectory;
        std::tie(succeed, exists, isDirectory) = itemExists(pairedPath.first);
        if (!succeed) {
            break;
        }
        if (exists) {
            const StringView &newPath = pairedPath.second;
            if (!removeItem(newPath)) {
                result = false;
                break;
            }
            if (isDirectory) {
                if (!createDirectoryHardLink(pairedPath.first, newPath)) {
                    result = false;
                    break;
                }
            } else {
                if (!createFileHardLink(pairedPath.first, newPath)) {
                    result = false;
                    break;
                }
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
            removeItem(recover.data());
        }
    }
    return result;
}

bool FileManager::createDirectoryWithIntermediateDirectories(const UnsafeStringView &directory)
{
    auto ret = directoryExists(directory);
    if (!ret.first) {
        return false;
    }
    if (!ret.second) {
        return createDirectoryWithIntermediateDirectories(Path::getDirectoryName(directory))
               && createDirectory(directory);
    }
    return true;
}

bool FileManager::setFileProtectionCompleteUntilFirstUserAuthenticationIfNeeded(const UnsafeStringView &path)
{
    bool succeed;
    FileProtection fileProtection;
    std::tie(succeed, fileProtection) = getFileProtection(path);
    if (!succeed) {
        return false;
    }
    if (fileProtection != FileProtection::None
        && fileProtection != FileProtection::CompleteUntilFirstUserAuthentication) {
        return setFileProtection(path, FileProtection::CompleteUntilFirstUserAuthentication);
    }
    return true;
}

bool FileManager::createDirectoryHardLink(const UnsafeStringView &from,
                                          const UnsafeStringView &to)
{
    if (!createDirectory(to)) {
        return false;
    }
    bool succeed = true;
    if (enumerateDirectory(from,
                           [&succeed, &to](const UnsafeStringView &directory,
                                           const UnsafeStringView &subpath,
                                           bool isDirectory) -> bool {
                               StringView oldPath = Path::addComponent(directory, subpath);
                               StringView newPath = Path::addComponent(to, subpath);
                               if (isDirectory) {
                                   succeed = createDirectoryHardLink(oldPath, newPath);
                               } else {
                                   succeed = createFileHardLink(oldPath, newPath);
                               }
                               return succeed;
                           })
        && succeed) {
        return true;
    }
    removeItem(to);
    return false;
}

#pragma mark - Error
void FileManager::setThreadedError(const UnsafeStringView &path)
{
    Error error;
    error.level = Error::Level::Error;
    error.setSystemCode(errno, Error::Code::IOError);
    error.infos.insert_or_assign(StringView(ErrorStringKeyPath), path);
    Notifier::shared().notify(error);
    SharedThreadedErrorProne::setThreadedError(std::move(error));
}

void FileManager::setThreadedError(Error::Code codeIfUnresolved)
{
    Error error;
    error.level = Error::Level::Error;
    error.setSystemCode(errno, codeIfUnresolved);
    Notifier::shared().notify(error);
    SharedThreadedErrorProne::setThreadedError(std::move(error));
}

} //namespace WCDB
