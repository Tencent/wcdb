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

#pragma once

#include "SharedThreadedErrorProne.hpp"
#include "StringView.hpp"
#include "Time.hpp"
#include "WCDBError.hpp"
#include "WCDBOptional.hpp"
#include <functional>
#include <list>

namespace WCDB {

enum class FileProtection {
    None = 0,
    Complete,
    CompleteUnlessOpen,
    CompleteUntilFirstUserAuthentication,
};

class FileManager final : public SharedThreadedErrorProne {
public:
    FileManager() = delete;
    FileManager(const FileManager &) = delete;
    FileManager &operator=(const FileManager &) = delete;

#pragma mark - Basic
public:
    // exists, directory?
    static Optional<std::pair<bool, bool>> itemExists(const UnsafeStringView &path);
    static Optional<size_t> getDirectorySize(const UnsafeStringView &directory);
    static Optional<size_t> getFileSize(const UnsafeStringView &file);
#ifndef __ANDROID__
    static bool
    createFileHardLink(const UnsafeStringView &from, const UnsafeStringView &to);
    static bool removeFileHardLink(const UnsafeStringView &path);
#endif
    static bool createDirectory(const UnsafeStringView &path);
    static Optional<Time> getFileModifiedTime(const UnsafeStringView &path);
    static Optional<uint32_t> getFileIdentifier(const UnsafeStringView &path);
    static bool createFile(const UnsafeStringView &path);

    static bool enumerateDirectory(
    const UnsafeStringView &directory,
    const std::function<bool /* stop */ (const UnsafeStringView &directory, const UnsafeStringView &subpath, bool isDirectory)>
    &enumeration);

    static bool setFileProtection(const UnsafeStringView &path, FileProtection fileProtection);
    static StringView getTemporaryDirectory();
    static Optional<FileProtection> getFileProtection(const UnsafeStringView &path);

protected:
    static bool removeFile(const UnsafeStringView &file);
    static bool removeDirectory(const UnsafeStringView &directory);

#pragma mark - Combination
public:
    static Optional<bool> fileExists(const UnsafeStringView &file);
    static Optional<bool> fileExistsAndNotEmpty(const UnsafeStringView &file);
    static Optional<bool> directoryExists(const UnsafeStringView &directory);
    static Optional<size_t> getItemSize(const UnsafeStringView &path);
    static Optional<size_t> getItemsSize(const std::list<StringView> &paths);
    static bool removeItem(const UnsafeStringView &path);
    static bool removeItems(const std::list<StringView> &paths);
    static bool
    moveItems(const std::list<StringView> &paths, const UnsafeStringView &directory);
    static bool moveItems(const std::list<std::pair<StringView, StringView>> &pairedPaths);
    static bool
    createDirectoryWithIntermediateDirectories(const UnsafeStringView &directory);
    static bool
    setFileProtectionCompleteUntilFirstUserAuthenticationIfNeeded(const UnsafeStringView &path);
#ifndef __ANDROID__
    static bool
    createDirectoryHardLink(const UnsafeStringView &from, const UnsafeStringView &to);
#endif

#pragma mark - Error
protected:
    static void setThreadedError(const UnsafeStringView &path);
    static void setThreadedError(Error::Code codeIfUnresolved);
#ifdef _WIN32
    static void setThreadedWinError(const UnsafeStringView &path);
    static void setThreadedWinError(Error::Code codeIfUnresolved);
#endif
};

} //namespace WCDB
