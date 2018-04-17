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

#ifndef ConvertibleImpl_hpp
#define ConvertibleImpl_hpp

#include <WCDB/Expression.hpp>
#include <WCDB/LiteralValue.hpp>
#include <WCDB/ColumnType.hpp>
#include <WCDB/RaiseFunction.hpp>
#include <WCDB/Column.hpp>
#include <WCDB/BindParameter.hpp>

namespace WCDB {

template <typename T>
class ExpressionConvertible<
    T,
    typename std::enable_if<ColumnIsNullType<T>::value>::type>
    : public std::true_type {
public:
    static Expression as(const T &t) { return LiteralValue(t); }
};

template <typename T>
class ExpressionConvertible<
    T,
    typename std::enable_if<ColumnIsInteger32Type<T>::value>::type>
    : public std::true_type {
public:
    static Expression as(const T &t) { return LiteralValue(t); }
};

template <typename T>
class ExpressionConvertible<
    T,
    typename std::enable_if<ColumnIsInteger64Type<T>::value>::type>
    : public std::true_type {
public:
    static Expression as(const T &t) { return LiteralValue(t); }
};

template <typename T>
class ExpressionConvertible<
    T,
    typename std::enable_if<ColumnIsFloatType<T>::value>::type>
    : public std::true_type {
public:
    static Expression as(const T &t) { return LiteralValue(t); }
};

template <typename T>
class ExpressionConvertible<
    T,
    typename std::enable_if<ColumnIsTextType<T>::value>::type>
    : public std::true_type {
public:
    static Expression as(const T &t) { return LiteralValue(t); }
};

template <typename T>
class ExpressionConvertible<
    T,
    typename std::enable_if<ColumnIsBLOBType<T>::value>::type>
    : public std::true_type {
public:
    static Expression as(const T &t) { return LiteralValue(t); }
};

template <>
class ExpressionConvertible<LiteralValue> : public std::true_type {
public:
    static Expression as(const LiteralValue &literalValue);
};

template <>
class ExpressionConvertible<Column> : public std::true_type {
public:
    static Expression as(const Column &column);
};

template <>
class ExpressionConvertible<BindParameter> : public std::true_type {
public:
    static Expression as(const BindParameter &bindParameter);
};

template <>
class ExpressionConvertible<RaiseFunction> : public std::true_type {
public:
    static Expression as(const RaiseFunction &raiseFunction);
};

template <>
class ExpressionConvertible<StatementSelect> : public std::true_type {
public:
    static Expression as(const StatementSelect &statementSelect);
};

template <typename T>
class LiteralValueConvertible<
    T,
    typename std::enable_if<ColumnIsNullType<T>::value>::type>
    : public std::true_type {
public:
    static LiteralValue as(const T &t) { return t; }
};

template <typename T>
class LiteralValueConvertible<
    T,
    typename std::enable_if<ColumnIsInteger32Type<T>::value>::type>
    : public std::true_type {
public:
    static LiteralValue as(const T &t) { return t; }
};

template <typename T>
class LiteralValueConvertible<
    T,
    typename std::enable_if<ColumnIsInteger64Type<T>::value>::type>
    : public std::true_type {
public:
    static LiteralValue as(const T &t) { return t; }
};

template <typename T>
class LiteralValueConvertible<
    T,
    typename std::enable_if<ColumnIsFloatType<T>::value>::type>
    : public std::true_type {
public:
    static LiteralValue as(const T &t) { return t; }
};

template <typename T>
class LiteralValueConvertible<
    T,
    typename std::enable_if<ColumnIsTextType<T>::value>::type>
    : public std::true_type {
public:
    static LiteralValue as(const T &t) { return t; }
};

template <typename T>
class LiteralValueConvertible<
    T,
    typename std::enable_if<ColumnIsBLOBType<T>::value>::type>
    : public std::true_type {
public:
    static LiteralValue as(const T &t) { return t; }
};

} // namespace WCDB

#endif /* ConvertibleImpl_hpp */
