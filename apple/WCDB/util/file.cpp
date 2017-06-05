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

#include <WCDB/file.hpp>
#include <WCDB/path.hpp>
#include <WCDB/utility.hpp>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

namespace WCDB {

namespace File {

//Basic
size_t getFileSize(const std::string &path, Error &error)
{
    struct stat temp;
    if (lstat(path.c_str(), &temp) == 0) {
        error.reset();
        return (size_t) temp.st_size;
    } else if (errno == ENOENT) {
        error.reset();
        return 0;
    }
    Error::ReportSystemCall(WCDB::Error::SystemCallOperation::Lstat, path,
                            errno, strerror(errno), &error);
    return 0;
}

bool isExists(const std::string &path, Error &error)
{
    if (access(path.c_str(), F_OK) == 0) {
        error.reset();
        return true;
    } else if (errno == ENOENT) {
        error.reset();
        return false;
    }
    Error::ReportSystemCall(Error::SystemCallOperation::Access, path, errno,
                            strerror(errno), &error);
    return false;
}

bool createHardLink(const std::string &from,
                    const std::string &to,
                    Error &error)
{
    if (link(from.c_str(), to.c_str()) == 0) {
        error.reset();
        return true;
    }
    Error::ReportSystemCall(Error::SystemCallOperation::Link, to, errno,
                            strerror(errno), &error);
    return false;
}

bool removeHardLink(const std::string &path, Error &error)
{
    if (unlink(path.c_str()) == 0 || errno == ENOENT) {
        return true;
    }
    Error::ReportSystemCall(Error::SystemCallOperation::Unlink, path, errno,
                            strerror(errno), &error);
    return false;
}

bool removeFile(const std::string &path, Error &error)
{
    if (remove(path.c_str()) == 0 || errno == ENOENT) {
        return true;
    }
    Error::ReportSystemCall(Error::SystemCallOperation::Remove, path, errno,
                            strerror(errno), &error);
    return false;
}

bool createDirectory(const std::string &path, Error &error)
{
    if (mkdir(path.c_str(), 0755) == 0) {
        return true;
    }
    Error::ReportSystemCall(WCDB::Error::SystemCallOperation::Mkdir, path,
                            errno, strerror(errno), &error);
    return false;
}

//Combination
size_t getFilesSize(const std::list<std::string> &paths, Error &error)
{
    size_t size = 0;
    for (const auto &path : paths) {
        size += File::getFileSize(path, error);
        if (!error.isOK()) {
            return 0;
        }
    }
    return size;
}

bool moveFiles(const std::list<std::string> &paths,
               const std::string &directory,
               Error &error)
{
    bool result = true;
    createDirectoryWithIntermediateDirectories(directory, error);
    if (!error.isOK()) {
        return false;
    }
    std::list<std::string> recovers;
    for (const auto &path : paths) {
        if (File::isExists(path, error)) {
            const std::string fileName = Path::getFileName(path);
            std::string newPath = Path::addComponent(directory, fileName);
            if (!File::removeFile(newPath, error) ||
                !File::createHardLink(path, newPath, error)) {
                result = false;
                break;
            }
            recovers.push_back(newPath);
        }
    }
    Error innerError; //This error do not need to be passed to the outside
    if (result) {
        File::removeFiles(paths, innerError);
    } else {
        for (const auto &recover : recovers) {
            File::removeHardLink(recover.c_str(), innerError);
        }
    }
    return result;
}

bool removeFiles(const std::list<std::string> &paths, Error &error)
{
    for (const auto &path : paths) {
        if (!File::removeFile(path, error)) {
            return false;
        }
    }
    return true;
}

bool createDirectoryWithIntermediateDirectories(const std::string &path,
                                                Error &error)
{
    if (!isExists(path, error) && (!error.isOK() ||
                                   !createDirectoryWithIntermediateDirectories(
                                       Path::getBaseName(path), error) ||
                                   !createDirectory(path, error))) {
        return false;
    }
    return true;
}

} //namespace File

} //namespace WCDB
