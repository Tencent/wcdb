//
// Created by sanhuazhang on 2018/07/19
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

#include <WCDB/SharedThreadedErrorProne.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <type_traits>
#include <vector>

namespace WCDB {

class UnsafeData : protected SharedThreadedErrorProne {
#pragma mark - Initialize
public:
    template<typename T, typename Enable = void>
    struct Convertible : public std::false_type {
    public:
        static UnsafeData asUnsafeData(const T &);
    };

    template<typename T, typename Enable = typename std::enable_if<Convertible<T>::value>::type>
    UnsafeData(const T &t) : UnsafeData(Convertible<T>::asUnsafeData(t))
    {
    }

    UnsafeData();
    UnsafeData(UnsafeData &&other);
    UnsafeData(const UnsafeData &other);
    virtual ~UnsafeData();

    UnsafeData &operator=(const UnsafeData &other);
    UnsafeData &operator=(UnsafeData &&other);

    UnsafeData(unsigned char *buffer, size_t size);

    static const UnsafeData immutable(const unsigned char *buffer, size_t size);

protected:
    UnsafeData(unsigned char *buffer,
               size_t size,
               const std::shared_ptr<std::vector<unsigned char>> &sharedBuffer);
    unsigned char *m_buffer;
    size_t m_size;
    std::shared_ptr<std::vector<unsigned char>> m_sharedBuffer;

#pragma mark - Subdata
public:
    UnsafeData subdata(size_t size) const;
    UnsafeData subdata(off_t offset, size_t size) const;

#pragma mark - Empty
public:
    static const UnsafeData &null();
    bool empty() const;

private:
    static unsigned char *emptyBuffer();

#pragma mark - Basic
public:
    size_t size() const;
    uint32_t hash() const;

    //Nota that buffer will never be null.
    const unsigned char *buffer() const;
    unsigned char *buffer();

    bool hasSharedBuffer() const;
};

}; // namespace WCDB
