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

#ifndef column_type_hpp
#define column_type_hpp

#include <cstdint>
#include <string>
#include <type_traits>

namespace WCDB {

enum class ColumnType : int {
    Null,
    Float,
    Integer32,
    Integer64,
    Text,
    BLOB,
};

template <typename T, typename Enable = void>
struct ColumnIsFloatType : public std::false_type {
};
template <typename T>
struct ColumnIsFloatType<
    T,
    typename std::enable_if<std::is_floating_point<T>::value>::type>
    : public std::true_type {
};

template <typename T, typename Enable = void>
struct ColumnIsInteger32Type : public std::false_type {
};
template <typename T>
struct ColumnIsInteger32Type<
    T,
    typename std::enable_if<(std::is_integral<T>::value ||
                             std::is_enum<T>::value) &&
                            (sizeof(T) <= 4)>::type> : public std::true_type {
};

template <typename T, typename Enable = void>
struct ColumnIsInteger64Type : public std::false_type {
};
template <typename T>
struct ColumnIsInteger64Type<
    T,
    typename std::enable_if<(std::is_integral<T>::value ||
                             std::is_enum<T>::value) &&
                            (sizeof(T) > 4)>::type> : public std::true_type {
};

template <typename T, typename Enable = void>
struct ColumnIsTextType : public std::false_type {
};
template <typename T>
struct ColumnIsTextType<
    T,
    typename std::enable_if<std::is_same<std::string, T>::value ||
                            std::is_same<const char *, T>::value>::type>
    : public std::true_type {
};

template <typename T, typename Enable = void>
struct ColumnIsBLOBType : public std::false_type {
};
template <typename T>
struct ColumnIsBLOBType<
    T,
    typename std::enable_if<std::is_same<void *, T>::value ||
                            std::is_same<const void *, T>::value>::type>
    : public std::true_type {
};

//Null
template <ColumnType T = ColumnType::Null>
struct ColumnTypeInfo {
    static constexpr const bool isNull = true;
    static constexpr const bool isFloat = false;
    static constexpr const bool isInteger32 = false;
    static constexpr const bool isInteger64 = false;
    static constexpr const bool isText = false;
    static constexpr const bool isBLOB = false;
    static constexpr const bool isBaseType = false;
    using CType = void;
    static constexpr const ColumnType type = ColumnType::Null;
    static constexpr const char *name = "";
};
template <typename T, typename Enable = void>
struct ColumnInfo : public ColumnTypeInfo<ColumnType::Null> {
};
//Float
template <>
struct ColumnTypeInfo<ColumnType::Float> {
    static constexpr const bool isNull = false;
    static constexpr const bool isFloat = true;
    static constexpr const bool isInteger32 = false;
    static constexpr const bool isInteger64 = false;
    static constexpr const bool isText = false;
    static constexpr const bool isBLOB = false;
    static constexpr const bool isBaseType = true;
    using CType = double;
    static constexpr const ColumnType type = ColumnType::Float;
    static constexpr const char *name = "REAL";
};
template <typename T>
struct ColumnInfo<T, typename std::enable_if<ColumnIsFloatType<T>::value>::type>
    : public ColumnTypeInfo<ColumnType::Float> {
};
//Integer32
template <>
struct ColumnTypeInfo<ColumnType::Integer32> {
    static constexpr const bool isNull = false;
    static constexpr const bool isFloat = false;
    static constexpr const bool isInteger32 = true;
    static constexpr const bool isInteger64 = false;
    static constexpr const bool isText = false;
    static constexpr const bool isBLOB = false;
    static constexpr const bool isBaseType = true;
    using CType = int32_t;
    static constexpr const ColumnType type = ColumnType::Integer32;
    static constexpr const char *name = "INTEGER";
};
template <typename T>
struct ColumnInfo<
    T,
    typename std::enable_if<ColumnIsInteger32Type<T>::value>::type>
    : public ColumnTypeInfo<ColumnType::Integer32> {
};
//Integer64
template <>
struct ColumnTypeInfo<ColumnType::Integer64> {
    static constexpr const bool isNull = false;
    static constexpr const bool isFloat = false;
    static constexpr const bool isInteger32 = false;
    static constexpr const bool isInteger64 = true;
    static constexpr const bool isText = false;
    static constexpr const bool isBLOB = false;
    static constexpr const bool isBaseType = true;
    using CType = int64_t;
    static constexpr const ColumnType type = ColumnType::Integer64;
    static constexpr const char *name = "INTEGER";
};
template <typename T>
struct ColumnInfo<
    T,
    typename std::enable_if<ColumnIsInteger64Type<T>::value>::type>
    : public ColumnTypeInfo<ColumnType::Integer64> {
};
//Text
template <>
struct ColumnTypeInfo<ColumnType::Text> {
    static constexpr const bool isNull = false;
    static constexpr const bool isFloat = false;
    static constexpr const bool isInteger32 = false;
    static constexpr const bool isInteger64 = false;
    static constexpr const bool isText = true;
    static constexpr const bool isBLOB = false;
    static constexpr const bool isBaseType = true;
    using CType = const char *;
    static constexpr const ColumnType type = ColumnType::Text;
    static constexpr const char *name = "TEXT";
};
template <typename T>
struct ColumnInfo<T, typename std::enable_if<ColumnIsTextType<T>::value>::type>
    : public ColumnTypeInfo<ColumnType::Text> {
};
//BLOB
template <>
struct ColumnTypeInfo<ColumnType::BLOB> {
    static constexpr const bool isNull = false;
    static constexpr const bool isFloat = false;
    static constexpr const bool isInteger32 = false;
    static constexpr const bool isInteger64 = false;
    static constexpr const bool isText = false;
    static constexpr const bool isBLOB = true;
    static constexpr const bool isBaseType = false;
    using CType = const void *;
    using SizeType = int;
    static constexpr const ColumnType type = ColumnType::BLOB;
    static constexpr const char *name = "BLOB";
};
template <typename T>
struct ColumnInfo<T, typename std::enable_if<ColumnIsBLOBType<T>::value>::type>
    : public ColumnTypeInfo<ColumnType::BLOB> {
};

const char *ColumnTypeName(ColumnType type);

} //namespace WCDB

#endif /* column_type_hpp */
