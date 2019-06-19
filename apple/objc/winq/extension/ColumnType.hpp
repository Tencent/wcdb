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

#include <WCDB/StringView.hpp>
#include <WCDB/Syntax.h>
#include <WCDB/SyntaxForwardDeclaration.h>
#include <WCDB/UnsafeData.hpp>
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
    static constexpr const bool isInteger32 = false;
    static constexpr const bool isInteger64 = false;
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
    static constexpr const bool isInteger32 = false;
    static constexpr const bool isInteger64 = false;
    static constexpr const bool isText = false;
    static constexpr const bool isBLOB = false;
    static constexpr const bool isBaseType = true;
    using UnderlyingType = double;
    static constexpr const ColumnType type = ColumnType::Float;
};
//Integer32
template<>
struct ColumnTypeInfo<ColumnType::Integer32> {
    static constexpr const bool isNull = false;
    static constexpr const bool isFloat = false;
    static constexpr const bool isInteger32 = true;
    static constexpr const bool isInteger64 = false;
    static constexpr const bool isText = false;
    static constexpr const bool isBLOB = false;
    static constexpr const bool isBaseType = true;
    using UnderlyingType = int32_t;
    static constexpr const ColumnType type = ColumnType::Integer32;
};
//Integer64
template<>
struct ColumnTypeInfo<ColumnType::Integer64> {
    static constexpr const bool isNull = false;
    static constexpr const bool isFloat = false;
    static constexpr const bool isInteger32 = false;
    static constexpr const bool isInteger64 = true;
    static constexpr const bool isText = false;
    static constexpr const bool isBLOB = false;
    static constexpr const bool isBaseType = true;
    using UnderlyingType = int64_t;
    static constexpr const ColumnType type = ColumnType::Integer64;
};
//Text
template<>
struct ColumnTypeInfo<ColumnType::Text> {
    static constexpr const bool isNull = false;
    static constexpr const bool isFloat = false;
    static constexpr const bool isInteger32 = false;
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
    static constexpr const bool isInteger32 = false;
    static constexpr const bool isInteger64 = false;
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
//Integer32
template<typename T, typename Enable = void>
struct ColumnIsInteger32Type : public std::false_type {
public:
    static ColumnTypeInfo<ColumnType::Integer32>::UnderlyingType
    asUnderlyingType(const T &);
};
//Integer64
template<typename T, typename Enable = void>
struct ColumnIsInteger64Type : public std::false_type {
public:
    static ColumnTypeInfo<ColumnType::Integer64>::UnderlyingType
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
//Integer32
template<typename T>
struct ColumnInfo<T, typename std::enable_if<ColumnIsInteger32Type<T>::value>::type>
: public ColumnTypeInfo<ColumnType::Integer32> {
};
//Integer64
template<typename T>
struct ColumnInfo<T, typename std::enable_if<ColumnIsInteger64Type<T>::value>::type>
: public ColumnTypeInfo<ColumnType::Integer64> {
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
struct ColumnIsNullType<std::nullptr_t> : public std::true_type {
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

//Integer32
template<typename T>
struct ColumnIsInteger32Type<T, typename std::enable_if<(std::is_integral<T>::value || std::is_enum<T>::value) && (sizeof(T) <= 4)>::type>
: public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Integer32>::UnderlyingType
    asUnderlyingType(const T &t)
    {
        return (ColumnTypeInfo<ColumnType::Integer32>::UnderlyingType) t;
    }
};

//Integer64
template<typename T>
struct ColumnIsInteger64Type<T, typename std::enable_if<(std::is_integral<T>::value || std::is_enum<T>::value) && (sizeof(T) > 4)>::type>
: public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Integer64>::UnderlyingType
    asUnderlyingType(const T &t)
    {
        return (ColumnTypeInfo<ColumnType::Integer64>::UnderlyingType) t;
    }
};

//Text
template<>
struct ColumnIsTextType<const char *> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Text>::UnderlyingType
    asUnderlyingType(const char *text);
};

template<>
struct ColumnIsTextType<char *> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Text>::UnderlyingType
    asUnderlyingType(const char *text);
};

template<int size>
struct ColumnIsTextType<const char[size]> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Text>::UnderlyingType
    asUnderlyingType(const char text[size])
    {
        return text;
    }
};

template<int size>
struct ColumnIsTextType<char[size]> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Text>::UnderlyingType
    asUnderlyingType(const char text[size])
    {
        return text;
    }
};

template<>
struct ColumnIsTextType<std::string> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Text>::UnderlyingType
    asUnderlyingType(const std::string &text);
};

template<>
struct ColumnIsTextType<UnsafeStringView> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Text>::UnderlyingType
    asUnderlyingType(const UnsafeStringView &text);
};

template<>
struct ColumnIsTextType<StringView> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::Text>::UnderlyingType
    asUnderlyingType(const UnsafeStringView &text);
};

//BLOB
template<>
struct ColumnIsBLOBType<std::vector<unsigned char>> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
    asUnderlyingType(const std::vector<unsigned char> &blob);
};

template<>
struct ColumnIsBLOBType<UnsafeData> : public std::true_type {
public:
    static ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType
    asUnderlyingType(const UnsafeData &blob);
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

} //namespace WCDB
