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

#ifndef __WCDB_CONVERTIBLE_IMPLEMENTATION_HPP
#define __WCDB_CONVERTIBLE_IMPLEMENTATION_HPP

#include <WCDB/Convertible.hpp>

namespace WCDB {

template<>
class LiteralValueConvertible<bool> final : public std::true_type {
public:
    static LiteralValue asLiteralValue(const bool& t);
};

template<typename T>
class LiteralValueConvertible<T, typename std::enable_if<ColumnIsNullType<T>::value>::type> final
: public std::true_type {
public:
    static LiteralValue asLiteralValue(const T& t) { return nullptr; }
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

template<typename T>
class LiteralValueConvertible<T, typename std::enable_if<ColumnIsInteger64Type<T>::value>::type> final
: public std::true_type {
public:
    static LiteralValue asLiteralValue(const T& t)
    {
        return ColumnIsInteger64Type<T>::asUnderlyingType(t);
    }
};

template<typename T>
class LiteralValueConvertible<T, typename std::enable_if<ColumnIsInteger32Type<T>::value>::type> final
: public std::true_type {
public:
    static LiteralValue asLiteralValue(const T& t)
    {
        return (int64_t) ColumnIsInteger32Type<T>::asUnderlyingType(t);
    }
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
class ExpressionConvertible<Column> final : public std::true_type {
public:
    static Expression asExpression(const Column& column);
};

template<>
class ExpressionConvertible<BindParameter> final : public std::true_type {
public:
    static Expression asExpression(const BindParameter& bindParameter);
};

template<>
class ExpressionConvertible<LiteralValue> final : public std::true_type {
public:
    static Expression asExpression(const LiteralValue& literalValue);
};

template<>
class ExpressionConvertible<RaiseFunction> final : public std::true_type {
public:
    static Expression asExpression(const RaiseFunction& raiseFunction);
};

template<>
class ExpressionConvertible<StatementSelect> final : public std::true_type {
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
class IndexedColumnConvertible<Column> final : public std::true_type {
public:
    static IndexedColumn asIndexedColumn(const Column& column);
};

template<>
class IndexedColumnConvertible<Expression> final : public std::true_type {
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

template<>
class ResultColumnConvertible<Expression> final : public std::true_type {
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
class CTETableConvertible<T, typename std::enable_if<ColumnIsTextType<T>::value>::type> final
: public std::true_type {
public:
    static CTETable asCTETable(const T& t)
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
class OrderingTermConvertible<Expression> final : public std::true_type {
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
class TableOrSubqueryConvertible<Join> final : public std::true_type {
public:
    static TableOrSubquery asTableOrSubquery(const Join& join);
};

template<>
class TableOrSubqueryConvertible<StatementSelect> final : public std::true_type {
public:
    static TableOrSubquery asTableOrSubquery(const StatementSelect& select);
};

} // namespace WCDB

#endif /* __WCDB_CONVERTIBLE_IMPLEMENTATION_HPP */
