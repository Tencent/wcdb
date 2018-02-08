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

#include <WINQ/WINQ.h>

namespace WCDB {

Expression::Expression(const LiteralValue &literalValue)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::LiteralValue;
    lang.literalValue.assign(literalValue.getLang());
}

Expression::Expression(const BindParameter &bindParameter)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::BindParameter;
    lang.bindParamter.assign(bindParameter.getLang());
}

Expression::Expression(const Expression::Column &expressionColumn)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::Column;
    lang.exprColumn.assign(expressionColumn.getLang());
}

Expression::Column Expression::ColumnNamed(const std::string &columnName)
{
    return Expression::Column(columnName);
}

Expression::Column::Column(const std::string &columnName)
{
    lang::ExprColumn &lang = getMutableLang();
    lang.columnName.assign(columnName);
}

Expression::Column &Expression::Column::withTable(const std::string &tableName)
{
    lang::ExprColumn &lang = getMutableLang();
    lang.tableName.assign(tableName);
    return *this;
}

Expression::Column &
Expression::Column::withSchema(const std::string &schemaName)
{
    lang::ExprColumn &lang = getMutableLang();
    lang.schemaName.assign(schemaName);
    return *this;
}

Expression::Expression(
    const lang::copy_on_write_lazy_lang<lang::ExprUnaryOperation>
        &exprUnaryOperation)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::UnaryOperator;
    lang.exprUnaryOperator.assign(exprUnaryOperation);
}

lang::copy_on_write_lazy_lang<lang::ExprUnaryOperation>
Expression::unaryOperation(
    const lang::ExprUnaryOperation::Operator &unaryOperator) const
{
    lang::copy_on_write_lazy_lang<lang::ExprUnaryOperation> cowLang;
    lang::ExprUnaryOperation &lang = cowLang.get_or_copy();
    lang.expr.assign(getLang());
    lang.unaryOperator = unaryOperator;
    return cowLang;
}

Expression Expression::operator-() const
{
    return unaryOperation(lang::ExprUnaryOperation::Operator::Negative);
}

Expression Expression::operator+() const
{
    return unaryOperation(lang::ExprUnaryOperation::Operator::Positive);
}

Expression Expression::operator!() const
{
    return unaryOperation(lang::ExprUnaryOperation::Operator::Not);
}

Expression Expression::operator~() const
{
    return unaryOperation(lang::ExprUnaryOperation::Operator::Tilde);
}

Expression::Expression(
    const lang::copy_on_write_lazy_lang<lang::ExprBinaryOperation>
        &exprBinaryOperation)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::BinaryOperator;
    lang.exprBinaryOperator.assign(exprBinaryOperation);
}

lang::copy_on_write_lazy_lang<lang::ExprBinaryOperation>
Expression::binaryOperation(
    const lang::ExprBinaryOperation::Operator &binaryOperator,
    const Expression &operand) const
{
    lang::copy_on_write_lazy_lang<lang::ExprBinaryOperation> cowLang;
    lang::ExprBinaryOperation &lang = cowLang.get_or_copy();
    lang.left.assign(getLang());
    lang.right.assign(operand.getLang());
    lang.binaryOperator = binaryOperator;
    return cowLang;
}

Expression Expression::concat(const Expression &operand) const
{
    return binaryOperation(lang::ExprBinaryOperation::Operator::Concatenate,
                           operand);
}

Expression Expression::operator*(const Expression &operand) const
{
    return binaryOperation(lang::ExprBinaryOperation::Operator::Multiply,
                           operand);
}

Expression Expression::operator/(const Expression &operand) const
{
    return binaryOperation(lang::ExprBinaryOperation::Operator::Divide,
                           operand);
}

Expression Expression::operator%(const Expression &operand) const
{
    return binaryOperation(lang::ExprBinaryOperation::Operator::Modulo,
                           operand);
}

Expression Expression::operator+(const Expression &operand) const
{
    return binaryOperation(lang::ExprBinaryOperation::Operator::Plus, operand);
}

Expression Expression::operator-(const Expression &operand) const
{
    return binaryOperation(lang::ExprBinaryOperation::Operator::Minus, operand);
}

