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

#ifndef FileHandle_hpp
#define FileHandle_hpp

#include <WCDB/Error.hpp>
#include <stdio.h>

namespace WCDB {

class FileHandle {
#pragma mark - Initialize
public:
    FileHandle(const std::string &path);
    FileHandle() = delete;
    FileHandle(const FileHandle &) = delete;
    FileHandle &operator=(const FileHandle &) = delete;

    const std::string path;

protected:
    int m_fd;

#pragma mark - Basic
public:
    bool open();
    void close();
    ssize_t read(unsigned char *data, off_t offset, size_t size);

#pragma mark - Error
public:
    class Error : public WCDB::Error {
    public:
        //TODO
        static constexpr const int type = 7;
        std::string path;

        std::string getDescription() const override;

        int getType() const override;
    };

    const Error &getError() const;

protected:
    Error m_error;
};

} //namespace WCDB

#endif /* FileHandle_hpp */
