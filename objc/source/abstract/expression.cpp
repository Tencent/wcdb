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

#include <WCDB/utility.hpp>
#include <WCDB/expression.hpp>
#include <WCDB/statement_select.hpp>
#include <WCDB/column.hpp>
#include <WCDB/convertible_impl.hpp>

namespace WCDB {

#pragma mark - Expression
    
const Expression Expression::BindParameter(Column("?"));

Expression::Expression(const std::string &raw, const std::nullptr_t& dummy) : Describable(raw)
{
}

Expression::Expression(const StatementSelect &statementSelect)
    : Describable(statementSelect.getDescription())
{
}

Expression Expression::Exists(const StatementSelect &statementSelect)
{
    return Operator::operateWithPrefix("EXISTS ", statementSelect);
}

Expression Expression::NotExists(const StatementSelect &statementSelect)
{
    return Operator::operateWithPrefix("NOT EXISTS ", statementSelect);
}

Expression::CaseExpression Expression::Case(const Expression &expression)
{
    return Expression::CaseExpression(expression);
}
    
Expression Expression::Function(const std::string& name, const std::list<const Expression> &list, bool isDistinct)
{
    return Operator::operateWithTitle(name, isDistinct?"DISTINCT":"", list);
}
    
Expression Expression::Function(const std::string& name, const Expression &expression, bool isDistinct)
{
    return Operator::operateWithTitle(name, isDistinct?"DISTINCT":"", {expression});
}
    
Expression Expression::Combine(const std::list<const Expression> &list) {
    return Expression("(" + stringByJoiningList(list) + ")", nullptr);
}
    
Expression Expression::asExpression() const
{
    return *this;
}
    
Expression::operator std::list<const Expression>() const
{
    return {*this};
}
    
#pragma mark - CaseExpression
    
Expression::CaseExpression &Expression::CaseExpression::when(const Expression &expression)
{
    m_description.append(" WHEN " + expression.getDescription());
    return *this;
}
    
Expression::CaseExpression &Expression::CaseExpression::then(const Expression &expression)
{
    m_description.append(" THEN " + expression.getDescription());
    return *this;
}
    
Expression::CaseExpression &Expression::CaseExpression::else_(const Expression &expression)
{
    m_description.append(" ELSE " + expression.getDescription() + " END");
    return *this;
}
    
Expression::CaseExpression::CaseExpression(const Expression &expression)
    : Describable("CASE " + expression.getDescription())
{
}
    
} //namespace WCDB
