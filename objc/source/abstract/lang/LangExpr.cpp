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

#include <WINQ/lang.h>

namespace WCDB {

namespace lang {

copy_on_write_string ExprColumn::SQL() const
{
    std::string description;
    if (!schemaName.empty()) {
        description.append(schemaName.get() + ".");
    }
    if (!tableName.empty()) {
        description.append(tableName.get() + ".");
    }
    assert(!columnName.empty());
    description.append(columnName.get());
    return description;
}

copy_on_write_string ExprUnaryOperation::SQL() const
{
    assert(!expr.empty());
    return ExprUnaryOperation::OperatorName(unaryOperator) +
           expr.description().get();
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

copy_on_write_string ExprBinaryOperation::SQL() const
{
    assert(!left.empty());
    assert(!right.empty());
    return left.description().get() +
           ExprBinaryOperation::OperatorName(binaryOperator) +
           right.description().get();
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

copy_on_write_string ExprFunction::SQL() const
{
    std::string description;
    assert(!functionName.empty());
    description.append(functionName.get() + "(");
    switch (type) {
        case Type::DistinctExpr:
            description.append("DISTINCT ");
        // fallthrough
        case Type::Expr:
            assert(!exprs.empty());
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

copy_on_write_string ExprList::SQL() const
{
    assert(!exprs.empty());
    return "(" + exprs.description().get() + ")";
}

copy_on_write_string ExprCast::SQL() const
{
    std::string description("CAST(");
    assert(!expr.empty());
    description.append(expr.description().get());
    description.append(" AS ");
    description.append(ColumnTypeName(type));
    description.append(")");
    return description;
}

copy_on_write_string ExprCollate::SQL() const
{
    return expr.description().get() + " COLLATE " + collationName.get();
}

copy_on_write_string ExprPattern::SQL() const
{
    std::string description;
    assert(!left.empty());
    description.append(left.description().get());
    if (isNot) {
        description.append(" NOT ");
    }
    description.append(ExprPattern::TypeName(type));
    assert(!right.empty());
    description.append(" " + right.description().get());
    if (!escape.empty()) {
        description.append(" ESCAPE " + escape.description().get());
    }
    return description;
}

constexpr const char *ExprPattern::TypeName(const Type &pattern)
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

copy_on_write_string ExprNull::SQL() const
{
    assert(!expr.empty());
    if (isNull) {
        return expr.description().get() + " ISNULL";
    } else {
        return expr.description().get() + " NOTNULL";
    }
}

copy_on_write_string ExprBetween::SQL() const
{
    std::string description;
    assert(!expr.empty());
    description.append(expr.description().get());
    if (isNot) {
        description.append(" NOT");
    }
    assert(!left.empty() && !right.empty());
    description.append(" BETWEEN " + left.description().get() + " AND " +
                       right.description().get());
    return description;
}

copy_on_write_string ExprIn::SQL() const
{
    std::string description;
    assert(!expr.empty());
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
            assert(!selectSTMT.empty());
            description.append("(" + selectSTMT.description().get() + ")");
            break;
        case Switch::Expr:
            assert(!exprs.empty());
            description.append("(" + exprs.description().get() + ")");
            break;
        case Switch::TableOrFunction:
            if (!schemaName.empty()) {
                description.append(schemaName.get() + ".");
            }
            assert(!tableNameOrFunction.empty());
            description.append(tableNameOrFunction.get());
            if (!exprs.empty()) {
                description.append(exprs.description().get());
            }
            description.append(")");
    }
    return description;
}

copy_on_write_string ExprExists::SQL() const
{
    std::string description;
    if (exists) {
        if (isNot) {
            description.append("NOT ");
        }
        description.append("EXISTS ");
    }
    assert(!selectSTMT.empty());
    description.append("(" + selectSTMT.description().get() + ")");
    return description;
}

copy_on_write_string ExprCase::SQL() const
{
    std::string description("CASE ");
    if (!exprCase.empty()) {
        description.append(exprCase.description().get() + " ");
    }
    assert(!pairs.empty());
    description.append(pairs.description().get());
    if (!exprElse.empty()) {
        description.append(" ELSE " + exprElse.description().get());
    }
    description.append(" END");
    return description;
}

copy_on_write_string ExprCase::Pair::SQL() const
{
    return "WHEN " + when.description().get() + " THEN " +
           then.description().get();
}

template <>
copy_on_write_string
copy_on_write_lazy_lang_list<ExprCase::Pair>::calculatedDescription() const
{
    std::string description;
    bool comma = false;
    for (const auto &element : this->get()) {
        if (comma) {
            description.append(" ");
        } else {
            comma = true;
        }
        assert(!element.empty());
        description.append(element.description().get());
    }
    return description;
}

copy_on_write_string Expr::SQL() const
{
    switch (type) {
        case Type::LiteralValue:
            assert(!literalValue.empty());
            return literalValue.description();
        case Type::BindParameter:
            assert(!bindParamter.empty());
            return bindParamter.description();
        case Type::Column:
            assert(!exprColumn.empty());
            return exprColumn.description();
        case Type::UnaryOperator:
            assert(!exprUnaryOperator.empty());
            return exprUnaryOperator.description();
        case Type::BinaryOperator:
            assert(!exprBinaryOperator.empty());
            return exprBinaryOperator.description();
        case Type::Function:
            assert(!exprFunction.empty());
            return exprFunction.description();
        case Type::List:
            assert(!exprList.empty());
            return exprList.description();
        case Type::Cast:
            assert(!exprCast.empty());
            return exprCast.description();
        case Type::Collate:
            assert(!exprCollate.empty());
            return exprCollate.description();
        case Type::Pattern:
            assert(!exprPattern.empty());
            return exprPattern.description();
        case Type::Null:
            assert(!exprNull.empty());
            return exprNull.description();
        case Type::Between:
            assert(!exprBetween.empty());
            return exprBetween.description();
        case Type::In:
            assert(!exprIn.empty());
            return exprIn.description();
        case Type::Exists:
            assert(!exprExists.empty());
            return exprExists.description();
        case Type::Case:
            assert(!exprCase.empty());
            return exprCase.description();
        case Type::RaiseFunction:
            assert(!raiseFunction.empty());
            return raiseFunction.description();
    }
}

} // namespace lang

} // namespace WCDB
