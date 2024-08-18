//
// Created by sanhuazhang on 2018/05/19
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

#include "FileManager.hpp"
#include "Assertion.hpp"
#include "CoreConst.h"
#include "Notifier.hpp"
#include "Path.hpp"
#include "StringView.hpp"
#include "UnsafeData.hpp"
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#ifndef _WIN32
#include <dirent.h>
#else
#include <direct.h>
#include <windows.h>
#endif
#include "CrossPlatform.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

namespace WCDB {

#pragma mark - Basic
Optional<std::pair<bool, bool>> FileManager::itemExists(const UnsafeStringView &path)
{
    StatType s;
    if (StatFunc(GetPathString(path), &s) == 0) {
        if ((s.st_mode & S_IFMT) == S_IFDIR) {
            return std::make_pair(true, true);
        } else {
            return std::make_pair(true, false);
        }
    } else if (errno == ENOENT) {
        return std::make_pair(false, false);
    }
    setThreadedError(path);
    return NullOpt;
}

Optional<size_t> FileManager::getFileSize(const UnsafeStringView &file)
{
    StatType temp;
    if (StatFunc(GetPathString(file), &temp) == 0) {
        return (size_t) temp.st_size;
    } else if (errno == ENOENT) {
        return 0;
    }
    setThreadedError(file);
    return NullOpt;
}

Optional<size_t> FileManager::getDirectorySize(const UnsafeStringView &directory)
{
    Optional<size_t> totalSize = 0;
    if (!enumerateDirectory(
        directory, [&totalSize](const UnsafeStringView &dir, const UnsafeStringView &subpath, bool isDirectory) {
            StringView path = Path::addComponent(dir, subpath);
            auto size = isDirectory ? getDirectorySize(path) : getFileSize(path);
            if (!size.succeed()) {
                totalSize = NullOpt;
                return false;
            }
            totalSize.value() += size.value();
            return true;
        })) {
        totalSize = NullOpt;
    };
    return totalSize;
}

bool FileManager::enumerateDirectory(
const UnsafeStringView &directory,
const std::function<bool(const UnsafeStringView &, const UnsafeStringView &, bool)> &enumeration)
{
#ifndef _WIN32
    DIR *dir = opendir(GetPathString(directory));
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
#else
    StringView searchDir = Path::addComponent(directory, "*.*");
    uint32_t status = 0;
    WIN32_FIND_DATAW findFileData;
    HANDLE findHandle = FindFirstFileW(GetPathString(searchDir), &findFileData);
    if (findHandle == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_PATH_NOT_FOUND) {
            return true;
        }
        setThreadedWinError(directory);
        return false;
    }
    bool isBreak = false;
    do {
        StringView subFileName = StringView::createFromWString(findFileData.cFileName);
        if (strcmp(subFileName.data(), ".") != 0 && strcmp(subFileName.data(), "..") != 0) {
            if (!enumeration(directory,
                             subFileName,
                             (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)) {
                isBreak = true;
                break;
            }
        }
    } while (FindNextFileW(findHandle, &findFileData) != 0);
    if (!isBreak && GetLastError() != ERROR_NO_MORE_FILES) {
        setThreadedWinError(directory);
        bool ret = FindClose(findHandle);
        WCTAssert(ret);
        return false;
    }
    bool ret = FindClose(findHandle);
    WCTAssert(ret);
#endif

    return true;
}
#ifndef __ANDROID__
bool FileManager::createFileHardLink(const UnsafeStringView &from, const UnsafeStringView &to)
{
#ifndef _WIN32
    if (link(GetPathString(from), GetPathString(to)) == 0) {
        return true;
    }
    setThreadedError(to);
    return false;
#else
    if (CreateHardLinkW(GetPathString(to), GetPathString(from), NULL)) {
        return true;
    }
    setThreadedWinError(to);
    return false;
#endif
}

bool FileManager::removeFileHardLink(const UnsafeStringView &path)
{
    if (wcdb_unlink(GetPathString(path)) == 0 || errno == ENOENT) {
        return true;
    }
    setThreadedError(path);
    return false;
}
#endif

bool FileManager::removeFile(const UnsafeStringView &file)
{
    if (wcdb_remove(GetPathString(file)) == 0 || errno == ENOENT) {
        return true;
    }
    setThreadedError(file);
    return false;
}

bool FileManager::removeDirectory(const UnsafeStringView &directory)
{
#ifndef _WIN32
    DIR *dir = opendir(GetPathString(directory));
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

    if (wcdb_remove(GetPathString(directory)) != 0) {
        setThreadedError(directory);
        return false;
    }
    return true;
#else
    StringView searchDir = Path::addComponent(directory, "*.*");
    uint32_t status = 0;
    WIN32_FIND_DATAW findFileData;
    HANDLE findHandle = FindFirstFileW(GetPathString(searchDir), &findFileData);
    if (findHandle == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_PATH_NOT_FOUND) {
            return true;
        }
        setThreadedWinError(directory);
        return false;
    }
    bool result;
    do {
        StringView subFileName = StringView::createFromWString(findFileData.cFileName);
        if (strcmp(subFileName.data(), ".") != 0 && strcmp(subFileName.data(), "..") != 0) {
            StringView currentPath = Path::addComponent(directory, subFileName);
            if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
                result = removeDirectory(currentPath);
            } else {
                result = removeFile(currentPath);
            }
            if (!result) {
                setThreadedWinError(directory);
                bool ret = FindClose(findHandle);
                WCTAssert(ret);
                return false;
            }
        }
    } while (FindNextFileW(findHandle, &findFileData) != 0);
    if (GetLastError() != ERROR_NO_MORE_FILES) {
        setThreadedWinError(directory);
        bool ret = FindClose(findHandle);
        WCTAssert(ret);
        return false;
    }
    bool ret = FindClose(findHandle);
    WCTAssert(ret);
    if (!RemoveDirectoryW(GetPathString(directory))) {
        setThreadedWinError(directory);
        return false;
    }
    return true;
