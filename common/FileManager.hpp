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

#ifndef FileManager_hpp
#define FileManager_hpp

#include <WCDB/Error.hpp>
#include <WCDB/ThreadLocal.hpp>
#include <list>
#include <string>

#pragma GCC visibility push(hidden)

namespace WCDB {

class FileManager {
#pragma mark - Initialize
public:
    static FileManager *shared();

protected:
    FileManager();
    FileManager(const FileManager &) = delete;
    FileManager &operator=(const FileManager &) = delete;

#pragma mark - Basic
public:
    std::pair<bool, bool> isExists(const std::string &path);
    std::pair<bool, size_t> getFileSize(const std::string &path);
    bool createHardLink(const std::string &from, const std::string &to);
    bool removeHardLink(const std::string &path);
    bool removeFile(const std::string &path);
    bool createDirectory(const std::string &path);

#pragma mark - Combination
public:
    std::pair<bool, size_t> getFilesSize(const std::list<std::string> &paths);
    bool removeFiles(const std::list<std::string> &paths);
    bool moveFiles(const std::list<std::string> &paths,
                   const std::string &directory);
    bool createDirectoryWithIntermediateDirectories(const std::string &path);

#pragma mark - Error
public:
    enum Operation {
        NotSet = 0,
        Lstat = 1,
        Access = 2,
        Link = 3,
        Unlink = 4,
        Remove = 5,
        Mkdir = 6,
    };

    const Error &getError();

protected:
    void error(Operation operation, const std::string &path);
    ThreadLocal<Error> m_errors;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* FileManager_hpp */
