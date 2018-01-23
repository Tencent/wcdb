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

#ifndef convertible_hpp
#define convertible_hpp

#include <WCDB/declare.hpp>

namespace WCDB {

#pragma mark - Converible
    
template <typename T, typename Enable = void>
struct OrderConvertible : public std::false_type
{
public:
    static Order asOrder(const T& t);
};
    
template <typename T, typename Enable = void>
struct SpecificOrderConvertible : public std::false_type
{
public:
    static Order asOrder(const T& t, OrderTerm term);
};
    
template <typename T, typename Enable = void>
struct SpecificColumnDefConvertible : public std::false_type
{
public:
    static ColumnDef asDef(const T& t, ColumnType columnType);
};

template <typename T, typename Enable = void>
struct ColumnIndexConvertible : public std::false_type
{
public:
    static ColumnIndex asIndex(const T &t);
};

template <typename T, typename Enable = void>
struct SpecificColumnIndexConvertible : public std::false_type
{
public:
    static ColumnIndex asIndex(const T &t, OrderTerm term);
};

template <typename T, typename Enable = void>
struct ColumnResultConvertible : public std::false_type
{
public:
    static ColumnResult asColumnResult(const T &t);
};

template <typename T, typename Enable = void>
struct TableOrSubqueryConvertible : public std::false_type
{
public:
    static Subquery asTableOrSubquery(const T &t);
};

template <typename T, typename Enable = void>
struct ExpressionConvertible : public std::false_type
{
public:
    static Expression asExpression(const T &t);
};
    
template <typename T, typename Enable = void>
struct ColumnConvertible : public std::false_type
{
public:
    static Column asColumn(const T &t);
};
    
template <typename T, typename Enable = void>
struct LiteralValueConvertible : public std::false_type
{
public:
    static LiteralValue asLiteralValue(const T &t);
};

#pragma mark - ColumnIndex
    
template <>
struct ColumnIndexConvertible<ColumnIndex> : public std::true_type
{
public:
    static ColumnIndex asIndex(const ColumnIndex &index);
};

#pragma mark - ColumnResult

template <>
struct ColumnResultConvertible<ColumnResult> : public std::true_type
{
public:
    static ColumnResult asColumnResult(const ColumnResult &columnResult);
};

#pragma mark - Column

template <>
struct ColumnConvertible<Column> : public std::true_type
{
public:
    static Column asColumn(const Column &column);
};

template <>
struct ExpressionConvertible<Column> : public std::true_type
{
public:
    static Expression asExpression(const Column &column);
};

template <>
struct ColumnResultConvertible<Column> : public std::true_type
{
public:
    static ColumnResult asColumnResult(const Column &column);
};

template <>
struct SpecificOrderConvertible<Column> : public std::true_type
{
public:
    static Order asOrder(const Column& column, OrderTerm term);
};

template <>
struct OrderConvertible<Column> : public std::true_type
{
public:
    static Order asOrder(const Column& column);
};

template <>
struct SpecificColumnIndexConvertible<Column> : public std::true_type
{
public:
    static ColumnIndex asIndex(const Column &column, OrderTerm term) ;
};

template <>
struct ColumnIndexConvertible<Column> : public std::true_type
{
public:
    static ColumnIndex asIndex(const Column &column) ;
};

template <>
struct SpecificColumnDefConvertible<Column> : public std::true_type
{
public:
    static ColumnDef asDef(const Column &column, ColumnType columnType);
};

#pragma mark - Expression

template <>
struct ExpressionConvertible<Expression> : public std::true_type
{
public:
    static Expression asExpression(const Expression &expression) ;
};

template <>
struct ColumnResultConvertible<Expression> : public std::true_type
{
public:
    static ColumnResult asColumnResult(const Expression &expression) ;
};

template <>
struct OrderConvertible<Expression> : public std::true_type
{
public:
    static Order asOrder(const Expression& expression);
};

template <>
struct SpecificOrderConvertible<Expression> : public std::true_type
{
public:
    static Order asOrder(const Expression& expression, OrderTerm term);
};

#pragma mark - LiteralValue

template <>
struct LiteralValueConvertible<LiteralValue> : public std::true_type
{
public:
    static LiteralValue asLiteralValue(const LiteralValue &literalValue) ;
};

template <>
struct ExpressionConvertible<LiteralValue> : public std::true_type
{
public:
    static Expression asExpression(const LiteralValue &literalValue);
};

template <>
struct ColumnResultConvertible<LiteralValue> : public std::true_type
{
public:
    static ColumnResult asColumnResult(const LiteralValue &literalValue);
};

template <>
struct OrderConvertible<LiteralValue> : public std::true_type
{
public:
    static Order asOrder(const LiteralValue& literalValue);
};

template <>
struct SpecificOrderConvertible<LiteralValue> : public std::true_type
{
public:
    static Order asOrder(const LiteralValue& literalValue, OrderTerm term) ;
};

#pragma mark - StatementSelect

template <>
struct ExpressionConvertible<StatementSelect> : public std::true_type
{
public:
    static Expression asExpression(const StatementSelect &statementSelect);
};

template <>
struct ColumnResultConvertible<StatementSelect> : public std::true_type
{
public:
    static ColumnResult asColumnResult(const StatementSelect &statementSelect);
};

template <>
struct OrderConvertible<StatementSelect> : public std::true_type
{
public:
    static Order asOrder(const StatementSelect& statementSelect);
};

template <>
struct SpecificOrderConvertible<StatementSelect> : public std::true_type
{
public:
    static Order asOrder(const StatementSelect& statementSelect, OrderTerm term) ;
};

template <>
struct TableOrSubqueryConvertible<StatementSelect> : public std::true_type
{
public:
    static Subquery asTableOrSubquery(const StatementSelect &statementSelect) ;
};

#pragma mark - Subquery

template <>
struct TableOrSubqueryConvertible<Subquery> : public std::true_type
{
public:
    static Subquery asTableOrSubquery(const Subquery &subquery) ;
};
    
#pragma mark - Null Type
template <typename T>
struct LiteralValueConvertible<T, typename std::enable_if<ColumnIsNullType<T>::value>::type> : public std::true_type
{
public:
    static LiteralValue asLiteralValue(const T &t);
};
    
template <typename T>
struct ExpressionConvertible<T, typename std::enable_if<ColumnIsNullType<T>::value>::type> : public std::true_type
{
public:
    static Expression asExpression(const T &t);
};

template <typename T>
struct ColumnResultConvertible<T, typename std::enable_if<ColumnIsNullType<T>::value>::type> : public std::true_type
{
public:
    static ColumnResult asColumnResult(const T &t);
};

template <typename T>
struct OrderConvertible<T, typename std::enable_if<ColumnIsNullType<T>::value>::type> : public std::true_type
{
public:
    static Order asOrder(const T& t);
};

template <typename T>
struct SpecificOrderConvertible<T, typename std::enable_if<ColumnIsNullType<T>::value>::type> : public std::true_type
{
public:
    static Order asOrder(const T& t, OrderTerm term) ;
};
    
#pragma mark - Float Type
template <typename T>
struct LiteralValueConvertible<T, typename std::enable_if<ColumnIsFloatType<T>::value>::type> : public std::true_type
{
public:
    static LiteralValue asLiteralValue(const T &t);
};
    
template <typename T>
struct ExpressionConvertible<T, typename std::enable_if<ColumnIsFloatType<T>::value>::type> : public std::true_type
{
public:
    static Expression asExpression(const T &t);
};

template <typename T>
struct ColumnResultConvertible<T, typename std::enable_if<ColumnIsFloatType<T>::value>::type> : public std::true_type
{
public:
    static ColumnResult asColumnResult(const T &t);
};

template <typename T>
struct OrderConvertible<T, typename std::enable_if<ColumnIsFloatType<T>::value>::type> : public std::true_type
{
public:
    static Order asOrder(const T& t);
};

template <typename T>
struct SpecificOrderConvertible<T, typename std::enable_if<ColumnIsFloatType<T>::value>::type> : public std::true_type
{
public:
    static Order asOrder(const T& t, OrderTerm term) ;
};

#pragma mark - Integer32 Type
template <typename T>
struct LiteralValueConvertible<T, typename std::enable_if<ColumnIsInteger32Type<T>::value>::type> : public std::true_type
{
public:
    static LiteralValue asLiteralValue(const T &t);
};

template <typename T>
struct ExpressionConvertible<T, typename std::enable_if<ColumnIsInteger32Type<T>::value>::type> : public std::true_type
{
public:
    static Expression asExpression(const T &t);
};

template <typename T>
struct ColumnResultConvertible<T, typename std::enable_if<ColumnIsInteger32Type<T>::value>::type> : public std::true_type
{
public:
    static ColumnResult asColumnResult(const T &t);
};

template <typename T>
struct OrderConvertible<T, typename std::enable_if<ColumnIsInteger32Type<T>::value>::type> : public std::true_type
{
public:
    static Order asOrder(const T& t);
};

template <typename T>
struct SpecificOrderConvertible<T, typename std::enable_if<ColumnIsInteger32Type<T>::value>::type> : public std::true_type
{
public:
    static Order asOrder(const T& t, OrderTerm term) ;
};

#pragma mark - Integer64 Type
template <typename T>
struct LiteralValueConvertible<T, typename std::enable_if<ColumnIsInteger64Type<T>::value>::type> : public std::true_type
{
public:
    static LiteralValue asLiteralValue(const T &t);
};
    
template <typename T>
struct ExpressionConvertible<T, typename std::enable_if<ColumnIsInteger64Type<T>::value>::type> : public std::true_type
{
public:
    static Expression asExpression(const T &t);
};

template <typename T>
struct ColumnResultConvertible<T, typename std::enable_if<ColumnIsInteger64Type<T>::value>::type> : public std::true_type
{
public:
    static ColumnResult asColumnResult(const T &t);
};

template <typename T>
struct OrderConvertible<T, typename std::enable_if<ColumnIsInteger64Type<T>::value>::type> : public std::true_type
{
public:
    static Order asOrder(const T& t);
};

template <typename T>
struct SpecificOrderConvertible<T, typename std::enable_if<ColumnIsInteger64Type<T>::value>::type> : public std::true_type
{
public:
    static Order asOrder(const T& t, OrderTerm term) ;
};

#pragma mark - Text Type
template <typename T>
struct LiteralValueConvertible<T, typename std::enable_if<ColumnIsTextType<T>::value>::type> : public std::true_type
{
public:
    static LiteralValue asLiteralValue(const T &t);
};
    
template <typename T>
struct ExpressionConvertible<T, typename std::enable_if<ColumnIsTextType<T>::value>::type> : public std::true_type
{
public:
    static Expression asExpression(const T &t);
};

template <typename T>
struct ColumnResultConvertible<T, typename std::enable_if<ColumnIsTextType<T>::value>::type> : public std::true_type
{
public:
    static ColumnResult asColumnResult(const T &t);
};

template <typename T>
struct OrderConvertible<T, typename std::enable_if<ColumnIsTextType<T>::value>::type> : public std::true_type
{
public:
    static Order asOrder(const T& t);
};

template <typename T>
struct SpecificOrderConvertible<T, typename std::enable_if<ColumnIsTextType<T>::value>::type> : public std::true_type
{
public:
    static Order asOrder(const T& t, OrderTerm term) ;
};

template <typename T>
struct TableOrSubqueryConvertible<T, typename std::enable_if<ColumnIsTextType<T>::value>::type> : public std::true_type
{
public:
    static Subquery asTableOrSubquery(const T &t) ;
};

#pragma mark - BLOB Type
template <typename T>
struct LiteralValueConvertible<T, typename std::enable_if<ColumnIsBLOBType<T>::value>::type> : public std::true_type
{
public:
    static LiteralValue asLiteralValue(const T &t);
};
    
template <typename T>
struct ExpressionConvertible<T, typename std::enable_if<ColumnIsBLOBType<T>::value>::type> : public std::true_type
{
public:
    static Expression asExpression(const T &t);
};

template <typename T>
struct ColumnResultConvertible<T, typename std::enable_if<ColumnIsBLOBType<T>::value>::type> : public std::true_type
{
public:
    static ColumnResult asColumnResult(const T &t);
};

template <typename T>
struct OrderConvertible<T, typename std::enable_if<ColumnIsBLOBType<T>::value>::type> : public std::true_type
{
public:
    static Order asOrder(const T& t);
};

template <typename T>
struct SpecificOrderConvertible<T, typename std::enable_if<ColumnIsBLOBType<T>::value>::type> : public std::true_type
{
public:
    static Order asOrder(const T& t, OrderTerm term) ;
};

} //namespace WCDB

#endif /* convertible_hpp */
