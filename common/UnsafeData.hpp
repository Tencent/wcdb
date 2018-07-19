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

#ifndef UnsafeData_hpp
#define UnsafeData_hpp

#include <WCDB/SharedThreadedErrorProne.hpp>
#include <stdio.h>
#include <stdlib.h>

namespace WCDB {

class UnsafeData : protected SharedThreadedErrorProne {
#pragma mark - Initialize
public:
    UnsafeData();
    UnsafeData(unsigned char *buffer, size_t size);

    static const UnsafeData immutable(const unsigned char *buffer, size_t size);

protected:
    size_t m_size;
    unsigned char *m_buffer;

#pragma mark - Subdata
public:
    UnsafeData subdata(size_t size) const;
    UnsafeData subdata(off_t offset, size_t size) const;

#pragma mark - Empty
public:
    static const UnsafeData &emptyData();
    bool empty() const;

protected:
    static unsigned char *emptyBuffer();

#pragma mark - Basic
public:
    size_t size() const;
    uint32_t hash() const;

    //Nota that buffer will never be null.
    const unsigned char *buffer() const;
    unsigned char *buffer();
};

}; // namespace WCDB

#endif /* UnsafeData_hpp */