#endif
}

bool FileManager::createDirectory(const UnsafeStringView &path)
{
    if (wcdb_mkdir(GetPathString(path), DirFullAccess) == 0) {
        return true;
    }
    setThreadedError(path);
    return false;
}

Optional<Time> FileManager::getFileModifiedTime(const UnsafeStringView &path)
{
    StatType result;
    if (StatFunc(GetPathString(path), &result) == 0) {
        return Time(result.st_mtimespec);
    }
    setThreadedError(path);
    return NullOpt;
}

Optional<uint32_t> FileManager::getFileIdentifier(const UnsafeStringView &path)
{
#ifndef _WIN32
    StatType result;
    if (StatFunc(GetPathString(path), &result) == 0) {
        constexpr int size = sizeof(result.st_dev) + sizeof(result.st_ino);
        unsigned char buffer[size];
        memcpy(buffer, &result.st_dev, sizeof(result.st_dev));
        memcpy(buffer + sizeof(result.st_dev), &result.st_ino, sizeof(result.st_ino));
        return UnsafeData(buffer, size).hash();
    }
    return NullOpt;
#else
    HANDLE hFile = CreateFileW(GetPathString(path),
                               GENERIC_READ,
                               FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        setThreadedWinError(path);
        return NullOpt;
    }
    FILE_ID_INFO fileIdInfo;
    if (!GetFileInformationByHandleEx(hFile, FileIdInfo, &fileIdInfo, sizeof(fileIdInfo))) {
        setThreadedWinError(path);
        CloseHandle(hFile);
        return NullOpt;
    }
    CloseHandle(hFile);

    // Copy FileID and VolumeSerialNumber to a buffer
    constexpr size_t size
    = sizeof(fileIdInfo.FileId) + sizeof(fileIdInfo.VolumeSerialNumber);
    unsigned char buffer[size];
    memcpy(buffer, &fileIdInfo.FileId, sizeof(fileIdInfo.FileId));
    memcpy(buffer + sizeof(fileIdInfo.FileId),
           &fileIdInfo.VolumeSerialNumber,
           sizeof(fileIdInfo.VolumeSerialNumber));
    return UnsafeData(buffer, size).hash();
#endif
}

bool FileManager::createFile(const UnsafeStringView &path)
{
    int fd = wcdb_open(GetPathString(path), O_CREAT | O_RDWR | O_BINARY, FileFullAccess);
    if (fd != -1) {
        close(fd);
        return true;
    }
    setThreadedError(path);
    return false;
}

#pragma mark - Combination
Optional<size_t> FileManager::getItemSize(const UnsafeStringView &path)
{
    StatType temp;
    if (StatFunc(GetPathString(path), &temp) == 0) {
        if ((temp.st_mode & S_IFMT) == S_IFDIR) {
            return getDirectorySize(path);
        }
        return getFileSize(path);
    } else if (errno == ENOENT) {
        return 0;
    }
    setThreadedError(path);
    return NullOpt;
}