Expression Expression::operator<<(const Expression &operand) const
{
    return binaryOperation(lang::ExprBinaryOperation::Operator::LeftShift,
                           operand);
}

Expression Expression::operator>>(const Expression &operand) const
{
    return binaryOperation(lang::ExprBinaryOperation::Operator::RightShift,
                           operand);
}

Expression Expression::operator&(const Expression &operand) const
{
    return binaryOperation(lang::ExprBinaryOperation::Operator::BitwiseAnd,
                           operand);
}

Expression Expression::operator|(const Expression &operand) const
{
    return binaryOperation(lang::ExprBinaryOperation::Operator::BitwiseOr,
                           operand);
}

Expression Expression::operator<(const Expression &operand) const
{
    return binaryOperation(lang::ExprBinaryOperation::Operator::Less, operand);
}

Expression Expression::operator<=(const Expression &operand) const
{
    return binaryOperation(lang::ExprBinaryOperation::Operator::LessOrEqual,
                           operand);
}

Expression Expression::operator>(const Expression &operand) const
{
    return binaryOperation(lang::ExprBinaryOperation::Operator::Greater,
                           operand);
}

Expression Expression::operator>=(const Expression &operand) const
{
    return binaryOperation(lang::ExprBinaryOperation::Operator::GreaterOrEqual,
                           operand);
}

Expression Expression::operator==(const Expression &operand) const
{
    return binaryOperation(lang::ExprBinaryOperation::Operator::Equal, operand);
}

Expression Expression::operator!=(const Expression &operand) const
{
    return binaryOperation(lang::ExprBinaryOperation::Operator::NotEqual,
                           operand);
}

Expression Expression::is(const Expression &operand) const
{
    return binaryOperation(lang::ExprBinaryOperation::Operator::Is, operand);
}

Expression Expression::isNot(const Expression &operand) const
{
    return binaryOperation(lang::ExprBinaryOperation::Operator::IsNot, operand);
}

Expression Expression::operator&&(const Expression &operand) const
{
    return binaryOperation(lang::ExprBinaryOperation::Operator::And, operand);
}

Expression Expression::operator||(const Expression &operand) const
{
    return binaryOperation(lang::ExprBinaryOperation::Operator::Or, operand);
}

Expression::Expression(
    const lang::copy_on_write_lazy_lang<lang::ExprFunction> &exprFunction)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::Function;
    lang.exprFunction.assign(exprFunction);
}

Expression Expression::Function(const std::string &functionName)
{
    lang::copy_on_write_lazy_lang<lang::ExprFunction> cowLang;
    lang::ExprFunction &lang = cowLang.get_or_copy();
    lang.functionName.assign(functionName);
    return cowLang;
}

Expression Expression::Function(const std::string &functionName,
                                const Expression &parameter,
                                bool distinct)
{
    lang::copy_on_write_lazy_lang<lang::ExprFunction> cowLang;
    lang::ExprFunction &lang = cowLang.get_or_copy();
    if (distinct) {
        lang.type = lang::ExprFunction::Type::DistinctExpr;
    } else {
        lang.type = lang::ExprFunction::Type::Expr;
    }
    lang.functionName.assign(functionName);
    lang.exprs.append(parameter.getLang());
    return cowLang;
}

Expression Expression::Function(const std::string &functionName,
                                const std::list<Expression> &parameters,
                                bool distinct)
{
    lang::copy_on_write_lazy_lang<lang::ExprFunction> cowLang;
    lang::ExprFunction &lang = cowLang.get_or_copy();
    if (distinct) {
        lang.type = lang::ExprFunction::Type::DistinctExpr;
    } else {
        lang.type = lang::ExprFunction::Type::Expr;
    }
    lang.functionName.assign(functionName);
    for (const Expression &parameter : parameters) {
        lang.exprs.append(parameter.getLang());
    }
    return cowLang;
}

Expression Expression::FunctionAll(const std::string &functionName)
{
    lang::copy_on_write_lazy_lang<lang::ExprFunction> cowLang;
    lang::ExprFunction &lang = cowLang.get_or_copy();
    lang.type = lang::ExprFunction::Type::Star;
    lang.functionName.assign(functionName);
    return cowLang;
}

