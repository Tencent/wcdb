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

Expression::Expression()
{
}

Expression::Expression(const LiteralValue &literalValue)
{
    setLiteralValue(literalValue);
}

Expression::Expression(const BindParameter &bindParameter)
{
    Lang::Expr &lang = getMutableLang();
    lang.type = Lang::ExprBase::Type::BindParameter;
    lang.bindParamter.assign(bindParameter.getCOWLang());
}

Expression::Expression(const Column &column)
{
    Lang::Expr &lang = getMutableLang();
    lang.type = Lang::ExprBase::Type::Column;
    lang.exprColumn.get_or_copy().column.assign(column.getCOWLang());
}

Expression &Expression::withTable(const std::string &tableName)
{
    Lang::Expr &lang = getMutableLang();
    WINQRemedialAssert(lang.type == Lang::ExprBase::Type::Column);
    lang.exprColumn.get_or_copy().tableName.assign(tableName);
    return *this;
}

Expression &Expression::withSchema(const std::string &schemaName)
{
    Lang::Expr &lang = getMutableLang();
    WINQRemedialAssert(lang.type == Lang::ExprBase::Type::Column);
    lang.exprColumn.get_or_copy().schemaName.assign(schemaName);
    return *this;
}

Expression::Expression(const Lang::CopyOnWriteLazyLang<Lang::ExprUnaryOperation>
                           &exprUnaryOperation)
{
    Lang::Expr &lang = getMutableLang();
    lang.type = Lang::ExprBase::Type::UnaryOperator;
    lang.exprUnaryOperator.assign(exprUnaryOperation);
}

Expression::Expression(
    const Lang::CopyOnWriteLazyLang<Lang::ExprBinaryOperation>
        &exprBinaryOperation)
{
    Lang::Expr &lang = getMutableLang();
    lang.type = Lang::ExprBase::Type::BinaryOperator;
    lang.exprBinaryOperator.assign(exprBinaryOperation);
}

Expression::Expression(
    const Lang::CopyOnWriteLazyLang<Lang::ExprFunction> &exprFunction)
{
    Lang::Expr &lang = getMutableLang();
    lang.type = Lang::ExprBase::Type::Function;
    lang.exprFunction.assign(exprFunction);
}

const Expression &Expression::lastInsertedRowid()
{
    static const Expression *s_lastInsertedRowid =
        new Expression("last_insert_rowid");
    return *s_lastInsertedRowid;
}

Expression Expression::function(const std::string &functionName)
{
    Lang::CopyOnWriteLazyLang<Lang::ExprFunction> cowLang;
    Lang::ExprFunction &lang = cowLang.get_or_copy();
    lang.type = Lang::ExprFunction::Type::NotSet;
    lang.functionName.assign(functionName);
    return cowLang;
}

Expression Expression::function(const std::string &functionName,
                                const Expression &parameter,
                                bool distinct)
{
    Lang::CopyOnWriteLazyLang<Lang::ExprFunction> cowLang;
    Lang::ExprFunction &lang = cowLang.get_or_copy();
    if (distinct) {
        lang.type = Lang::ExprFunction::Type::DistinctExpr;
    } else {
        lang.type = Lang::ExprFunction::Type::Expr;
    }
    lang.functionName.assign(functionName);
    lang.exprs.append(parameter.getCOWLang());
    return cowLang;
}

Expression Expression::function(const std::string &functionName,
                                const std::list<Expression> &parameters,
                                bool distinct)
{
    Lang::CopyOnWriteLazyLang<Lang::ExprFunction> cowLang;
    Lang::ExprFunction &lang = cowLang.get_or_copy();
    if (distinct) {
        lang.type = Lang::ExprFunction::Type::DistinctExpr;
    } else {
        lang.type = Lang::ExprFunction::Type::Expr;
    }
    lang.functionName.assign(functionName);
    for (const Expression &parameter : parameters) {
        lang.exprs.append(parameter.getCOWLang());
    }
    return cowLang;
}

const Expression::All &Expression::All::default_()
{
    static const Expression::All *s_default = new Expression::All;
    return *s_default;
}

Expression Expression::All::function(const std::string &functionName,
                                     __unused bool distinct) const
{
    Lang::CopyOnWriteLazyLang<Lang::ExprFunction> cowLang;
    Lang::ExprFunction &lang = cowLang.get_or_copy();
    lang.type = Lang::ExprFunction::Type::Star;
    lang.functionName.assign(functionName);
    return cowLang;
}

Expression::Expression(const std::list<Expression> &expressions)
{
    Lang::Expr &lang = getMutableLang();
    lang.type = Lang::ExprBase::Type::List;
    Lang::ExprList &langExprList = lang.exprList.get_or_copy();
    for (const Expression &expression : expressions) {
        langExprList.exprs.append(expression.getCOWLang());
    }
}

