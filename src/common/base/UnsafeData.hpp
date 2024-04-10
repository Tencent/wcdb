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

#include "Recyclable.hpp"
#include "SharedThreadedErrorProne.hpp"
#include "SysTypes.h"
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <type_traits>
#include <vector>

namespace WCDB {

template<typename T, typename Enable = void>
struct ColumnIsBLOBType;

class WCDB_API UnsafeData : protected SharedThreadedErrorProne {
#pragma mark - Initialize
public:
    template<typename T, typename Enable = void>
    struct Convertible : public std::false_type {
    public:
        static UnsafeData asUnsafeData(const T &);
    };

    template<typename T>
    struct Convertible<T, typename std::enable_if<ColumnIsBLOBType<T>::value>::type>
    : public std::true_type {
    public:
        static UnsafeData asUnsafeData(const T &t)
        {
            return ColumnIsBLOBType<T>::asUnderlyingType(t);
        }
    };

    template<typename T, typename Enable = typename std::enable_if<Convertible<T>::value>::type>
    UnsafeData(const T &t)
    {
        *this = Convertible<T>::asUnsafeData(t);
    }

    UnsafeData();
    UnsafeData(UnsafeData &&other);
    UnsafeData(const UnsafeData &other);
    virtual ~UnsafeData();

    UnsafeData &operator=(const UnsafeData &other);
    UnsafeData &operator=(UnsafeData &&other);
    bool operator==(const UnsafeData &other) const;
    bool operator!=(const UnsafeData &other) const;

    UnsafeData(unsigned char *buffer, size_t size);

    static const UnsafeData immutable(const unsigned char *buffer, size_t size);

protected:
    unsigned char *m_buffer;
    size_t m_size;

#pragma mark - SharedBuffer
protected:
    struct SharedData {
        SharedData(unsigned char *buffer, size_t size);
        SharedData(std::nullptr_t p);
        bool operator==(const std::nullptr_t &) const;
        bool operator!=(const std::nullptr_t &) const;
        unsigned char *buffer;
        size_t size;
    };
    typedef Recyclable<SharedData> SharedBuffer;
    static SharedBuffer
    makeSharedBuffer(unsigned char *buffer, size_t size, SharedBuffer::OnRecycled onRecycle);

    UnsafeData(unsigned char *buffer, size_t size, const SharedBuffer &sharedBuffer);
    SharedBuffer m_sharedBuffer;

#pragma mark - Subdata
public:
    UnsafeData subdata(size_t size) const;
    UnsafeData subdata(offset_t offset, size_t size) const;

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