Expression::Expression(const std::list<Expression> &expressions)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::List;
    lang::ExprList &langExprList = lang.exprList.get_or_copy();
    for (const Expression &expression : expressions) {
        langExprList.exprs.append(expression.getLang());
    }
}

Expression::Expression(
    const lang::copy_on_write_lazy_lang<lang::ExprCast> &exprCast)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::Cast;
    lang.exprCast.assign(exprCast);
}

Expression Expression::Cast(const Expression &expression,
                            const ColumnType &columnType)
{
    lang::copy_on_write_lazy_lang<lang::ExprCast> cowLang;
    lang::ExprCast &lang = cowLang.get_or_copy();
    lang.expr.assign(expression.getLang());
    lang.type = columnType;
    return cowLang;
}

Expression &Expression::withCollate(const std::string &collationName)
{
    lang::copy_on_write_lazy_lang<lang::ExprCollate> cowLang;
    {
        lang::ExprCollate &lang = cowLang.get_or_copy();
        lang.collationName.assign(collationName);
        lang.expr.assign(getLang());
    }

    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::Collate;
    lang.exprCollate.assign(cowLang);
    return *this;
}

Expression::Expression(
    const lang::copy_on_write_lazy_lang<lang::ExprPattern> &exprPattern)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::Pattern;
    lang.exprPattern.assign(exprPattern);
}

lang::copy_on_write_lazy_lang<lang::ExprPattern>
Expression::pattern(const lang::ExprPattern::Type &type,
                    const Expression &operand,
                    bool isNot) const
{
    lang::copy_on_write_lazy_lang<lang::ExprPattern> cowLang;
    lang::ExprPattern &lang = cowLang.get_or_copy();
    lang.isNot = isNot;
    lang.type = type;
    lang.left.assign(getLang());
    lang.right.assign(operand.getLang());
    return cowLang;
}

Expression Expression::like(const Expression &expr) const
{
    return pattern(lang::ExprPattern::Type::Like, expr, false);
}

Expression Expression::glob(const Expression &expr) const
{
    return pattern(lang::ExprPattern::Type::Glob, expr, false);
}

Expression Expression::regexp(const Expression &expr) const
{
    return pattern(lang::ExprPattern::Type::Regexp, expr, false);
}

Expression Expression::match(const Expression &expr) const
{
    return pattern(lang::ExprPattern::Type::Match, expr, false);
}

Expression Expression::notLike(const Expression &expr) const
{
    return pattern(lang::ExprPattern::Type::Like, expr, true);
}

Expression Expression::notGlob(const Expression &expr) const
{
    return pattern(lang::ExprPattern::Type::Glob, expr, true);
}

Expression Expression::notRegexp(const Expression &expr) const
{
    return pattern(lang::ExprPattern::Type::Regexp, expr, true);
}

Expression Expression::notMatch(const Expression &expr) const
{
    return pattern(lang::ExprPattern::Type::Match, expr, true);
}

Expression &Expression::withEscape(const Expression &expr)
{
    assert(getLang().get().type == lang::Expr::Type::Pattern);
    lang::Expr &lang = getMutableLang();
    lang.exprPattern.get_or_copy().escape.assign(expr.getLang());
    return *this;
}

Expression::Expression(
    const lang::copy_on_write_lazy_lang<lang::ExprNull> &exprNull)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::Null;
    lang.exprNull.assign(exprNull);
}

Expression::Expression(
    const lang::copy_on_write_lazy_lang<lang::ExprBetween> &exprBetween)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::Between;
    lang.exprBetween.assign(exprBetween);
}

lang::copy_on_write_lazy_lang<lang::ExprNull>
Expression::null(bool isNull) const
{
    lang::copy_on_write_lazy_lang<lang::ExprNull> cowLang;
    lang::ExprNull &lang = cowLang.get_or_copy();
    lang.isNull = isNull;
    lang.expr.assign(getLang());
    return cowLang;
}

lang::copy_on_write_lazy_lang<lang::ExprBetween> Expression::between(
    const Expression &left, const Expression &right, bool isNot) const
{
    lang::copy_on_write_lazy_lang<lang::ExprBetween> cowLang;
    lang::ExprBetween &lang = cowLang.get_or_copy();
    lang.isNot = isNot;
    lang.left.assign(left.getLang());
    lang.right.assign(right.getLang());
    return cowLang;
}

