//
// Created by sanhuazhang on 2018/06/27
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

#include "CrossPlatform.h"
#include "Data.hpp"
#include "MappedData.hpp"
#include "SharedThreadedErrorProne.hpp"
#include "StringView.hpp"
#include <stdio.h>

namespace WCDB {

class FileHandle : public SharedThreadedErrorProne {
#pragma mark - Initialize
public:
    FileHandle(const UnsafeStringView &path);
    FileHandle(FileHandle &&);
    virtual ~FileHandle() override;
    FileHandle &operator=(FileHandle &&);
    FileHandle() = delete;
    FileHandle(const FileHandle &) = delete;
    FileHandle &operator=(const FileHandle &) = delete;

    const StringView path;

protected:
    int m_fd;
#ifdef _WIN32
    void *m_mapHandle;
#endif

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
    Data read(size_t size);
    bool write(const UnsafeData &unsafeData);

protected:
    int m_mode;
    bool m_errorIgnorable;
    ssize_t m_fileSize;

#pragma mark - Memory map
public:
    MappedData map(offset_t offset, size_t size, SharedHighWater highWater = nullptr);
    UnsafeData mapOrReadAllData();

protected:
    static const size_t &memoryPageSize();

#pragma mark - Error
public:
    void markErrorAsIgnorable(bool flag = true);

protected:
    //    void markAsMisuse(const char *message);
    void setThreadedError(const UnsafeStringView &message = "");
};

} //namespace WCDB
