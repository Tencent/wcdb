//
// Created by sanhuazhang on 2019/05/02
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

#include "Data.hpp"
#include "StringView.hpp"
#include "Syntax.h"
#include "SyntaxForwardDeclaration.h"
#include <cstdint>
#include <type_traits>
#include <vector>

namespace WCDB {

typedef Syntax::ColumnType ColumnType;

#pragma mark - Column Type Info
//Null
template<ColumnType T = ColumnType::Null>
struct ColumnTypeInfo {
    static constexpr const bool isNull = true;
    static constexpr const bool isFloat = false;
    static constexpr const bool isInteger = false;
    static constexpr const bool isText = false;
    static constexpr const bool isBLOB = false;
    static constexpr const bool isBaseType = false;
    using UnderlyingType = std::nullptr_t;
    static constexpr const ColumnType type = ColumnType::Null;
};
//Float
template<>
struct ColumnTypeInfo<ColumnType::Float> {
    static constexpr const bool isNull = false;
    static constexpr const bool isFloat = true;
    static constexpr const bool isInteger = false;
    static constexpr const bool isText = false;
    static constexpr const bool isBLOB = false;
    static constexpr const bool isBaseType = true;
    using UnderlyingType = double;
    static constexpr const ColumnType type = ColumnType::Float;
};
//Integer
template<>
struct ColumnTypeInfo<ColumnType::Integer> {
    static constexpr const bool isNull = false;
    static constexpr const bool isFloat = false;
    static constexpr const bool isInteger = true;
    static constexpr const bool isText = false;
    static constexpr const bool isBLOB = false;
    static constexpr const bool isBaseType = true;
    using UnderlyingType = int64_t;
    static constexpr const ColumnType type = ColumnType::Integer;
};
//Text
template<>
struct ColumnTypeInfo<ColumnType::Text> {
    static constexpr const bool isNull = false;
    static constexpr const bool isFloat = false;
    static constexpr const bool isInteger64 = false;
    static constexpr const bool isText = true;
    static constexpr const bool isBLOB = false;
    static constexpr const bool isBaseType = true;
    using UnderlyingType = UnsafeStringView;
    static constexpr const ColumnType type = ColumnType::Text;
};
//BLOB
template<>
struct ColumnTypeInfo<ColumnType::BLOB> {
    static constexpr const bool isNull = false;
    static constexpr const bool isFloat = false;
    static constexpr const bool isInteger = false;
    static constexpr const bool isText = false;
    static constexpr const bool isBLOB = true;
    static constexpr const bool isBaseType = false;
    using UnderlyingType = UnsafeData;
    static constexpr const ColumnType type = ColumnType::BLOB;
};

#pragma mark - ColumnIsType
//Float
template<typename T, typename Enable = void>
struct ColumnIsFloatType : public std::false_type {
public:
    static ColumnTypeInfo<ColumnType::Float>::UnderlyingType asUnderlyingType(const T &);
};
//Integer
template<typename T, typename Enable = void>
struct ColumnIsIntegerType : public std::false_type {
public:
    static ColumnTypeInfo<ColumnType::Integer>::UnderlyingType
    asUnderlyingType(const T &);
};
//Text
template<typename T, typename Enable>
struct ColumnIsTextType : public std::false_type {
public:
    static ColumnTypeInfo<ColumnType::Text>::UnderlyingType asUnderlyingType(const T &);
};
//BLOB
template<typename T, typename Enable>
struct ColumnIsBLOBType : public std::false_type {
public:
    static ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType asUnderlyingType(const T &);
};

#pragma mark - ColumnInfo
template<typename T, typename Enable = void>
struct ColumnInfo {};
//Float
template<typename T>
struct ColumnInfo<T, typename std::enable_if<ColumnIsFloatType<T>::value>::type>
: public ColumnTypeInfo<ColumnType::Float> {};
//Integer
template<typename T>
struct ColumnInfo<T, typename std::enable_if<ColumnIsIntegerType<T>::value>::type>
: public ColumnTypeInfo<ColumnType::Integer> {};
//Text
template<typename T>
struct ColumnInfo<T, typename std::enable_if<ColumnIsTextType<T>::value>::type>
: public ColumnTypeInfo<ColumnType::Text> {};
//BLOB
template<typename T>
struct ColumnInfo<T, typename std::enable_if<ColumnIsBLOBType<T>::value>::type>
: public ColumnTypeInfo<ColumnType::BLOB> {};

#pragma mark - Builtin Type
//Float
template<typename T>
struct ColumnIsFloatType<T, typename std::enable_if<std::is_floating_point<T>::value>::type>
: public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Float>::UnderlyingType asUnderlyingType(const T &t)
    {
        return (ColumnTypeInfo<ColumnType::Float>::UnderlyingType) t;
    }
};

