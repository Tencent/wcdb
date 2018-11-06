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

#include <WCDB/Assertion.hpp>
#include <WCDB/Enum.hpp>
#include <WCDB/Syntax.h>

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

#pragma mark - Identifier
Identifier::Type Expression::getType() const
{
    return type;
}

String Expression::getDescription() const
{
    std::ostringstream stream;
    switch (switcher) {
    case Switch::LiteralValue:
        stream << literalValue;
        break;
    case Switch::BindParameter:
        stream << bindParameter;
        break;
    case Switch::Column:
        if (!table.empty()) {
            stream << schema << "." << table << ".";
        }
        stream << column;
        break;
    case Switch::UnaryOperation:
        SyntaxRemedialAssert(expressions.size() == 1);
        switch (unaryOperator) {
        case UnaryOperator::Null:
            stream << "(" << *expressions.begin() << ") ";
            if (isNot) {
                stream << "NOT";
            } else {
                stream << "IS";
            }
            stream << unaryOperator;
            break;
        case UnaryOperator::Not:
            stream << unaryOperator << " (" << *expressions.begin() << ")";
            break;
        case UnaryOperator::Positive:
        case UnaryOperator::Negative:
        case UnaryOperator::Tilde:
            stream << unaryOperator << "(" << *expressions.begin() << ")";
            break;
        }
        break;
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
        case Syntax::Expression::BinaryOperator::And:
        case Syntax::Expression::BinaryOperator::Or:
            SyntaxRemedialAssert(expressions.size() == 2);
            // Extra parentheses to ensure the correctness of operator precedence
            stream << "(" << *expressions.begin() << ") " << binaryOperator
                   << " (" << *(++expressions.begin()) << ")";
            break;
        case Syntax::Expression::BinaryOperator::Is:
            SyntaxRemedialAssert(expressions.size() == 2);
            // Extra parentheses to ensure the correctness of operator precedence
            stream << "(" << *expressions.begin() << ") " << binaryOperator;
            if (isNot) {
                stream << " NOT";
            }
            stream << " (" << *(++expressions.begin()) << ")";
            break;
        case Syntax::Expression::BinaryOperator::Like:
        case Syntax::Expression::BinaryOperator::GLOB:
        case Syntax::Expression::BinaryOperator::RegExp:
        case Syntax::Expression::BinaryOperator::Match: {
            SyntaxRemedialAssert(expressions.size() == 2 + escape);
            auto iter = expressions.begin();
            stream << "(" << *iter << ")";
            if (isNot) {
                stream << " NOT";
            }
            stream << space << binaryOperator << space << "(" << *(++iter) << ")";
            if (escape) {
                stream << " ESCAPE (" << *(++iter) << ")";
            }
            break;
        }
        }
        break;
    case Switch::Function:
        stream << function << "(";
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
        SyntaxRemedialAssert(expressions.size() == 1);
        stream << "CAST(" << *expressions.begin() << " AS " << castType << ")";
        break;
    case Switch::Collate:
        SyntaxRemedialAssert(expressions.size() == 1);
        stream << *expressions.begin() << " COLLATE " << collation;
        break;
    case Switch::Between: {
        SyntaxRemedialAssert(expressions.size() == 3);
        auto iter = expressions.begin();
        stream << *iter;
        if (isNot) {
            stream << " NOT";
        }
        stream << " BETWEEN " << *++iter << " AND " << *++iter;
        break;
    }
    case Switch::In: {
        SyntaxRemedialAssert(expressions.size() >= 1);
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
            SyntaxRemedialAssert(select != nullptr);
            stream << "(" << *select.get() << ")";
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
            stream << space << schema << "." << table;
            break;
        case SwitchIn::Function: {
            stream << space << schema << "." << function << "(";
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
        SyntaxRemedialAssert(select != nullptr);
        if (isNot) {
            stream << "NOT ";
        }
        stream << "EXISTS(" << *select.get() << ")";
        break;
    case Switch::Case: {
        SyntaxRemedialAssert(expressions.size() >= hasCase + 2 + hasElse);
        SyntaxRemedialAssert((expressions.size() - hasCase - hasElse) % 2 == 0);
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
        stream << raiseFunction;
        break;
    case Switch::Window:
        stream << function << "(";
        if (!expressions.empty()) {
            stream << expressions;
        } else if (useWildcard) {
            stream << "*";
        }
        stream << ")";
        if (useFilter) {
            stream << space << filter;
        }
        stream << " OVER";
        if (windowName.empty()) {
            stream << "(" << windowDef << ")";
        } else {
            stream << space << windowName;
        }
        break;
    }
    return stream.str();
}

void Expression::iterate(const Iterator& iterator, void* parameter)
{
    Identifier::iterate(iterator, parameter);
    switch (switcher) {
    // one expression
    case Switch::UnaryOperation:
    case Switch::Collate:
        IterateRemedialAssert(expressions.size() == 1);
        expressions.begin()->iterate(iterator, parameter);
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
            IterateRemedialAssert(expressions.size() == 2);
            listIterate(expressions, iterator, parameter);
            break;
        case Syntax::Expression::BinaryOperator::Like:
        case Syntax::Expression::BinaryOperator::GLOB:
        case Syntax::Expression::BinaryOperator::RegExp:
        case Syntax::Expression::BinaryOperator::Match: {
            IterateRemedialAssert(expressions.size() == 2 + escape);
            listIterate(expressions, iterator, parameter);
            break;
        }
        }
        break;
    // three expressions
    case Switch::Between: {
        IterateRemedialAssert(expressions.size() == 3);
        listIterate(expressions, iterator, parameter);
        break;
    }
    // expression list
    case Switch::Function:
    case Switch::Expressions:
    case Switch::Cast:
    case Switch::Case:
        listIterate(expressions, iterator, parameter);
        break;
    // others
    case Switch::LiteralValue:
        literalValue.iterate(iterator, parameter);
        break;
    case Switch::BindParameter:
        bindParameter.iterate(iterator, parameter);
        break;
    case Switch::Column:
        if (!table.empty()) {
            schema.iterate(iterator, parameter);
        }
        column.iterate(iterator, parameter);
        break;
    case Switch::In: {
        IterateRemedialAssert(expressions.size() >= 1);
        auto iter = expressions.begin();
        iter->iterate(iterator, parameter);
        switch (inSwitcher) {
        case SwitchIn::Empty:
            break;
        case SwitchIn::Select:
            IterateRemedialAssert(select != nullptr);
            select->iterate(iterator, parameter);
            break;
        case SwitchIn::Expressions: {
            while (++iter != expressions.end()) {
                iter->iterate(iterator, parameter);
            }
            break;
        }
        case SwitchIn::Table:
            schema.iterate(iterator, parameter);
            break;
        case SwitchIn::Function: {
            schema.iterate(iterator, parameter);
            while (++iter != expressions.end()) {
                iter->iterate(iterator, parameter);
            }
            break;
        }
        }
        break;
    }
    case Switch::Exists:
        IterateRemedialAssert(select != nullptr);
        select->iterate(iterator, parameter);
        break;
    case Switch::RaiseFunction:
        raiseFunction.iterate(iterator, parameter);
        break;
    case Switch::Window:
        listIterate(expressions, iterator, parameter);
        if (useFilter) {
            filter.iterate(iterator, parameter);
        }
        if (windowName.empty()) {
            windowDef.iterate(iterator, parameter);
        }
        break;
    }
}

} // namespace Syntax

} // namespace WCDB
