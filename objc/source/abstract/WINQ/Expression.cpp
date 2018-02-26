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

#include <WCDB/WINQ.h>

namespace WCDB {

Expression::Expression(const LiteralValue &literalValue)
{
    setLiteralValue(literalValue);
}

Expression::Expression(const BindParameter &bindParameter)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::BindParameter;
    lang.bindParamter.assign(bindParameter.getLang());
}

Expression::Expression(const Column &column)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::Column;
    lang.exprColumn.get_or_copy().column.assign(column.getLang());
}

Expression &Expression::withTable(const std::string &tableName)
{
    lang::Expr &lang = getMutableLang();
    assert(lang.type == lang::Expr::Type::Column);
    lang.exprColumn.get_or_copy().tableName.assign(tableName);
    return *this;
}

Expression &Expression::withSchema(const std::string &schemaName)
{
    lang::Expr &lang = getMutableLang();
    assert(lang.type == lang::Expr::Type::Column);
    lang.exprColumn.get_or_copy().schemaName.assign(schemaName);
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

Expression::Expression(
    const lang::copy_on_write_lazy_lang<lang::ExprBinaryOperation>
        &exprBinaryOperation)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::BinaryOperator;
    lang.exprBinaryOperator.assign(exprBinaryOperation);
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
    lang.type = lang::ExprFunction::Type::NotSet;
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

Expression Expression::All::function(const std::string &functionName,
                                     __unused bool distinct) const
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

Expression::Expression(
    const lang::copy_on_write_lazy_lang<lang::ExprPattern> &exprPattern)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::Pattern;
    lang.exprPattern.assign(exprPattern);
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

Expression::Expression(
    const lang::copy_on_write_lazy_lang<lang::ExprIn> &exprIn)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::In;
    lang.exprIn.assign(exprIn);
}

Expression::Expression(const StatementSelect &selectSTMT,
                       bool isNot,
                       bool exists)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::Exists;
    lang::ExprExists &langExists = lang.exprExists.get_or_copy();
    langExists.isNot = isNot;
    langExists.exists = exists;
    langExists.selectSTMT.assign(selectSTMT.getLang());
}

Expression Expression::Exists(const StatementSelect &selectSTMT)
{
    return Expression(selectSTMT, false, true);
}

Expression Expression::NotExists(const StatementSelect &selectSTMT)
{
    return Expression(selectSTMT, true, true);
}

Expression::Expression(const CaseInternal &expressionCase)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::Case;
    lang.exprCase.assign(expressionCase.getLang());
}

Expression::CaseInternal Expression::Case()
{
    return Expression::CaseInternal();
}

Expression::CaseInternal Expression::Case(const Expression &expression)
{
    return Expression::CaseInternal(expression);
}

Expression::CaseInternal::CaseInternal()
{
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

void Expression::setLiteralValue(const LiteralValue &literalValue)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::LiteralValue;
    lang.literalValue.assign(literalValue.getLang());
}

lang::copy_on_write_lazy_lang<lang::Expr> Expression::getExpressionLang() const
{
    return getLang();
}

Expression::Expression(
    const lang::copy_on_write_lazy_lang<lang::ExprCollate> &exprCollate)
{
    lang::Expr &lang = getMutableLang();
    lang.type = lang::Expr::Type::Collate;
    lang.exprCollate.assign(exprCollate);
}

Expression::Expression(const lang::copy_on_write_lazy_lang<lang::Expr> &expr)
    : DescribableWithLang<lang::Expr>(expr)
{
}

Expression Expression::getRedirectSource() const
{
    return *this;
}

} // namespace WCDB
