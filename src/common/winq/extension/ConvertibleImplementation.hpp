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

#include "Convertible.hpp"
#include "Value.hpp"

namespace WCDB {

template<>
class WCDB_API LiteralValueConvertible<bool> final : public std::true_type {
public:
    static LiteralValue asLiteralValue(const bool& t);
};

template<>
class LiteralValueConvertible<std::nullptr_t> final : public std::true_type {
public:
    static LiteralValue asLiteralValue(const std::nullptr_t& t)
    {
        WCDB_UNUSED(t);
        return nullptr;
    }
};

template<typename T>
class LiteralValueConvertible<T, typename std::enable_if<ColumnIsFloatType<T>::value>::type> final
: public std::true_type {
public:
    static LiteralValue asLiteralValue(const T& t)
    {
        return ColumnIsFloatType<T>::asUnderlyingType(t);
    }
};

template<class T, bool = std::is_enum<T>::value>
struct SafeUnderlyingType : std::underlying_type<T> {};
template<class T>
struct SafeUnderlyingType<T, false> {};

template<typename T, typename Enable = void>
struct Is64BitUnsignedInteger : public std::false_type {};
template<typename T>
struct Is64BitUnsignedInteger<T, typename std::enable_if<std::is_integral<T>::value && (sizeof(T) > 4) && std::is_unsigned<T>::value>::type>
: public std::true_type {};

template<typename T, typename Enable = void>
struct Is64BitUnsignedEnum : public std::false_type {};
template<typename T>
struct Is64BitUnsignedEnum<T, typename std::enable_if<std::is_enum<T>::value && (sizeof(T) > 4) && std::is_unsigned<typename SafeUnderlyingType<T>::type>::value>::type>
: public std::true_type {};

template<typename T>
class LiteralValueConvertible<
T, // 32-bit-integer/enum or 64-bit-signed-integer/enum
typename std::enable_if<(std::is_integral<T>::value || std::is_enum<T>::value)
                        && !Is64BitUnsignedInteger<T>::value && !Is64BitUnsignedEnum<T>::value>::type>
final : public std::true_type {
public:
    static LiteralValue asLiteralValue(const T& t) { return (int64_t) t; }
};

template<typename T>
class LiteralValueConvertible<T, // 64-bit-unsigned-integer
                              typename std::enable_if<Is64BitUnsignedInteger<T>::value || Is64BitUnsignedEnum<T>::value>::type>
final : public std::true_type {
public:
    static LiteralValue asLiteralValue(const T& t) { return (uint64_t) t; }
};

template<typename T>
class LiteralValueConvertible<T, typename std::enable_if<ColumnIsTextType<T>::value>::type> final
: public std::true_type {
public:
    static LiteralValue asLiteralValue(const T& t)
    {
        return ColumnIsTextType<T>::asUnderlyingType(t);
    }
};

template<>
class WCDB_API LiteralValueConvertible<Value> final : public std::true_type {
public:
    static LiteralValue asLiteralValue(const Value& value);
};

template<typename T>
class ExpressionConvertible<T, typename std::enable_if<LiteralValueConvertible<T>::value>::type> final
: public std::true_type {
public:
    static Expression asExpression(const T& t)
    {
        return LiteralValueConvertible<T>::asLiteralValue(t);
    }
};

template<>
class WCDB_API ExpressionConvertible<Column> final : public std::true_type {
public:
    static Expression asExpression(const Column& column);
};

template<>
class WCDB_API ExpressionConvertible<BindParameter> final : public std::true_type {
public:
    static Expression asExpression(const BindParameter& bindParameter);
};

template<>
class WCDB_API ExpressionConvertible<LiteralValue> final : public std::true_type {
public:
    static Expression asExpression(const LiteralValue& literalValue);
};

template<>
class WCDB_API ExpressionConvertible<Value> final : public std::true_type {
public:
    static Expression asExpression(const Value& value);
};

template<>
class WCDB_API ExpressionConvertible<RaiseFunction> final : public std::true_type {
public:
    static Expression asExpression(const RaiseFunction& raiseFunction);
};

template<>
class WCDB_API ExpressionConvertible<StatementSelect> final : public std::true_type {
public:
    static Expression asExpression(const StatementSelect& statementSelect);
};

template<typename T>
class IndexedColumnConvertible<T, typename std::enable_if<ExpressionConvertible<T>::value>::type> final
: public std::true_type {
public:
    static IndexedColumn asIndexedColumn(const T& t)
    {
        return ExpressionConvertible<T>::asExpression(t);
    }
};

template<>
class WCDB_API IndexedColumnConvertible<Column> final : public std::true_type {
public:
    static IndexedColumn asIndexedColumn(const Column& column);
};

template<>
class WCDB_API IndexedColumnConvertible<Expression> final : public std::true_type {
public:
    static IndexedColumn asIndexedColumn(const Expression& expression);
};

template<typename T>
class ResultColumnConvertible<T, typename std::enable_if<ExpressionConvertible<T>::value>::type> final
: public std::true_type {
public:
    static ResultColumn asResultColumn(const T& t)
    {
        return ExpressionConvertible<T>::asExpression(t);
    }
};

template<typename T>
class ColumnConvertible<T, typename std::enable_if<ColumnIsTextType<T>::value>::type> final
: public std::true_type {
public:
    static Column asColumn(const T& t)
    {
        return ColumnIsTextType<T>::asUnderlyingType(t);
    }
};

template<>
class WCDB_API ResultColumnConvertible<Expression> final : public std::true_type {
public:
    static ResultColumn asResultColumn(const Expression& expression);
};

template<typename T>
class QualifiedTableConvertible<T, typename std::enable_if<ColumnIsTextType<T>::value>::type> final
: public std::true_type {
public:
    static QualifiedTable asQualifiedTable(const T& t)
    {
        return ColumnIsTextType<T>::asUnderlyingType(t);
    }
};

template<typename T>
class OrderingTermConvertible<T, typename std::enable_if<ExpressionConvertible<T>::value>::type> final
: public std::true_type {
public:
    static OrderingTerm asOrderingTerm(const T& t)
    {
        return ExpressionConvertible<T>::asExpression(t);
    }
};

template<>
class WCDB_API OrderingTermConvertible<Expression> final : public std::true_type {
public:
    static OrderingTerm asOrderingTerm(const Expression& expression);
};

template<typename T>
class TableOrSubqueryConvertible<T, typename std::enable_if<ColumnIsTextType<T>::value>::type> final
: public std::true_type {
public:
    static TableOrSubquery asTableOrSubquery(const T& t)
    {
        return ColumnIsTextType<T>::asUnderlyingType(t);
    }
};

template<>
class WCDB_API TableOrSubqueryConvertible<Join> final : public std::true_type {
public:
    static TableOrSubquery asTableOrSubquery(const Join& join);
};

template<>
class WCDB_API TableOrSubqueryConvertible<StatementSelect> final : public std::true_type {
public:
    static TableOrSubquery asTableOrSubquery(const StatementSelect& select);
};

} // namespace WCDB