Optional<bool> FileManager::fileExists(const UnsafeStringView &file)
{
    auto result = itemExists(file);
    if (result.succeed()) {
        bool exists, isDirectory;
        std::tie(exists, isDirectory) = result.value();
        return exists && !isDirectory;
    }
    return NullOpt;
}

Optional<bool> FileManager::fileExistsAndNotEmpty(const UnsafeStringView &file)
{
    StatType s;
    if (StatFunc(GetPathString(file), &s) == 0) {
        if ((s.st_mode & S_IFMT) == S_IFDIR) {
            return false;
        } else {
            return s.st_size > 0;
        }
    } else if (errno == ENOENT) {
        return false;
    }
    setThreadedError(file);
    return NullOpt;
}

Optional<bool> FileManager::directoryExists(const UnsafeStringView &directory)
{
    auto result = itemExists(directory);
    if (result.succeed()) {
        bool exists, isDirectory;
        std::tie(exists, isDirectory) = result.value();
        return exists && isDirectory;
    }
    return NullOpt;
}

Optional<size_t> FileManager::getItemsSize(const std::list<StringView> &paths)
{
    size_t size = 0;
    Optional<size_t> temp;
    for (const auto &path : paths) {
        temp = getItemSize(path);
        if (!temp.succeed()) {
            return NullOpt;
        }
        size += temp.value();
    }
    return size;
}

bool FileManager::removeItem(const UnsafeStringView &path)
{
    StatType temp;
    if (StatFunc(GetPathString(path), &temp) == 0) {
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
        auto optionalExists = itemExists(pairedPath.first);
        if (!optionalExists.succeed()) {
            break;
        }
        bool exists, isDirectory;
        std::tie(exists, isDirectory) = optionalExists.value();
        if (exists) {
            const StringView &newPath = pairedPath.second;
            if (!removeItem(newPath)) {
                result = false;
                break;
            }
            if (isDirectory) {
#ifdef __ANDROID__
                if (::rename(pairedPath.first.data(), newPath.data()) != 0) {
#else
                if (!createDirectoryHardLink(pairedPath.first, newPath)) {
#endif
                    result = false;
                    break;
                }
            } else {
#ifdef __ANDROID__
                if (::rename(pairedPath.first.data(), newPath.data()) != 0) {
#else
                if (!createFileHardLink(pairedPath.first, newPath)) {
#endif
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
    if (directory.length() == 0) {
        Error error(Error::Code::IOError, Error::Level::Error, "Empty directory");
        Notifier::shared().notify(error);
        SharedThreadedErrorProne::setThreadedError(std::move(error));
        return false;
    }
    auto exists = directoryExists(directory);
    if (!exists.succeed()) {
        return false;
    }
    if (!exists.value()) {
        return createDirectoryWithIntermediateDirectories(Path::getDirectory(directory))
               && createDirectory(directory);
    }
    return true;
}

bool FileManager::setFileProtectionCompleteUntilFirstUserAuthenticationIfNeeded(const UnsafeStringView &path)
{
    if (!path.empty()) {
        auto optionalFileProtection = getFileProtection(path);
        if (!optionalFileProtection.succeed()) {
            return false;
        }
        FileProtection fileProtection = optionalFileProtection.value();
        if (fileProtection != FileProtection::None
            && fileProtection != FileProtection::CompleteUntilFirstUserAuthentication) {
            return setFileProtection(path, FileProtection::CompleteUntilFirstUserAuthentication);
        }
    }
    return true;
}

#ifndef __ANDROID__
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
#endif

#pragma mark - Error
void FileManager::setThreadedError(const UnsafeStringView &path)
{
    Error error;
    error.level = Error::Level::Error;
    error.setSystemCode(errno, Error::Code::IOError);
    error.infos.insert_or_assign(ErrorStringKeyAssociatePath, path);
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

#ifdef _WIN32
void FileManager::setThreadedWinError(const UnsafeStringView &path)
{
    Error error;
    error.level = Error::Level::Error;
    error.setWinSystemCode(GetLastError(), Error::Code::IOError);
    error.infos.insert_or_assign(ErrorStringKeyAssociatePath, path);
    Notifier::shared().notify(error);
    SharedThreadedErrorProne::setThreadedError(std::move(error));
}

void FileManager::setThreadedWinError(Error::Code codeIfUnresolved)
{
    Error error;
    error.level = Error::Level::Error;
    error.setWinSystemCode(GetLastError(), codeIfUnresolved);
    Notifier::shared().notify(error);
    SharedThreadedErrorProne::setThreadedError(std::move(error));
}
#endif

} //namespace WCDB
