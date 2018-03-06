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

#include <WCDB/File.hpp>
#include <WCDB/Path.hpp>
#include <WCDB/String.hpp>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

namespace WCDB {

namespace File {

struct ErrorInfo {
    std::string path;
    Error::Operation operation;
};
typedef struct ErrorInfo ErrorInfo;

static ThreadLocal<ErrorInfo> s_errorInfos;

static void setThreadedErrorInfo(Error::Operation operation,
                                 const std::string &path)
{
    ErrorInfo *errorInfo = s_errorInfos.get();
    errorInfo->path = path;
    errorInfo->operation = operation;
}

static ErrorInfo *getThreadedErrorInfo()
{
    return s_errorInfos.get();
}

#pragma mark - Basic
std::pair<bool, size_t> getFileSize(const std::string &path)
{
    struct stat temp;
    if (lstat(path.c_str(), &temp) == 0) {
        return {true, (size_t) temp.st_size};
    } else if (errno == ENOENT) {
        return {true, 0};
    }
    setThreadedErrorInfo(Error::Operation::SystemCallLstat, path);
    return {false, 0};
}

std::pair<bool, bool> isExists(const std::string &path)
{
    if (access(path.c_str(), F_OK) == 0) {
        return {true, true};
    } else if (errno == ENOENT) {
        return {true, false};
    }
    setThreadedErrorInfo(Error::Operation::SystemCallAccess, path);
    return {false, false};
}

bool createHardLink(const std::string &from, const std::string &to)
{
    if (link(from.c_str(), to.c_str()) == 0) {
        return true;
    }
    setThreadedErrorInfo(Error::Operation::SystemCallLink, to);
    return false;
}

bool removeHardLink(const std::string &path)
{
    if (unlink(path.c_str()) == 0 || errno == ENOENT) {
        return true;
    }
    setThreadedErrorInfo(Error::Operation::SystemCallUnlink, path);
    return false;
}

bool removeFile(const std::string &path)
{
    if (remove(path.c_str()) == 0 || errno == ENOENT) {
        return true;
    }
    setThreadedErrorInfo(Error::Operation::SystemCallRemove, path);
    return false;
}

bool createDirectory(const std::string &path)
{
    if (mkdir(path.c_str(), 0755) == 0) {
        return true;
    }
    setThreadedErrorInfo(Error::Operation::SystemCallMkdir, path);
    return false;
}

#pragma mark - Combination
std::pair<bool, size_t> getFilesSize(const std::list<std::string> &paths)
{
    size_t size = 0;
    std::pair<bool, size_t> ret;
    for (const auto &path : paths) {
        ret = File::getFileSize(path);
        if (ret.first) {
            size += ret.second;
        } else {
            return {false, 0};
        }
    }
    return {true, size};
}

bool moveFiles(const std::list<std::string> &paths,
               const std::string &directory)
{
    if (!createDirectoryWithIntermediateDirectories(directory)) {
        return false;
    }
    bool result = true;
    std::list<std::string> recovers;
    for (const auto &path : paths) {
        std::pair<bool, bool> ret = File::isExists(path);
        if (!ret.first) {
            break;
        }
        if (ret.second) {
            const std::string fileName = Path::getFileName(path);
            std::string newPath = Path::addComponent(directory, fileName);
            if (!File::removeFile(newPath) ||
                !File::createHardLink(path, newPath)) {
                result = false;
                break;
            }
            recovers.push_back(newPath);
        }
    }
    if (result) {
        File::removeFiles(paths);
    } else {
        for (const auto &recover : recovers) {
            File::removeHardLink(recover.c_str());
        }
    }
    return result;
}

bool removeFiles(const std::list<std::string> &paths)
{
    for (const auto &path : paths) {
        if (!File::removeFile(path)) {
            return false;
        }
    }
    return true;
}

bool createDirectoryWithIntermediateDirectories(const std::string &path)
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

Error getError()
{
    Error error;
    error.setError(Error::Type::SystemCall, errno);
    ErrorInfo *errorInfo = getThreadedErrorInfo();
    error.setMessage(strerror(errno));
    error.setOperation(errorInfo->operation);
    error.setPath(errorInfo->path);
    return error;
}

} //namespace File

} //namespace WCDB