Expression::Expression(
    const Lang::CopyOnWriteLazyLang<Lang::ExprCast> &exprCast)
{
    Lang::Expr &lang = getMutableLang();
    lang.type = Lang::ExprBase::Type::Cast;
    lang.exprCast.assign(exprCast);
}

Expression::Expression(
    const Lang::CopyOnWriteLazyLang<Lang::ExprPattern> &exprPattern)
{
    Lang::Expr &lang = getMutableLang();
    lang.type = Lang::ExprBase::Type::Pattern;
    lang.exprPattern.assign(exprPattern);
}

Expression &Expression::escape(const Expression &expr)
{
    WINQRemedialAssert(getCOWLang().get<Lang::Expr>().type ==
                       Lang::ExprBase::Type::Pattern);
    Lang::Expr &lang = getMutableLang();
    lang.exprPattern.get_or_copy().escape.assign(expr.getCOWLang());
    return *this;
}

Expression::Expression(
    const Lang::CopyOnWriteLazyLang<Lang::ExprNull> &exprNull)
{
    Lang::Expr &lang = getMutableLang();
    lang.type = Lang::ExprBase::Type::Null;
    lang.exprNull.assign(exprNull);
}

Expression::Expression(
    const Lang::CopyOnWriteLazyLang<Lang::ExprBetween> &exprBetween)
{
    Lang::Expr &lang = getMutableLang();
    lang.type = Lang::ExprBase::Type::Between;
    lang.exprBetween.assign(exprBetween);
}

Expression::Expression(const Lang::CopyOnWriteLazyLang<Lang::ExprIn> &exprIn)
{
    Lang::Expr &lang = getMutableLang();
    lang.type = Lang::ExprBase::Type::In;
    lang.exprIn.assign(exprIn);
}

Expression::Expression(const StatementSelect &selectSTMT,
                       bool isNot,
                       bool exists)
{
    Lang::Expr &lang = getMutableLang();
    lang.type = Lang::ExprBase::Type::Exists;
    Lang::ExprExists &langExists = lang.exprExists.get_or_copy();
    langExists.isNot = isNot;
    langExists.exists = exists;
    langExists.selectSTMT.assign(selectSTMT.getCOWLang());
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
    Lang::Expr &lang = getMutableLang();
    lang.type = Lang::ExprBase::Type::Case;
    lang.exprCase.assign(expressionCase.getCOWLang());
}

Expression::CaseInternal Expression::case_()
{
    return Expression::CaseInternal();
}

Expression::CaseInternal Expression::case_(const Expression &expression)
{
    return Expression::CaseInternal(expression);
}

Expression::CaseInternal::CaseInternal()
{
}

Expression::CaseInternal::CaseInternal(const Expression &expression)
{
    Lang::ExprCase &lang = getMutableLang();
    lang.exprCase.assign(expression.getCOWLang());
}

Expression::CaseInternal &
Expression::CaseInternal::whenAndThen(const Expression &when,
                                      const Expression &then)
{
    Lang::CopyOnWriteLazyLang<Lang::ExprCase::Pair> pair;
    {
        Lang::ExprCase::Pair &langPair = pair.get_or_copy();
        langPair.when.assign(when.getCOWLang());
        langPair.then.assign(then.getCOWLang());
    }
    Lang::ExprCase &lang = getMutableLang();
    lang.pairs.append(pair);
    return *this;
}

Expression::CaseInternal &
Expression::CaseInternal::else_(const Expression &expression)
{
    Lang::ExprCase &lang = getMutableLang();
    lang.exprElse.assign(expression.getCOWLang());
    return *this;
}

Expression::Expression(const RaiseFunction &raiseFunction)
{
    Lang::Expr &lang = getMutableLang();
    lang.type = Lang::ExprBase::Type::RaiseFunction;
    lang.raiseFunction.assign(raiseFunction.getCOWLang());
}

void Expression::setLiteralValue(const LiteralValue &literalValue)
{
    Lang::Expr &lang = getMutableLang();
    lang.type = Lang::ExprBase::Type::LiteralValue;
    lang.literalValue.assign(literalValue.getCOWLang());
}

Lang::CopyOnWriteLazyLang<Lang::Expr> Expression::getExpressionLang() const
{
    return getCOWLang();
}

Expression::Expression(
    const Lang::CopyOnWriteLazyLang<Lang::ExprCollate> &exprCollate)
{
    Lang::Expr &lang = getMutableLang();
    lang.type = Lang::ExprBase::Type::Collate;
    lang.exprCollate.assign(exprCollate);
}

Expression::Expression(const Lang::CopyOnWriteLazyLang<Lang::Expr> &expr)
    : DescribableWithLang<Lang::Expr>(expr)
{
}

Expression Expression::getRedirectSource() const
{
    return *this;
}

Expression::operator std::list<WCDB::Expression>() const
{
    return {*this};
}

} // namespace WCDB
