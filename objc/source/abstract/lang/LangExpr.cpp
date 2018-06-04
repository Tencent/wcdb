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

#include <WCDB/Lang.h>
#include <WCDB/String.hpp>

namespace WCDB {

namespace Lang {

CopyOnWriteString ExprColumn::SQL() const
{
    std::string description;
    if (!tableName.empty()) {
        //adding main schema for empty schema is unnecessary for ExprColumn.
        if (!schemaName.empty()) {
            description.append(schemaName.get() + ".");
        }
        description.append(tableName.get() + ".");
    }
    LangRemedialAssert(!column.empty());
    description.append(column.description().get());
    return description;
}

ExprBase::Type ExprColumn::getType() const
{
    return ExprBase::Type::Column;
}

CopyOnWriteString ExprUnaryOperation::SQL() const
{
    std::string description("(");
    description.append(ExprUnaryOperation::OperatorName(unaryOperator));
    LangRemedialAssert(!expr.empty());
    description.append(expr.description().get());
    description.append(")");
    return description;
}

ExprBase::Type ExprUnaryOperation::getType() const
{
    return ExprBase::Type::UnaryOperator;
}

constexpr const char *
ExprUnaryOperation::OperatorName(const Operator &unaryOpeartor)
{
    switch (unaryOpeartor) {
        case Operator::Negative:
            return "-";
        case Operator::Positive:
            return "+";
        case Operator::Tilde:
            return "~";
        case Operator::Not:
            return "NOT ";
    }
}

CopyOnWriteString ExprBinaryOperation::SQL() const
{
    std::string description("(");
    LangRemedialAssert(!left.empty());
    description.append(left.description().get());
    description.append(ExprBinaryOperation::OperatorName(binaryOperator));
    LangRemedialAssert(!right.empty());
    description.append(right.description().get());
    description.append(")");
    return description;
}

ExprBase::Type ExprBinaryOperation::getType() const
{
    return ExprBase::Type::BinaryOperator;
}

constexpr const char *
ExprBinaryOperation::OperatorName(const Operator &binaryOpeartor)
{
    switch (binaryOpeartor) {
        case Operator::Concatenate:
            return " || ";
        case Operator::Multiply:
            return " * ";
        case Operator::Divide:
            return " / ";
        case Operator::Modulo:
            return " % ";
        case Operator::Plus:
            return " + ";
        case Operator::Minus:
            return " - ";
        case Operator::LeftShift:
            return " << ";
        case Operator::RightShift:
            return " >> ";
        case Operator::BitwiseAnd:
            return " & ";
        case Operator::BitwiseOr:
            return " | ";
        case Operator::Less:
            return " < ";
        case Operator::LessOrEqual:
            return " <= ";
        case Operator::Greater:
            return " > ";
        case Operator::GreaterOrEqual:
            return " >= ";
        case Operator::Equal:
            return " == ";
        case Operator::NotEqual:
            return " != ";
        case Operator::Is:
            return " IS ";
        case Operator::IsNot:
            return " IS NOT ";
        case Operator::And:
            return " AND ";
        case Operator::Or:
            return " OR ";
    }
}

CopyOnWriteString ExprFunction::SQL() const
{
    std::string description;
    LangRemedialAssert(!functionName.empty());
    description.append(functionName.get() + "(");
    switch (type) {
        case Type::DistinctExpr:
            description.append("DISTINCT ");
        // fallthrough
        case Type::Expr:
            LangRemedialAssert(!exprs.empty());
            description.append(exprs.description().get());
            break;
        case Type::Star:
            description.append("*");
            break;
        default:
            break;
    }
    description.append(")");
    return description;
}

ExprBase::Type ExprFunction::getType() const
{
    return ExprBase::Type::Function;
}

CopyOnWriteString ExprList::SQL() const
{
    std::string description("(");
    LangRemedialAssert(!exprs.empty());
    description.append(exprs.description().get());
    description.append(")");
    return description;
}

ExprBase::Type ExprList::getType() const
{
    return ExprBase::Type::List;
}

CopyOnWriteString ExprCast::SQL() const
{
    std::string description("CAST(");
    LangRemedialAssert(!expr.empty());
    description.append(expr.description().get());
    description.append(" AS ");
    description.append(ColumnTypeName(type));
    description.append(")");
    return description;
}

ExprBase::Type ExprCast::getType() const
{
    return ExprBase::Type::Cast;
}

CopyOnWriteString ExprCollate::SQL() const
{
    return expr.description().get() + " COLLATE " + collationName.get();
}

ExprBase::Type ExprCollate::getType() const
{
    return ExprBase::Type::Collate;
}

CopyOnWriteString ExprPattern::SQL() const
{
    std::string description;
    LangRemedialAssert(!left.empty());
    description.append(left.description().get() + " ");
    if (isNot) {
        description.append("NOT ");
    }
    description.append(ExprPattern::typeName(type));
    LangRemedialAssert(!right.empty());
    description.append(" " + right.description().get());
    if (!escape.empty()) {
        description.append(" ESCAPE " + escape.description().get());
    }
    return description;
}

ExprBase::Type ExprPattern::getType() const
{
    return ExprBase::Type::Pattern;
}

constexpr const char *ExprPattern::typeName(const Type &pattern)
{
    switch (pattern) {
        case Type::Like:
            return "LIKE";
        case Type::Glob:
            return "GLOB";
        case Type::Regexp:
            return "REGEXP";
        case Type::Match:
            return "MATCH";
    }
}

CopyOnWriteString ExprNull::SQL() const
{
    std::string description;
    LangRemedialAssert(!expr.empty());
    description.append(expr.description().get());
    if (isNull) {
        description.append(" ISNULL");
    } else {
        description.append(" NOTNULL");
    }
    return description;
}

ExprBase::Type ExprNull::getType() const
{
    return ExprBase::Type::Null;
}

CopyOnWriteString ExprBetween::SQL() const
{
    std::string description;
    LangRemedialAssert(!expr.empty());
    description.append(expr.description().get());
    if (isNot) {
        description.append(" NOT");
    }
    LangRemedialAssert(!left.empty() && !right.empty());
    description.append(" BETWEEN " + left.description().get() + " AND " +
                       right.description().get());
    return description;
}

ExprBase::Type ExprBetween::getType() const
{
    return ExprBase::Type::Between;
}

CopyOnWriteString ExprIn::SQL() const
{
    std::string description;
    LangRemedialAssert(!expr.empty());
    description.append(expr.description().get());
    if (isNot) {
        description.append(" NOT");
    }
    description.append(" IN");
    switch (switcher) {
        case Switch::NotSet:
            description.append("()");
            break;
        case Switch::Select:
            LangRemedialAssert(!selectSTMT.empty());
            description.append("(" + selectSTMT.description().get() + ")");
            break;
        case Switch::Expr:
            LangRemedialAssert(!exprs.empty());
            description.append("(" + exprs.description().get() + ")");
            break;
        case Switch::Table:
            description.append(" ");
            if (schemaName.isNull()) {
                description.append(mainSchema() + ".");
            } else if (!schemaName.get().empty()) {
                description.append(schemaName.get() + ".");
            }
            LangRemedialAssert(!tableNameOrFunction.empty());
            description.append(tableNameOrFunction.get());
            break;
        case Switch::Function:
            description.append(" ");
            if (schemaName.isNull()) {
                description.append(mainSchema() + ".");
            } else if (!schemaName.get().empty()) {
                description.append(schemaName.get() + ".");
            }
            LangRemedialAssert(!tableNameOrFunction.empty());
            description.append(tableNameOrFunction.get() + "(");
            if (!exprs.empty()) {
                description.append(exprs.description().get());
            }
            description.append(")");
            break;
    }
    return description;
}

ExprBase::Type ExprIn::getType() const
{
    return ExprBase::Type::In;
}

CopyOnWriteString ExprExists::SQL() const
{
    std::string description;
    if (isNot) {
        description.append("NOT");
    }
    if (exists) {
        if (isNot) {
            description.append(" ");
        }
        description.append("EXISTS");
    }
    LangRemedialAssert(!selectSTMT.empty());
    description.append("(" + selectSTMT.description().get() + ")");
    return description;
}

ExprBase::Type ExprExists::getType() const
{
    return ExprBase::Type::Exists;
}

CopyOnWriteString ExprCase::SQL() const
{
    std::string description("CASE ");
    if (!exprCase.empty()) {
        description.append(exprCase.description().get() + " ");
    }
    LangRemedialAssert(!pairs.empty());
    description.append(pairs.description().get());
    if (!exprElse.empty()) {
        description.append(" ELSE " + exprElse.description().get());
    }
    description.append(" END");
    return description;
}

ExprBase::Type ExprCase::getType() const
{
    return ExprBase::Type::Case;
}

CopyOnWriteString ExprCase::Pair::SQL() const
{
    return "WHEN " + when.description().get() + " THEN " +
           then.description().get();
}

template <>
CopyOnWriteString
CopyOnWriteLazyLangList<ExprCase::Pair>::calculatedDescription() const
{
    std::string description;
    bool space = false;
    for (const auto &element : this->get()) {
        if (space) {
            description.append(" ");
        } else {
            space = true;
        }
        LangRemedialAssert(!element.empty());
        description.append(element.description().get());
    }
    return description;
}

Expr::Expr() : type(Type::NotSet)
{
}

CopyOnWriteString Expr::SQL() const
{
    std::string description = String::empty();
    switch (type) {
        case Type::LiteralValue:
            LangRemedialAssert(!literalValue.empty());
            return literalValue.description();
        case Type::BindParameter:
            LangRemedialAssert(!bindParamter.empty());
            return bindParamter.description();
        case Type::Column:
            LangRemedialAssert(!exprColumn.empty());
            return exprColumn.description();
        case Type::UnaryOperator:
            LangRemedialAssert(!exprUnaryOperator.empty());
            return exprUnaryOperator.description();
        case Type::BinaryOperator:
            LangRemedialAssert(!exprBinaryOperator.empty());
            return exprBinaryOperator.description();
        case Type::Function:
            LangRemedialAssert(!exprFunction.empty());
            return exprFunction.description();
        case Type::List:
            LangRemedialAssert(!exprList.empty());
            return exprList.description();
        case Type::Cast:
            LangRemedialAssert(!exprCast.empty());
            return exprCast.description();
        case Type::Collate:
            LangRemedialAssert(!exprCollate.empty());
            return exprCollate.description();
        case Type::Pattern:
            LangRemedialAssert(!exprPattern.empty());
            return exprPattern.description();
        case Type::Null:
            LangRemedialAssert(!exprNull.empty());
            return exprNull.description();
        case Type::Between:
            LangRemedialAssert(!exprBetween.empty());
            return exprBetween.description();
        case Type::In:
            LangRemedialAssert(!exprIn.empty());
            return exprIn.description();
        case Type::Exists:
            LangRemedialAssert(!exprExists.empty());
            return exprExists.description();
        case Type::Case:
            LangRemedialAssert(!exprCase.empty());
            return exprCase.description();
        case Type::RaiseFunction:
            LangRemedialAssert(!raiseFunction.empty());
            return raiseFunction.description();
        default:
            LangRemedialFatalError();
    }
    return description;
}

ExprBase::Type Expr::getType() const
{
    return ExprBase::Type::Expr;
}

} // namespace Lang

} // namespace WCDB