Expression Expression::isNull() const
{
    return null(true);
}

Expression Expression::notNull() const
{
    return null(false);
}

Expression Expression::between(const Expression &left,
                               const Expression &right) const
{
    return between(left, right, false);
}

Expression Expression::notBetween(const Expression &left,
                                  const Expression &right) const
{
    return between(left, right, true);
}

Expression::Expression(
    const lang::copy_on_write_lazy_lang<lang::ExprIn> &exprIn)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::In;
    lang.exprIn.assign(exprIn);
}

lang::copy_on_write_lazy_lang<lang::ExprIn>
Expression::in(const StatementSelect &selectSTMT, bool isNot)
{
    lang::copy_on_write_lazy_lang<lang::ExprIn> cowLang;
    lang::ExprIn &lang = cowLang.get_or_copy();
    lang.switcher = lang::ExprIn::Switch::Select;
    lang.expr.assign(getLang());
    lang.selectSTMT.assign(selectSTMT.getLang());
    lang.isNot = isNot;
    return cowLang;
}

lang::copy_on_write_lazy_lang<lang::ExprIn>
Expression::in(const Expression &expression, bool isNot)
{
    lang::copy_on_write_lazy_lang<lang::ExprIn> cowLang;
    lang::ExprIn &lang = cowLang.get_or_copy();
    lang.switcher = lang::ExprIn::Switch::Expr;
    lang.expr.assign(getLang());
    lang.exprs.append(expression.getLang());
    lang.isNot = isNot;
    return cowLang;
}

lang::copy_on_write_lazy_lang<lang::ExprIn>
Expression::in(const std::list<Expression> &expressions, bool isNot)
{
    lang::copy_on_write_lazy_lang<lang::ExprIn> cowLang;
    lang::ExprIn &lang = cowLang.get_or_copy();
    lang.switcher = lang::ExprIn::Switch::Expr;
    lang.expr.assign(getLang());
    for (const Expression &expression : expressions) {
        lang.exprs.append(expression.getLang());
    }
    lang.isNot = isNot;
    return cowLang;
}

lang::copy_on_write_lazy_lang<lang::ExprIn>
Expression::in(const std::string &schemaName,
               const std::string &tableOrFunctionName,
               bool isNot)
{
    lang::copy_on_write_lazy_lang<lang::ExprIn> cowLang;
    lang::ExprIn &lang = cowLang.get_or_copy();
    lang.switcher = lang::ExprIn::Switch::TableOrFunction;
    lang.expr.assign(getLang());
    if (!schemaName.empty()) {
        lang.schemaName.assign(schemaName);
    }
    lang.tableNameOrFunction.assign(tableOrFunctionName);
    lang.isNot = isNot;
    return cowLang;
}

lang::copy_on_write_lazy_lang<lang::ExprIn>
Expression::in(const std::string &schemaName,
               const std::string &tableOrFunctionName,
               const Expression &expression,
               bool isNot)
{
    lang::copy_on_write_lazy_lang<lang::ExprIn> cowLang;
    lang::ExprIn &lang = cowLang.get_or_copy();
    lang.switcher = lang::ExprIn::Switch::TableOrFunction;
    lang.expr.assign(getLang());
    if (!schemaName.empty()) {
        lang.schemaName.assign(schemaName);
    }
    lang.tableNameOrFunction.assign(tableOrFunctionName);
    lang.exprs.append(expression.getLang());
    lang.isNot = isNot;
    return cowLang;
}

lang::copy_on_write_lazy_lang<lang::ExprIn>
Expression::in(const std::string &schemaName,
               const std::string &tableOrFunctionName,
               const std::list<Expression> &expressions,
               bool isNot)
{
    lang::copy_on_write_lazy_lang<lang::ExprIn> cowLang;
    lang::ExprIn &lang = cowLang.get_or_copy();
    lang.switcher = lang::ExprIn::Switch::TableOrFunction;
    lang.expr.assign(getLang());
    if (!schemaName.empty()) {
        lang.schemaName.assign(schemaName);
    }
    lang.tableNameOrFunction.assign(tableOrFunctionName);
    for (const Expression &expression : expressions) {
        lang.exprs.append(expression.getLang());
    }
    lang.isNot = isNot;
    return cowLang;
}

