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

#include <WCDB/Error.hpp>
#include <WCDB/SharedThreadedErrorProne.hpp>
#include <WCDB/String.hpp>
#include <WCDB/Time.hpp>
#include <functional>
#include <list>

namespace WCDB {

enum FileProtection {
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
    static std::tuple<bool, bool, bool> itemExists(const String &path);
    static std::pair<bool, size_t> getDirectorySize(const String &directory);
    static std::pair<bool, size_t> getFileSize(const String &file);
    static bool createFileHardLink(const String &from, const String &to);
    static bool removeFileHardLink(const String &path);
    static bool createDirectory(const String &path);
    static std::pair<bool, Time> getFileModifiedTime(const String &path);
    static std::pair<bool, Time> getFileCreatedTime(const String &path);
    static std::pair<bool, uint32_t> getFileIdentifier(const String &path);
    static bool createFile(const String &path);

    static bool
    enumerateDirectory(const String &directory,
                       const std::function<bool(const String &, const String &, bool)> &enumeration);

    static bool setFileProtection(const String &path, FileProtection fileProtection);
    static std::pair<bool, FileProtection> getFileProtection(const String &path);

protected:
    static bool removeFile(const String &file);
    static bool removeDirectory(const String &directory);

#pragma mark - Combination
public:
    static std::pair<bool, bool> fileExists(const String &file);
    static std::pair<bool, bool> directoryExists(const String &directory);
    static std::pair<bool, size_t> getItemSize(const String &path);
    static std::pair<bool, size_t> getItemsSize(const std::list<String> &paths);
    static bool removeItem(const String &path);
    static bool removeItems(const std::list<String> &paths);
    static bool moveItems(const std::list<String> &paths, const String &directory);
    static bool moveItems(const std::list<std::pair<String, String>> &pairedPaths);
    static bool createDirectoryWithIntermediateDirectories(const String &directory);
    static bool
    setFileProtectionCompleteUntilFirstUserAuthenticationIfNeeded(const String &path);
    static bool createDirectoryHardLink(const String &from, const String &to);

#pragma mark - Error
protected:
    static void setThreadedError(const String &path);
    static void setThreadedError(Error::Code codeIfUnresolved);
};

} //namespace WCDB
