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
//NULL
template<typename T, typename Enable = void>
struct ColumnIsNullType : public std::false_type {
public:
    static ColumnTypeInfo<ColumnType::Null>::UnderlyingType asUnderlyingType(const T &);
};
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
template<typename T, typename Enable = void>
struct ColumnIsTextType : public std::false_type {
public:
    static ColumnTypeInfo<ColumnType::Text>::UnderlyingType asUnderlyingType(const T &);
};
//BLOB
template<typename T, typename Enable = void>
struct ColumnIsBLOBType : public std::false_type {
public:
    static ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType asUnderlyingType(const T &);
};

#pragma mark - ColumnInfo
template<typename T, typename Enable = void>
struct ColumnInfo {
};
//NULL
template<typename T>
struct ColumnInfo<T, typename std::enable_if<ColumnIsNullType<T>::value>::type>
: public ColumnTypeInfo<ColumnType::Null> {
};
//Float
template<typename T>
struct ColumnInfo<T, typename std::enable_if<ColumnIsFloatType<T>::value>::type>
: public ColumnTypeInfo<ColumnType::Float> {
};
//Integer
template<typename T>
struct ColumnInfo<T, typename std::enable_if<ColumnIsIntegerType<T>::value>::type>
: public ColumnTypeInfo<ColumnType::Integer> {
};
//Text
template<typename T>
struct ColumnInfo<T, typename std::enable_if<ColumnIsTextType<T>::value>::type>
: public ColumnTypeInfo<ColumnType::Text> {
};
//BLOB
template<typename T>
struct ColumnInfo<T, typename std::enable_if<ColumnIsBLOBType<T>::value>::type>
: public ColumnTypeInfo<ColumnType::BLOB> {
};

#pragma mark - Builtin Type
//NULL
template<>
struct WCDB_API ColumnIsNullType<std::nullptr_t> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Null>::UnderlyingType
    asUnderlyingType(const std::nullptr_t &);
};

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
    static const char *
    fromUnderlyingType(const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &t);
};

template<>
struct WCDB_API ColumnIsTextType<char *> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Text>::UnderlyingType
    asUnderlyingType(const char *text);
    static char *
    fromUnderlyingType(const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &t);
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
};

template<>
struct WCDB_API ColumnIsTextType<std::string> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Text>::UnderlyingType
    asUnderlyingType(const std::string &text);
    static std::string
    fromUnderlyingType(const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &t);
};

template<>
struct WCDB_API ColumnIsTextType<UnsafeStringView> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Text>::UnderlyingType
    asUnderlyingType(const UnsafeStringView &text);
    static UnsafeStringView
    fromUnderlyingType(const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &t);
};

template<>
struct WCDB_API ColumnIsTextType<StringView> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Text>::UnderlyingType
    asUnderlyingType(const UnsafeStringView &text);
    static StringView
    fromUnderlyingType(const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &t);
};

//BLOB
template<>
struct WCDB_API ColumnIsBLOBType<std::vector<unsigned char>> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
    asUnderlyingType(const std::vector<unsigned char> &blob);
    static std::vector<unsigned char>
    fromUnderlyingType(const ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType &t);
};

template<>
struct WCDB_API ColumnIsBLOBType<UnsafeData> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
    asUnderlyingType(const UnsafeData &blob);
    static UnsafeData
    fromUnderlyingType(const ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType &t);
};

template<>
struct WCDB_API ColumnIsBLOBType<Data> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
    asUnderlyingType(const UnsafeData &blob);
    static Data
    fromUnderlyingType(const ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType &t);
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
};

template<typename T>
struct UnsafeStringView::Convertible<T, typename std::enable_if<ColumnIsTextType<T>::value>::type>
: public std::true_type {
public:
    static UnsafeStringView asUnsafeStringView(const T &t)
    {
        return ColumnIsTextType<T>::asUnderlyingType(t);
    }
};

template<typename T>
struct UnsafeData::Convertible<T, typename std::enable_if<ColumnIsBLOBType<T>::value>::type>
: public std::true_type {
public:
    static UnsafeData asUnsafeData(const T &t)
    {
        return ColumnIsBLOBType<T>::asUnderlyingType(t);
    }
};

} //namespace WCDB
