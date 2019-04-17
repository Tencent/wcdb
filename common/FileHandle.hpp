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

#ifndef __WCDB_FILE_HANDLE_HPP
#define __WCDB_FILE_HANDLE_HPP

#include <WCDB/Data.hpp>
#include <WCDB/MappedData.hpp>
#include <WCDB/SharedThreadedErrorProne.hpp>
#include <WCDB/String.hpp>
#include <stdio.h>

namespace WCDB {

class FileHandle : public SharedThreadedErrorProne {
#pragma mark - Initialize
public:
    FileHandle(const String &path);
    FileHandle(FileHandle &&);
    virtual ~FileHandle();
    FileHandle &operator=(FileHandle &&);
    FileHandle() = delete;
    FileHandle(const FileHandle &) = delete;
    FileHandle &operator=(const FileHandle &) = delete;

    const String path;

protected:
    int m_fd;

#pragma mark - Basic
public:
    enum Mode {
        None = 0,
        OverWrite = 1,
        ReadOnly = 2,
    };
    bool open(Mode mode);
    bool isOpened() const;
    void close();
    ssize_t size();
    Data read(off_t offset, size_t size);
    bool write(off_t offset, const UnsafeData &unsafeData);
    void markErrorAsIgnorable(bool flag = true);

protected:
    bool m_errorIgnorable;
    int m_mode;

#pragma mark - Memory map
public:
    MappedData map(off_t offset, size_t size);

#pragma mark - Error
protected:
    //    void markAsMisuse(const char *message);
    void setThreadedError();
};

} //namespace WCDB

#endif /* __WCDB_FILE_HANDLE_HPP */
