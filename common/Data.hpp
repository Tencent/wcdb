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

#ifndef Data_hpp
#define Data_hpp

#include <memory>
#include <stdio.h>
#include <vector>

namespace WCDB {

class Data {
#pragma mark - Initialize
public:
    Data();

protected:
    Data(const std::shared_ptr<std::vector<unsigned char>> &sharedBuffer,
         unsigned char *buffer,
         size_t size);
    size_t m_size;
    unsigned char *m_buffer;

#pragma mark - Shared
public:
    static Data data(unsigned char *buffer, size_t size);
    static const Data data(const unsigned char *buffer, size_t size);
    static Data data(size_t size);

    bool makeShared();
    bool isShared() const;
    Data copy() const;

    bool resize(size_t size);

protected:
    std::shared_ptr<std::vector<unsigned char>> m_sharedBuffer;

#pragma mark - No Shared
public:
    static Data noCopyData(unsigned char *buffer, size_t size);
    static const Data noCopyData(const unsigned char *buffer, size_t size);

#pragma mark - Subdata
public:
    Data subdata(size_t size) const;
    Data subdata(off_t offset, size_t size) const;

#pragma mark - Empty
public:
    bool empty() const;
    static Data emptyData();
    static unsigned char *emptyBuffer();

#pragma mark - Basic
public:
    size_t size() const;

    //Nota that buffer will never be null.
    const unsigned char *buffer() const;
    unsigned char *buffer();
};

} //namespace WCDB

#endif /* Data_hpp */