Expression Expression::in(const StatementSelect &selectSTMT)
{
    return in(selectSTMT, false);
}

Expression Expression::notIn(const StatementSelect &selectSTMT)
{
    return in(selectSTMT, true);
}

Expression Expression::in(const Expression &expression)
{
    return in(expression, false);
}

Expression Expression::notIn(const Expression &expression)
{
    return in(expression, true);
}

Expression Expression::in(const std::list<Expression> &expressions)
{
    return in(expressions, false);
}

Expression Expression::notIn(const std::list<Expression> &expressions)
{
    return in(expressions, true);
}

Expression Expression::in(const std::string &tableOrFunctionName)
{
    return in("", tableOrFunctionName, false);
}

Expression Expression::notIn(const std::string &tableOrFunctionName)
{
    return in("", tableOrFunctionName, true);
}

Expression Expression::in(const std::string &schemaName,
                          const std::string &tableOrFunctionName)
{
    return in(schemaName, tableOrFunctionName, false);
}

Expression Expression::notIn(const std::string &schemaName,
                             const std::string &tableOrFunctionName)
{
    return in(schemaName, tableOrFunctionName, false);
}

Expression Expression::in(const std::string &schemaName,
                          const std::string &functionName,
                          const Expression &expression)
{
    return in(schemaName, functionName, expression, true);
}

Expression Expression::notIn(const std::string &schemaName,
                             const std::string &functionName,
                             const Expression &expression)
{
    return in(schemaName, functionName, expression, false);
}

Expression Expression::in(const std::string &schemaName,
                          const std::string &functionName,
                          const std::list<Expression> &expressions)
{
    return in(schemaName, functionName, expressions, false);
}

Expression Expression::notIn(const std::string &schemaName,
                             const std::string &functionName,
                             const std::list<Expression> &expressions)
{
    return in(schemaName, functionName, expressions, true);
}

Expression::Expression(
    const lang::copy_on_write_lazy_lang<lang::ExprExists> &exprExists)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::Exists;
    lang.exprExists.assign(exprExists);
}

lang::copy_on_write_lazy_lang<lang::ExprExists>
Expression::Exists(const StatementSelect &selectSTMT, bool isNot)
{
    lang::copy_on_write_lazy_lang<lang::ExprExists> cowLang;
    lang::ExprExists &lang = cowLang.get_or_copy();
    lang.isNot = isNot;
    lang.selectSTMT.assign(selectSTMT.getLang());
    return cowLang;
}

Expression Expression::Exists(const StatementSelect &selectSTMT)
{
    return Expression::Exists(selectSTMT, false);
}

Expression Expression::NotExists(const StatementSelect &selectSTMT)
{
    return Expression::Exists(selectSTMT, true);
}

Expression::Expression(const CaseInternal &expressionCase)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::Case;
    lang.exprCase.assign(expressionCase.getLang());
}

Expression::CaseInternal Expression::Case(const Expression &expression)
{
    return Expression::CaseInternal(expression);
}

Expression::CaseInternal::CaseInternal(const Expression &expression)
{
    lang::ExprCase &lang = getMutableLang();
    lang.exprCase.assign(expression.getLang());
}

Expression::CaseInternal &
Expression::CaseInternal::whenAndThen(const Expression &when,
                                      const Expression &then)
{
    lang::copy_on_write_lazy_lang<lang::ExprCase::Pair> pair;
    {
        lang::ExprCase::Pair &langPair = pair.get_or_copy();
        langPair.when.assign(when.getLang());
        langPair.then.assign(then.getLang());
    }
    lang::ExprCase &lang = getMutableLang();
    lang.pairs.append(pair);
    return *this;
}

Expression::CaseInternal &
Expression::CaseInternal::else_(const Expression &expression)
{
    lang::ExprCase &lang = getMutableLang();
    lang.exprElse.assign(expression.getLang());
    return *this;
}

Expression::Expression(const RaiseFunction &raiseFunction)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::RaiseFunction;
    lang.raiseFunction.assign(raiseFunction.getLang());
}

} // namespace WCDB
