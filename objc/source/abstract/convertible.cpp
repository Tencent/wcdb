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

#include <WCDB/convertible.hpp>
#include <WCDB/column_index.hpp>
#include <WCDB/column_result.hpp>
#include <WCDB/column.hpp>
#include <WCDB/order.hpp>
#include <WCDB/column_def.hpp>
#include <WCDB/expression.hpp>
#include <WCDB/literal_value.hpp>
#include <WCDB/subquery.hpp>

namespace WCDB {

#pragma mark - ColumnIndex

ColumnIndex ColumnIndexConvertible<ColumnIndex>::asIndex(const ColumnIndex &index) {
    return index;
}

#pragma mark - ColumnResult

ColumnResult ColumnResultConvertible<ColumnResult>::asColumnResult(const ColumnResult &columnResult)
{
    return columnResult;
}

#pragma mark - Column

Column ColumnConvertible<Column>::asColumn(const Column &column)
{
    return column;
}

Expression ExpressionConvertible<Column>::asExpression(const Column &column)
{
    return Expression(column);
}

ColumnResult ColumnResultConvertible<Column>::asColumnResult(const Column &column)
{
    return ColumnResult(column);
}

Order SpecificOrderConvertible<Column>::asOrder(const Column& column, OrderTerm term)
{
    return Order(ExpressionConvertible<Column>::asExpression(column), term);
}

Order OrderConvertible<Column>::asOrder(const Column& column)
{
    return SpecificOrderConvertible<Column>::asOrder(column, OrderTerm::NotSet);
}

ColumnIndex SpecificColumnIndexConvertible<Column>::asIndex(const Column &column, OrderTerm term)
{
    return ColumnIndex(column, term);
}

ColumnIndex ColumnIndexConvertible<Column>::asIndex(const Column &column)
{
    return SpecificColumnIndexConvertible<Column>::asIndex(column, OrderTerm::NotSet);
}

ColumnDef SpecificColumnDefConvertible<Column>::asDef(const Column &column, ColumnType columnType)
{
    return ColumnDef(column, columnType);
}

#pragma mark - Expression

Expression ExpressionConvertible<Expression>::asExpression(const Expression &expression)
{
    return expression;
}

ColumnResult ColumnResultConvertible<Expression>::asColumnResult(const Expression &expression)
{
    return ColumnResult(expression);
}

Order OrderConvertible<Expression>::asOrder(const Expression& expression)
{
    return Order(expression, OrderTerm::NotSet);
}

Order SpecificOrderConvertible<Expression>::asOrder(const Expression& expression, OrderTerm term)
{
    return Order(expression, term);
}

#pragma mark - LiteralValue

LiteralValue LiteralValueConvertible<LiteralValue>::asLiteralValue(const LiteralValue &literalValue)
{
    return literalValue;
}

Expression ExpressionConvertible<LiteralValue>::asExpression(const LiteralValue &literalValue)
{
    return Expression(literalValue);
}

ColumnResult ColumnResultConvertible<LiteralValue>::asColumnResult(const LiteralValue &literalValue)
{
    return ColumnResult(ExpressionConvertible<LiteralValue>::asExpression(literalValue));
}

Order OrderConvertible<LiteralValue>::asOrder(const LiteralValue& literalValue)
{
    return Order(ExpressionConvertible<LiteralValue>::asExpression(literalValue), OrderTerm::NotSet);
}

Order SpecificOrderConvertible<LiteralValue>::asOrder(const LiteralValue& literalValue, OrderTerm term)
{
    return Order(ExpressionConvertible<LiteralValue>::asExpression(literalValue), term);
}

#pragma mark - StatementSelect

Expression ExpressionConvertible<StatementSelect>::asExpression(const StatementSelect &statementSelect)
{
    return Expression(statementSelect);
}

ColumnResult ColumnResultConvertible<StatementSelect>::asColumnResult(const StatementSelect &statementSelect)
{
    return ColumnResult(ExpressionConvertible<StatementSelect>::asExpression(statementSelect));
}

Order OrderConvertible<StatementSelect>::asOrder(const StatementSelect& statementSelect)
{
    return Order(ExpressionConvertible<StatementSelect>::asExpression(statementSelect), OrderTerm::NotSet);
}

Order SpecificOrderConvertible<StatementSelect>::asOrder(const StatementSelect& statementSelect, OrderTerm term)
{
    return Order(ExpressionConvertible<StatementSelect>::asExpression(statementSelect), term);
}

Subquery TableOrSubqueryConvertible<StatementSelect>::asTableOrSubquery(const StatementSelect &statementSelect)
{
    return Subquery(statementSelect);
}

#pragma mark - Subquery

Subquery TableOrSubqueryConvertible<Subquery>::asTableOrSubquery(const Subquery &subquery)
{
    return subquery;
}
    
} //namespace WCDB