//Integer
template<typename T>
struct ColumnIsIntegerType<T, typename std::enable_if<(std::is_integral<T>::value || std::is_enum<T>::value)>::type>
: public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Integer>::UnderlyingType
    asUnderlyingType(const T &t)
    {
        return (ColumnTypeInfo<ColumnType::Integer>::UnderlyingType) t;
    }
};

//Text
template<>
struct WCDB_API ColumnIsTextType<const char *> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Text>::UnderlyingType
    asUnderlyingType(const char *text);
    static void
    setToUnderlyingType(const char *&target,
                        const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &t);
};

template<>
struct WCDB_API ColumnIsTextType<char *> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Text>::UnderlyingType
    asUnderlyingType(const char *text);
    static void
    setToUnderlyingType(char *&target,
                        const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &t);
};

template<int size>
struct ColumnIsTextType<const char[size]> : public std::true_type {
public:
    using ValueType = char[size];
    static ColumnTypeInfo<ColumnType::Text>::UnderlyingType
    asUnderlyingType(const ValueType &text)
    {
        return text;
    }
    static void setToUnderlyingType(const ValueType &,
                                    const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &)
    {
    }
};

template<int size>
struct ColumnIsTextType<char[size]> : public std::true_type {
public:
    using ValueType = char[size];
    static ColumnTypeInfo<ColumnType::Text>::UnderlyingType
    asUnderlyingType(const ValueType &text)
    {
        return text;
    }
    static void
    setToUnderlyingType(ValueType &target,
                        const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &t)
    {
        memcpy(target, t.data(), std::min(t.size(), (size_t) size));
        if (t.size() < size) {
            target[t.size()] = '\0';
        }
    }
};

template<>
struct WCDB_API ColumnIsTextType<std::string> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Text>::UnderlyingType
    asUnderlyingType(const std::string &text);
    static void
    setToUnderlyingType(std::string &target,
                        const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &t);
};

template<>
struct WCDB_API ColumnIsTextType<UnsafeStringView> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Text>::UnderlyingType
    asUnderlyingType(const UnsafeStringView &text);
    static void
    setToUnderlyingType(UnsafeStringView &target,
                        const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &t);
};

template<>
struct WCDB_API ColumnIsTextType<StringView> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Text>::UnderlyingType
    asUnderlyingType(const UnsafeStringView &text);
    static void
    setToUnderlyingType(StringView &target,
                        const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &t);
};

template<typename T>
struct ColumnIsTextType<T, std::enable_if_t<std::is_function<T>::value>>
: public std::false_type {};

//BLOB
template<>
struct WCDB_API ColumnIsBLOBType<std::vector<unsigned char>> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
    asUnderlyingType(const std::vector<unsigned char> &blob);
    static void
    setToUnderlyingType(std::vector<unsigned char> &target,
                        const ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType &t);
};

template<>
struct WCDB_API ColumnIsBLOBType<UnsafeData> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
    asUnderlyingType(const UnsafeData &blob);
    static void
    setToUnderlyingType(UnsafeData &target,
                        const ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType &t);
};

template<>
struct WCDB_API ColumnIsBLOBType<Data> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
    asUnderlyingType(const UnsafeData &blob);
    static void
    setToUnderlyingType(Data &target,
                        const ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType &t);
};

template<int size>
struct ColumnIsBLOBType<const unsigned char[size]> : public std::true_type {
public:
    using ValueType = unsigned char[size];
    static ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
    asUnderlyingType(const ValueType &data)
    {
        return UnsafeData(data, size);
    }

    static void setToUnderlyingType(const ValueType &,
                                    const ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType &)
    {
    }
};

template<int size>
struct ColumnIsBLOBType<unsigned char[size]> : public std::true_type {
public:
    using ValueType = unsigned char[size];
    static ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
    asUnderlyingType(const ValueType &data)
    {
        return UnsafeData((unsigned char *) data, size);
    }
    static void
    setToUnderlyingType(ValueType &target,
                        const ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType &t)
    {
        memcpy(target, t.buffer(), std::min(t.size(), (size_t) size));
        if (size > t.size()) {
            memset(target + t.size(), 0, size - t.size());
        }
    }
};

} //namespace WCDB
