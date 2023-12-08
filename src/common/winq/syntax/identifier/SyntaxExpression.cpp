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

#include "Syntax.h"
#include "SyntaxAssertion.hpp"
#include "SyntaxEnum.hpp"

namespace WCDB {

template<>
constexpr const char*
Enum::description(const Syntax::Expression::UnaryOperator& unaryOperator)
{
    switch (unaryOperator) {
    case Syntax::Expression::UnaryOperator::Negative:
        return "-";
    case Syntax::Expression::UnaryOperator::Positive:
        return "+";
    case Syntax::Expression::UnaryOperator::Tilde:
        return "~";
    case Syntax::Expression::UnaryOperator::Not:
        return "NOT";
    case Syntax::Expression::UnaryOperator::Null:
        return "NULL";
    }
}

template<>
constexpr const char*
Enum::description(const Syntax::Expression::BinaryOperator& binaryOperator)
{
    switch (binaryOperator) {
    case Syntax::Expression::BinaryOperator::Concatenate:
        return "||";
    case Syntax::Expression::BinaryOperator::Multiply:
        return "*";
    case Syntax::Expression::BinaryOperator::Divide:
        return "/";
    case Syntax::Expression::BinaryOperator::Modulo:
        return "%";
    case Syntax::Expression::BinaryOperator::Plus:
        return "+";
    case Syntax::Expression::BinaryOperator::Minus:
        return "-";
    case Syntax::Expression::BinaryOperator::LeftShift:
        return "<<";
    case Syntax::Expression::BinaryOperator::RightShift:
        return ">>";
    case Syntax::Expression::BinaryOperator::BitwiseAnd:
        return "&";
    case Syntax::Expression::BinaryOperator::BitwiseOr:
        return "|";
    case Syntax::Expression::BinaryOperator::Less:
        return "<";
    case Syntax::Expression::BinaryOperator::LessOrEqual:
        return "<=";
    case Syntax::Expression::BinaryOperator::Greater:
        return ">";
    case Syntax::Expression::BinaryOperator::GreaterOrEqual:
        return ">=";
    case Syntax::Expression::BinaryOperator::Equal:
        return "==";
    case Syntax::Expression::BinaryOperator::NotEqual:
        return "!=";
    case Syntax::Expression::BinaryOperator::Is:
        return "IS";
    case Syntax::Expression::BinaryOperator::And:
        return "AND";
    case Syntax::Expression::BinaryOperator::Or:
        return "OR";
    case Syntax::Expression::BinaryOperator::Like:
        return "LIKE";
    case Syntax::Expression::BinaryOperator::GLOB:
        return "GLOB";
    case Syntax::Expression::BinaryOperator::RegExp:
        return "REGEXP";
    case Syntax::Expression::BinaryOperator::Match:
        return "MATCH";
    }
}

namespace Syntax {

#pragma mark - ExpressionMember
void ExpressionUnionMember::firstMemberReset() const
{
    if (m_firstMember == Member::Invalid) {
        return;
    }
    switch (m_firstMember) {
    case Member::literalValue:
        m_literalValue.~LiteralValue();
        break;
    case Member::bindParameter:
        m_bindParameter.~BindParameter();
        break;
    case Member::column:
        m_column.~Column();
        break;
    case Member::raiseFunction:
        m_raiseFunction.~RaiseFunction();
        break;
    case Member::windowDef:
        m_windowDef.~WindowDef();
        break;
    case Member::windowName:
        m_windowName.~StringView();
        break;
    default:
        break;
    }
    m_firstMember = Member::Invalid;
}

void ExpressionUnionMember::secondMemberReset() const
{
    if (m_secondMember == Member::Invalid) {
        return;
    }
    switch (m_secondMember) {
    case Member::select:
        m_select.~Shadow<SelectSTMT>();
        break;
    case Member::collation:
        m_collation.~StringView();
        break;
    case Member::schema:
        m_schema.~Schema();
        break;
    case Member::filter:
        m_filter.~Filter();
        break;
    default:
        break;
    }
    m_secondMember = Member::Invalid;
}

void ExpressionUnionMember::thirdMemberReset() const
{
    if (m_thirdMember == Member::Invalid) {
        return;
    }
    switch (m_thirdMember) {
    case Member::table:
        m_table.~StringView();
        break;
    case Member::function:
        m_function.~StringView();
    default:
        break;
    }
    m_thirdMember = Member::Invalid;
}

template<typename T>
void ExpressionUnionMember::assignFromOther(T&& other)
{
    if (other.m_firstMember != Member::Invalid) {
        switch (other.m_firstMember) {
        case Member::literalValue:
            new ((void*) std::addressof(m_literalValue))
            LiteralValue(std::forward<T>(other).m_literalValue);
            break;
        case Member::bindParameter:
            new ((void*) std::addressof(m_bindParameter))
            BindParameter(std::forward<T>(other).m_bindParameter);
            break;
        case Member::column:
            new ((void*) std::addressof(m_column)) Column(std::forward<T>(other).m_column);
            break;
        case Member::raiseFunction:
            new ((void*) std::addressof(m_raiseFunction))
            RaiseFunction(std::forward<T>(other).m_raiseFunction);
            break;
        case Member::windowDef:
            new ((void*) std::addressof(m_windowDef))
            WindowDef(std::forward<T>(other).m_windowDef);
            break;
        case Member::windowName:
            new ((void*) std::addressof(m_windowName))
            StringView(std::forward<T>(other).m_windowName);
            break;
        default:
            break;
        }
        m_firstMember = other.m_firstMember;
    }
    if (other.m_secondMember != Member::Invalid) {
        switch (other.m_secondMember) {
        case Member::select:
            new ((void*) std::addressof(m_select))
            Shadow<SelectSTMT>(std::forward<T>(other).m_select);
            break;
        case Member::collation:
            new ((void*) std::addressof(m_collation))
            StringView(std::forward<T>(other).m_collation);
            break;
        case Member::schema:
            new ((void*) std::addressof(m_schema)) Schema(std::forward<T>(other).m_schema);
            break;
        case Member::filter:
            new ((void*) std::addressof(m_filter)) Filter(std::forward<T>(other).m_filter);
            break;
        default:
            break;
        }
        m_secondMember = other.m_secondMember;
    }
    if (other.m_thirdMember != Member::Invalid) {
        switch (other.m_thirdMember) {
        case Member::table:
            new ((void*) std::addressof(m_table))
            StringView(std::forward<T>(other).m_table);
            break;
        case Member::function:
            new ((void*) std::addressof(m_function))
            StringView(std::forward<T>(other).m_function);
            break;
        default:
            break;
        }
        m_thirdMember = other.m_thirdMember;
    }
}

ExpressionUnionMember::ExpressionUnionMember()
: m_firstMember(Member::Invalid)
, m_secondMember(Member::Invalid)
, m_thirdMember(Member::Invalid)
{
}

ExpressionUnionMember::~ExpressionUnionMember()
{
    firstMemberReset();
    secondMemberReset();
    thirdMemberReset();
};

ExpressionUnionMember::ExpressionUnionMember(const ExpressionUnionMember& other)
{
    assignFromOther(other);
}

ExpressionUnionMember::ExpressionUnionMember(ExpressionUnionMember&& other)
{
    assignFromOther(std::move(other));
}

ExpressionUnionMember& ExpressionUnionMember::operator=(const ExpressionUnionMember& other)
{
    firstMemberReset();
    secondMemberReset();
    thirdMemberReset();
    assignFromOther(other);
    return *this;
}

ExpressionUnionMember& ExpressionUnionMember::operator=(ExpressionUnionMember&& other)
{
    firstMemberReset();
    secondMemberReset();
    thirdMemberReset();
    assignFromOther(std::move(other));
    return *this;
}

WCDB_SYNTAX_UNION_MEMBER_IMPLEMENT(ExpressionUnionMember, firstMember, LiteralValue, literalValue)
WCDB_SYNTAX_UNION_MEMBER_IMPLEMENT(ExpressionUnionMember, firstMember, BindParameter, bindParameter)
WCDB_SYNTAX_UNION_MEMBER_IMPLEMENT(ExpressionUnionMember, firstMember, Column, column)
WCDB_SYNTAX_UNION_MEMBER_IMPLEMENT(ExpressionUnionMember, firstMember, RaiseFunction, raiseFunction)
WCDB_SYNTAX_UNION_MEMBER_IMPLEMENT(ExpressionUnionMember, firstMember, WindowDef, windowDef)
WCDB_SYNTAX_UNION_MEMBER_IMPLEMENT(ExpressionUnionMember, firstMember, StringView, windowName)
WCDB_SYNTAX_UNION_MEMBER_IMPLEMENT(ExpressionUnionMember, secondMember, Shadow<SelectSTMT>, select)
WCDB_SYNTAX_UNION_MEMBER_IMPLEMENT(ExpressionUnionMember, secondMember, StringView, collation)
WCDB_SYNTAX_UNION_MEMBER_IMPLEMENT(ExpressionUnionMember, secondMember, Schema, schema)
WCDB_SYNTAX_UNION_MEMBER_IMPLEMENT(ExpressionUnionMember, secondMember, Filter, filter)
WCDB_SYNTAX_UNION_MEMBER_IMPLEMENT(ExpressionUnionMember, thirdMember, StringView, table)
WCDB_SYNTAX_UNION_MEMBER_IMPLEMENT(ExpressionUnionMember, thirdMember, StringView, function)

#pragma mark - Identifier
Expression::~Expression() = default;

Identifier::Type Expression::getType() const
{
    return type;
}

static void streamAutoParenthesesExpression(std::ostream& stream, const Expression& expression)
{
    bool parentheses;
    switch (expression.switcher) {
    case Expression::Switch::LiteralValue:
    case Expression::Switch::BindParameter:
    case Expression::Switch::Column:
    case Expression::Switch::Function:
    case Expression::Switch::Cast:
    case Expression::Switch::In:
    case Expression::Switch::Exists:
    case Expression::Switch::Case:
    case Expression::Switch::RaiseFunction:
    case Expression::Switch::Window:
    case Expression::Switch::Expressions:
    case Expression::Switch::Select:
        parentheses = false;
        break;
    case Expression::Switch::UnaryOperation:
    case Expression::Switch::BinaryOperation:
    case Expression::Switch::Collate:
    case Expression::Switch::Between:
        parentheses = true;
        break;
    }
    if (parentheses) {
        stream << "(";
    }
    stream << expression;
    if (parentheses) {
        stream << ")";
    }
}

bool Expression::describle(std::ostream& stream) const
{
    switch (switcher) {
    case Switch::LiteralValue:
        stream << literalValue();
        break;
    case Switch::BindParameter:
        stream << bindParameter();
        break;
    case Switch::Column:
        stream << column();
        break;
    case Switch::UnaryOperation: {
        WCTSyntaxRemedialAssert(expressions.size() == 1);
        switch (unaryOperator) {
        case UnaryOperator::Null:
            streamAutoParenthesesExpression(stream, *expressions.begin());
            stream << space;
            if (isNot) {
                stream << "NOT";
            } else {
                stream << "IS";
            }
            stream << unaryOperator;
            break;
        case UnaryOperator::Not:
            stream << unaryOperator << space;
            streamAutoParenthesesExpression(stream, *expressions.begin());
            break;
        case UnaryOperator::Positive:
        case UnaryOperator::Negative:
        case UnaryOperator::Tilde:
            stream << unaryOperator;
            streamAutoParenthesesExpression(stream, *expressions.begin());
            break;
        }
        break;
    }
    case Switch::BinaryOperation: {
        switch (binaryOperator) {
        case Syntax::Expression::BinaryOperator::Concatenate:
        case Syntax::Expression::BinaryOperator::Multiply:
        case Syntax::Expression::BinaryOperator::Divide:
        case Syntax::Expression::BinaryOperator::Modulo:
        case Syntax::Expression::BinaryOperator::Plus:
        case Syntax::Expression::BinaryOperator::Minus:
        case Syntax::Expression::BinaryOperator::LeftShift:
        case Syntax::Expression::BinaryOperator::RightShift:
        case Syntax::Expression::BinaryOperator::BitwiseAnd:
        case Syntax::Expression::BinaryOperator::BitwiseOr:
        case Syntax::Expression::BinaryOperator::Less:
        case Syntax::Expression::BinaryOperator::LessOrEqual:
        case Syntax::Expression::BinaryOperator::Greater:
        case Syntax::Expression::BinaryOperator::GreaterOrEqual:
        case Syntax::Expression::BinaryOperator::Equal:
        case Syntax::Expression::BinaryOperator::NotEqual:
        case Syntax::Expression::BinaryOperator::And:
        case Syntax::Expression::BinaryOperator::Or:
            WCTSyntaxRemedialAssert(expressions.size() == 2);
            streamAutoParenthesesExpression(stream, *expressions.begin());
            stream << space << binaryOperator << space;
            streamAutoParenthesesExpression(stream, *(++expressions.begin()));
            break;
        case Syntax::Expression::BinaryOperator::Is:
            WCTSyntaxRemedialAssert(expressions.size() == 2);
            // Extra parentheses to ensure the correctness of operator precedence
            streamAutoParenthesesExpression(stream, *expressions.begin());
            stream << space << binaryOperator;
            if (isNot) {
                stream << " NOT";
            }
            stream << space;
            streamAutoParenthesesExpression(stream, *(++expressions.begin()));
            break;
        case Syntax::Expression::BinaryOperator::Like:
        case Syntax::Expression::BinaryOperator::GLOB:
        case Syntax::Expression::BinaryOperator::RegExp:
        case Syntax::Expression::BinaryOperator::Match: {
            WCTSyntaxRemedialAssert(expressions.size() == 2 + escape);
            auto iter = expressions.begin();
            streamAutoParenthesesExpression(stream, *iter);
            if (isNot) {
                stream << " NOT";
            }
            stream << space << binaryOperator << space;
            streamAutoParenthesesExpression(stream, *(++iter));
            if (escape) {
                stream << " ESCAPE ";
                streamAutoParenthesesExpression(stream, *(++iter));
            }
            break;
        }
        }
    } break;
    case Switch::Function:
        stream << function() << "(";
        if (!expressions.empty()) {
            if (distinct) {
                stream << "DISTINCT ";
            }
            stream << expressions;
        } else if (useWildcard) {
            stream << "*";
        }
        stream << ")";
        break;
    case Switch::Expressions:
        stream << "(" << expressions << ")";
        break;
    case Switch::Cast:
        WCTSyntaxRemedialAssert(expressions.size() == 1);
        stream << "CAST(" << *expressions.begin() << " AS " << castType << ")";
        break;
    case Switch::Collate:
        WCTSyntaxRemedialAssert(expressions.size() == 1);
        streamAutoParenthesesExpression(stream, *expressions.begin());
        stream << " COLLATE " << collation();
        break;
    case Switch::Between: {
        WCTSyntaxRemedialAssert(expressions.size() == 3);
        auto iter = expressions.begin();
        streamAutoParenthesesExpression(stream, *iter);
        if (isNot) {
            stream << " NOT";
        }
        stream << " BETWEEN ";
        streamAutoParenthesesExpression(stream, *++iter);
        stream << " AND ";
        streamAutoParenthesesExpression(stream, *++iter);
        break;
    }
    case Switch::In: {
        WCTSyntaxRemedialAssert(expressions.size() >= 1);
        auto iter = expressions.begin();
        stream << *iter;
        if (isNot) {
            stream << " NOT";
        }
        stream << " IN";
        switch (inSwitcher) {
        case SwitchIn::Empty:
            stream << "()";
            break;
        case SwitchIn::Select:
            WCTSyntaxRemedialAssert(select() != nullptr);
            stream << "(" << *select().get() << ")";
            break;
        case SwitchIn::Expressions: {
            stream << "(";
            bool comma = false;
            while (++iter != expressions.end()) {
                if (comma) {
                    stream << ", ";
                } else {
                    comma = true;
                }
                stream << *iter;
            }
            stream << ")";
            break;
        }
        case SwitchIn::Table:
            stream << space;
            if (!schema().empty()) {
                stream << schema() << ".";
            }
            stream << table();
            break;
        case SwitchIn::Function: {
            stream << space;
            if (!schema().empty()) {
                stream << schema() << ".";
            }
            stream << function() << "(";
            bool comma = false;
            while (++iter != expressions.end()) {
                if (comma) {
                    stream << ", ";
                } else {
                    comma = true;
                }
                stream << *iter;
            }
            stream << ")";
            break;
        }
        }
        break;
    }
    case Switch::Exists:
        WCTSyntaxRemedialAssert(select() != nullptr);
        if (isNot) {
            stream << "NOT ";
        }
        stream << "EXISTS(" << *select().get() << ")";
        break;
    case Switch::Select:
        WCTSyntaxRemedialAssert(select() != nullptr);
        stream << "(" << *select().get() << ")";
        break;
    case Switch::Case: {
        WCTSyntaxRemedialAssert(expressions.size() >= hasCase + 2 + hasElse);
        WCTSyntaxRemedialAssert((expressions.size() - hasCase - hasElse) % 2 == 0);
        auto iter = expressions.begin();
        stream << "CASE ";
        if (hasCase) {
            stream << *iter << space;
            ++iter;
        }
        do {
            stream << "WHEN " << *iter;
            ++iter;
            stream << " THEN " << *iter << space;
            ++iter;
        } while (std::distance(iter, expressions.end()) > hasElse);
        if (hasElse) {
            stream << "ELSE " << *iter << space;
        }
        stream << "END";
        break;
    }
    case Switch::RaiseFunction:
        stream << raiseFunction();
        break;
    case Switch::Window:
        stream << function() << "(";
        if (!expressions.empty()) {
            stream << expressions;
        } else if (useWildcard) {
            stream << "*";
        }
        stream << ")";
        if (filter().isValid()) {
            stream << space << filter();
        }
        if (m_firstMember == Member::windowDef && windowDef().isValid()) {
            stream << " OVER";
            stream << windowDef();
        } else if (m_firstMember == Member::windowName && !windowName().empty()) {
            stream << " OVER";
            stream << space << windowName();
        }
        break;
    }
    return true;
}

void Expression::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, true, stop);
    switch (switcher) {
    // one expression
    case Switch::UnaryOperation:
    case Switch::Collate:
        WCTIterateRemedialAssert(expressions.size() == 1);
        expressions.begin()->iterate(iterator, stop);
        break;

    // two expressions
    case Switch::BinaryOperation:
        switch (binaryOperator) {
        case Syntax::Expression::BinaryOperator::Concatenate:
        case Syntax::Expression::BinaryOperator::Multiply:
        case Syntax::Expression::BinaryOperator::Divide:
        case Syntax::Expression::BinaryOperator::Modulo:
        case Syntax::Expression::BinaryOperator::Plus:
        case Syntax::Expression::BinaryOperator::Minus:
        case Syntax::Expression::BinaryOperator::LeftShift:
        case Syntax::Expression::BinaryOperator::RightShift:
        case Syntax::Expression::BinaryOperator::BitwiseAnd:
        case Syntax::Expression::BinaryOperator::BitwiseOr:
        case Syntax::Expression::BinaryOperator::Less:
        case Syntax::Expression::BinaryOperator::LessOrEqual:
        case Syntax::Expression::BinaryOperator::Greater:
        case Syntax::Expression::BinaryOperator::GreaterOrEqual:
        case Syntax::Expression::BinaryOperator::Equal:
        case Syntax::Expression::BinaryOperator::NotEqual:
        case Syntax::Expression::BinaryOperator::Is:
        case Syntax::Expression::BinaryOperator::And:
        case Syntax::Expression::BinaryOperator::Or:
            WCTIterateRemedialAssert(expressions.size() == 2);
            listIterate(expressions, iterator, stop);
            break;
        case Syntax::Expression::BinaryOperator::Like:
        case Syntax::Expression::BinaryOperator::GLOB:
        case Syntax::Expression::BinaryOperator::RegExp:
        case Syntax::Expression::BinaryOperator::Match: {
            WCTIterateRemedialAssert(expressions.size() == 2 + escape);
            listIterate(expressions, iterator, stop);
            break;
        }
        }
        break;
    // three expressions
    case Switch::Between: {
        WCTIterateRemedialAssert(expressions.size() == 3);
        listIterate(expressions, iterator, stop);
        break;
    }
    // expression list
    case Switch::Function:
    case Switch::Expressions:
    case Switch::Cast:
    case Switch::Case:
        listIterate(expressions, iterator, stop);
        break;
    // others
    case Switch::LiteralValue:
        recursiveIterate(literalValue(), iterator, stop);
        break;
    case Switch::BindParameter:
        recursiveIterate(bindParameter(), iterator, stop);
        break;
    case Switch::Column:
        recursiveIterate(column(), iterator, stop);
        break;
    case Switch::In: {
        WCTIterateRemedialAssert(expressions.size() >= 1);
        auto iter = expressions.begin();
        iter->iterate(iterator, stop);
        switch (inSwitcher) {
        case SwitchIn::Empty:
            break;
        case SwitchIn::Select:
            WCTIterateRemedialAssert(select() != nullptr);
            select().get()->iterate(iterator, stop);
            break;
        case SwitchIn::Expressions: {
            while (++iter != expressions.end()) {
                iter->iterate(iterator, stop);
            }
            break;
        }
        case SwitchIn::Table:
            recursiveIterate(schema(), iterator, stop);
            break;
        case SwitchIn::Function: {
            recursiveIterate(schema(), iterator, stop);
            while (++iter != expressions.end()) {
                iter->iterate(iterator, stop);
            }
            break;
        }
        }
        break;
    }
    case Switch::Exists:
        WCTIterateRemedialAssert(select() != nullptr);
        select().get()->iterate(iterator, stop);
        break;
    case Switch::Select:
        WCTIterateRemedialAssert(select() != nullptr);
        select().get()->iterate(iterator, stop);
        break;
    case Switch::RaiseFunction:
        recursiveIterate(raiseFunction(), iterator, stop);
        break;
    case Switch::Window:
        listIterate(expressions, iterator, stop);
        if (filter().isValid()) {
            recursiveIterate(filter(), iterator, stop);
        }
        if (m_firstMember == Member::windowDef) {
            recursiveIterate(windowDef(), iterator, stop);
        }
        break;
    }
    Identifier::iterate(iterator, false, stop);
}

} // namespace Syntax

} // namespace WCDB
