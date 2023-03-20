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

#include "WINQ.h"

namespace WCDB {

LiteralValue LiteralValueConvertible<bool>::asLiteralValue(const bool &t)
{
    return t;
}

LiteralValue LiteralValueConvertible<Value>::asLiteralValue(const Value &value)
{
    switch (value.getType()) {
    case Value::Type::Null:
        return nullptr;
    case Value::Type::Integer:
        return value.intValue();
    case Value::Type::Float:
        return value.floatValue();
    default:
        return value.textValue();
    }
}

Expression ExpressionConvertible<Column>::asExpression(const Column &column)
{
    return column;
}

Expression ExpressionConvertible<BindParameter>::asExpression(const BindParameter &bindParameter)
{
    return bindParameter;
}

Expression ExpressionConvertible<LiteralValue>::asExpression(const LiteralValue &literalValue)
{
    return literalValue;
}

Expression ExpressionConvertible<Value>::asExpression(const Value &value)
{
    return LiteralValue(value);
}

Expression ExpressionConvertible<RaiseFunction>::asExpression(const RaiseFunction &raiseFunction)
{
    return raiseFunction;
}

Expression ExpressionConvertible<StatementSelect>::asExpression(const StatementSelect &statementSelect)
{
    return statementSelect;
}

IndexedColumn IndexedColumnConvertible<Column>::asIndexedColumn(const Column &column)
{
    return column;
}

IndexedColumn
IndexedColumnConvertible<Expression>::asIndexedColumn(const Expression &expression)
{
    return expression;
}

ResultColumn ResultColumnConvertible<Expression>::asResultColumn(const Expression &expression)
{
    return expression;
}

OrderingTerm OrderingTermConvertible<Expression>::asOrderingTerm(const Expression &expression)
{
    return expression;
}

TableOrSubquery TableOrSubqueryConvertible<Join>::asTableOrSubquery(const Join &join)
{
    return join;
}

TableOrSubquery
TableOrSubqueryConvertible<StatementSelect>::asTableOrSubquery(const StatementSelect &statementSelect)
{
    return statementSelect;
}

} // namespace WCDB
