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

Expression::Expression(const LiteralValue& literalValue)
{
    LangExpr& lang = getMutableLang();
    lang.type = LangExpr::Type::LiteralValue;
    lang.literalValue.assign(literalValue.getLang());
}

Expression::Expression(const BindParameter& bindParameter)
{
    LangExpr& lang = getMutableLang();
    lang.type = LangExpr::Type::BindParameter;
    lang.bindParamter.assign(bindParameter.getLang());
}

Expression::Expression(const Expression::Column& expressionColumn)
{
    LangExpr& lang = getMutableLang();
    lang.type = LangExpr::Type::Column;
    lang.exprColumn.assign(expressionColumn.getLang());
}

Expression::Column Expression::ColumnNamed(const std::string& columnName)
{
    return Expression::Column(columnName);
}

Expression::Column::Column(const std::string& columnName)
{
    LangExprColumn& lang = getMutableLang();
    lang.columnName.assign(columnName);
}

Expression::Column& Expression::Column::withTable(const std::string& tableName)
{
    LangExprColumn& lang = getMutableLang();
    lang.tableName.assign(tableName);
    return *this;
}

Expression::Column& Expression::Column::withSchema(const std::string& schemaName)
{
    LangExprColumn& lang = getMutableLang();
    lang.schemaName.assign(schemaName);
    return *this;
}

Expression::Expression(const copy_on_write_lazy_lang<LangExprUnaryOperation>& exprUnaryOperation)
{
    LangExpr& lang = getMutableLang();
    lang.type = LangExpr::Type::UnaryOperator;
    lang.exprUnaryOperator.assign(exprUnaryOperation);
}

copy_on_write_lazy_lang<LangExprUnaryOperation> Expression::unaryOperation(const LangExprUnaryOperation::Operator& unaryOperator) const
{
    copy_on_write_lazy_lang<LangExprUnaryOperation> cowLang;
    LangExprUnaryOperation& lang = cowLang.get_or_copy();
    lang.expr.assign(getLang());
    lang.unaryOperator = unaryOperator;
    return cowLang;
}

Expression Expression::operator - () const
{
    return unaryOperation(LangExprUnaryOperation::Operator::Negative);
}

Expression Expression::operator + () const
{
    return unaryOperation(LangExprUnaryOperation::Operator::Positive);
}

Expression Expression::operator ! () const
{
    return unaryOperation(LangExprUnaryOperation::Operator::Not);
}

Expression Expression::operator ~ () const
{
    return unaryOperation(LangExprUnaryOperation::Operator::Tilde);
}

Expression::Expression(const copy_on_write_lazy_lang<LangExprBinaryOperation>& exprBinaryOperation)
{
    LangExpr& lang = getMutableLang();
    lang.type = LangExpr::Type::BinaryOperator;
    lang.exprBinaryOperator.assign(exprBinaryOperation);
}

copy_on_write_lazy_lang<LangExprBinaryOperation> Expression::binaryOperation(const LangExprBinaryOperation::Operator& binaryOperator, const Expression& operand) const
{
    copy_on_write_lazy_lang<LangExprBinaryOperation> cowLang;
    LangExprBinaryOperation& lang = cowLang.get_or_copy();
    lang.left.assign(getLang());
    lang.right.assign(operand.getLang());
    lang.binaryOperator = binaryOperator; 
    return cowLang;
}

Expression Expression::concat(const Expression& operand) const
{
    return binaryOperation(LangExprBinaryOperation::Operator::Concatenate, operand);
}

Expression Expression::operator * (const Expression& operand) const
{    
    return binaryOperation(LangExprBinaryOperation::Operator::Multiply, operand);
}

Expression Expression::operator / (const Expression& operand) const
{
    return binaryOperation(LangExprBinaryOperation::Operator::Divide, operand);
}

Expression Expression::operator % (const Expression& operand) const
{
    return binaryOperation(LangExprBinaryOperation::Operator::Modulo, operand);
}

Expression Expression::operator + (const Expression& operand) const
{
    return binaryOperation(LangExprBinaryOperation::Operator::Plus, operand);
}

Expression Expression::operator - (const Expression& operand) const
{
    return binaryOperation(LangExprBinaryOperation::Operator::Minus, operand);
}

Expression Expression::operator << (const Expression& operand) const
{
    return binaryOperation(LangExprBinaryOperation::Operator::LeftShift, operand);
}

Expression Expression::operator >> (const Expression& operand) const
{
    return binaryOperation(LangExprBinaryOperation::Operator::RightShift, operand);
}

Expression Expression::operator & (const Expression& operand) const
{
    return binaryOperation(LangExprBinaryOperation::Operator::BitwiseAnd, operand);
}

Expression Expression::operator | (const Expression& operand) const
{
    return binaryOperation(LangExprBinaryOperation::Operator::BitwiseOr, operand);
}

Expression Expression::operator < (const Expression& operand) const
{
    return binaryOperation(LangExprBinaryOperation::Operator::Less, operand);
}

Expression Expression::operator <= (const Expression& operand) const
{
    return binaryOperation(LangExprBinaryOperation::Operator::LessOrEqual, operand);
}

Expression Expression::operator > (const Expression& operand) const
{
    return binaryOperation(LangExprBinaryOperation::Operator::Greater, operand);
}

Expression Expression::operator >= (const Expression& operand) const
{
    return binaryOperation(LangExprBinaryOperation::Operator::GreaterOrEqual, operand);
}

Expression Expression::operator == (const Expression& operand) const
{
    return binaryOperation(LangExprBinaryOperation::Operator::Equal, operand);
}

Expression Expression::operator != (const Expression& operand) const
{
    return binaryOperation(LangExprBinaryOperation::Operator::NotEqual, operand);
}

Expression Expression::is(const Expression& operand) const
{
    return binaryOperation(LangExprBinaryOperation::Operator::Is, operand);
}

Expression Expression::isNot(const Expression& operand) const
{
    return binaryOperation(LangExprBinaryOperation::Operator::IsNot, operand);
}

Expression Expression::operator && (const Expression& operand) const
{
    return binaryOperation(LangExprBinaryOperation::Operator::And, operand);
}

Expression Expression::operator || (const Expression& operand) const
{
    return binaryOperation(LangExprBinaryOperation::Operator::Or, operand);
}

Expression::Expression(const copy_on_write_lazy_lang<LangExprFunction>& exprFunction)
{
    LangExpr& lang = getMutableLang();
    lang.type = LangExpr::Type::Function;
    lang.exprFunction.assign(exprFunction);
}

Expression Expression::Function(const std::string& functionName)
{
    copy_on_write_lazy_lang<LangExprFunction> cowLang;
    LangExprFunction& lang = cowLang.get_or_copy();
    lang.functionName.assign(functionName);    
    return cowLang;
}

Expression Expression::Function(const std::string& functionName, const Expression& parameter, bool distinct)
{
    copy_on_write_lazy_lang<LangExprFunction> cowLang;
    LangExprFunction& lang = cowLang.get_or_copy();
    if (distinct) {
        lang.type = LangExprFunction::Type::DistinctExpr;
    }else {
        lang.type = LangExprFunction::Type::Expr;
    }
    lang.functionName.assign(functionName);    
    lang.exprs.append(parameter.getLang());
    return cowLang;
}

Expression Expression::Function(const std::string& functionName, const std::list<Expression>& parameters, bool distinct)
{
    copy_on_write_lazy_lang<LangExprFunction> cowLang;
    LangExprFunction& lang = cowLang.get_or_copy();
    if (distinct) {
        lang.type = LangExprFunction::Type::DistinctExpr;
    }else {
        lang.type = LangExprFunction::Type::Expr;
    }
    lang.functionName.assign(functionName);    
    for (const Expression& parameter : parameters) {
        lang.exprs.append(parameter.getLang());
    }
    return cowLang;
}

Expression Expression::FunctionAll(const std::string& functionName)
{
    copy_on_write_lazy_lang<LangExprFunction> cowLang;
    LangExprFunction& lang = cowLang.get_or_copy();
    lang.type = LangExprFunction::Type::Star;
    lang.functionName.assign(functionName);  
    return cowLang;
}

Expression::Expression(const std::list<Expression>& expressions)
{
    LangExpr& lang = getMutableLang();
    lang.type = LangExpr::Type::List; 
    LangExprList& langExprList = lang.exprList.get_or_copy();
    for (const Expression& expression : expressions) {
        langExprList.exprs.append(expression.getLang());
    }
}

Expression::Expression(const copy_on_write_lazy_lang<LangExprCast>& exprCast)
{
    LangExpr& lang = getMutableLang();
    lang.type = LangExpr::Type::Cast; 
    lang.exprCast.assign(exprCast);
}

Expression Expression::Cast(const Expression& expression, const ColumnType& columnType)
{
    copy_on_write_lazy_lang<LangExprCast> cowLang;
    LangExprCast& lang = cowLang.get_or_copy();
    lang.expr.assign(expression.getLang());
    lang.type = columnType;
    return cowLang;
}

Expression& Expression::withCollate(const std::string& collationName)
{
    copy_on_write_lazy_lang<LangExprCollate> cowLang;
    {
        LangExprCollate& lang = cowLang.get_or_copy();
        lang.collationName.assign(collationName);
        lang.expr.assign(getLang());
    }
    
    LangExpr& lang = getMutableLang();
    lang.type = LangExpr::Type::Collate;
    lang.exprCollate.assign(cowLang);
    return *this;
}

Expression::Expression(const copy_on_write_lazy_lang<LangExprPattern>& exprPattern)
{
    LangExpr& lang = getMutableLang();
    lang.type = LangExpr::Type::Pattern;
    lang.exprPattern.assign(exprPattern);
}

copy_on_write_lazy_lang<LangExprPattern> Expression::pattern(const LangExprPattern::Type& type, const Expression& operand, bool isNot) const
{
    copy_on_write_lazy_lang<LangExprPattern> cowLang;
    LangExprPattern& lang = cowLang.get_or_copy();
    lang.isNot = isNot;
    lang.type = type;
    lang.left.assign(getLang());
    lang.right.assign(operand.getLang());
    return cowLang;
}

Expression Expression::like(const Expression& expr) const
{
    return pattern(LangExprPattern::Type::Like, expr, false);
}

Expression Expression::glob(const Expression& expr) const
{
    return pattern(LangExprPattern::Type::Glob, expr, false);
}

Expression Expression::regexp(const Expression& expr) const
{
    return pattern(LangExprPattern::Type::Regexp, expr, false);
}

Expression Expression::match(const Expression& expr) const
{
    return pattern(LangExprPattern::Type::Match, expr, false);
}

Expression Expression::notLike(const Expression& expr) const
{
    return pattern(LangExprPattern::Type::Like, expr, true);
}

Expression Expression::notGlob(const Expression& expr) const
{
    return pattern(LangExprPattern::Type::Glob, expr, true);
}

Expression Expression::notRegexp(const Expression& expr) const
{
    return pattern(LangExprPattern::Type::Regexp, expr, true);
}

Expression Expression::notMatch(const Expression& expr) const
{
    return pattern(LangExprPattern::Type::Match, expr, true);
}

Expression& Expression::withEscape(const Expression& expr)
{
    assert(getLang().get().type == LangExpr::Type::Pattern);
    LangExpr& lang = getMutableLang();
    lang.exprPattern.get_or_copy().escape.assign(expr.getLang());
    return *this;
}

Expression::Expression(const copy_on_write_lazy_lang<LangExprNull>& exprNull)
{
    LangExpr& lang = getMutableLang();
    lang.type = LangExpr::Type::Null;
    lang.exprNull.assign(exprNull);
}

Expression::Expression(const copy_on_write_lazy_lang<LangExprBetween>& exprBetween)
{
    LangExpr& lang = getMutableLang();
    lang.type = LangExpr::Type::Between;
    lang.exprBetween.assign(exprBetween);
}

copy_on_write_lazy_lang<LangExprNull> Expression::null(bool isNull) const
{
    copy_on_write_lazy_lang<LangExprNull> cowLang;
    LangExprNull& lang = cowLang.get_or_copy();
    lang.isNull = isNull;
    lang.expr.assign(getLang());
    return cowLang;
}

copy_on_write_lazy_lang<LangExprBetween> Expression::between(const Expression& left, const Expression& right, bool isNot) const
{
    copy_on_write_lazy_lang<LangExprBetween> cowLang;
    LangExprBetween& lang = cowLang.get_or_copy();
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

Expression Expression::between(const Expression& left, const Expression& right) const
{
    return between(left, right, false);
}

Expression Expression::notBetween(const Expression& left, const Expression& right) const
{
    return between(left, right, true);    
}

Expression::Expression(const copy_on_write_lazy_lang<LangExprIn>& exprIn)    
{
    LangExpr& lang = getMutableLang();
    lang.type = LangExpr::Type::In;
    lang.exprIn.assign(exprIn);
}

copy_on_write_lazy_lang<LangExprIn> Expression::in(const StatementSelect& selectSTMT, bool isNot)
{
    copy_on_write_lazy_lang<LangExprIn> cowLang;
    LangExprIn& lang = cowLang.get_or_copy();
    lang.switcher = LangExprIn::Switch::Select;
    lang.expr.assign(getLang());
    lang.selectSTMT.assign(selectSTMT.getLang());
    lang.isNot = isNot;
    return cowLang;
}

copy_on_write_lazy_lang<LangExprIn> Expression::in(const Expression& expression, bool isNot)
{
    copy_on_write_lazy_lang<LangExprIn> cowLang;
    LangExprIn& lang = cowLang.get_or_copy();
    lang.switcher = LangExprIn::Switch::Expr; 
    lang.expr.assign(getLang());
    lang.exprs.append(expression.getLang());
    lang.isNot = isNot;
    return cowLang;
}

copy_on_write_lazy_lang<LangExprIn> Expression::in(const std::list<Expression>& expressions, bool isNot)
{
    copy_on_write_lazy_lang<LangExprIn> cowLang;
    LangExprIn& lang = cowLang.get_or_copy();
    lang.switcher = LangExprIn::Switch::Expr;
    lang.expr.assign(getLang());
    for (const Expression& expression : expressions) {
        lang.exprs.append(expression.getLang());
    }
    lang.isNot = isNot;
    return cowLang;
}

copy_on_write_lazy_lang<LangExprIn> Expression::in(const std::string& schemaName, const std::string& tableOrFunctionName, bool isNot)
{
    copy_on_write_lazy_lang<LangExprIn> cowLang;
    LangExprIn& lang = cowLang.get_or_copy();
    lang.switcher = LangExprIn::Switch::TableOrFunction;
    lang.expr.assign(getLang());
    if (!schemaName.empty()) {
        lang.schemaName.assign(schemaName);
    }
    lang.tableNameOrFunction.assign(tableOrFunctionName);
    lang.isNot = isNot;
    return cowLang;
}

copy_on_write_lazy_lang<LangExprIn> Expression::in(const std::string& schemaName, const std::string& tableOrFunctionName, const Expression& expression, bool isNot)
{
    copy_on_write_lazy_lang<LangExprIn> cowLang;
    LangExprIn& lang = cowLang.get_or_copy();
    lang.switcher = LangExprIn::Switch::TableOrFunction;
    lang.expr.assign(getLang());
    if (!schemaName.empty()) {
        lang.schemaName.assign(schemaName);
    }
    lang.tableNameOrFunction.assign(tableOrFunctionName);
    lang.exprs.append(expression.getLang());
    lang.isNot = isNot;
    return cowLang;
}

copy_on_write_lazy_lang<LangExprIn> Expression::in(const std::string& schemaName, const std::string& tableOrFunctionName, const std::list<Expression>& expressions, bool isNot)
{
    copy_on_write_lazy_lang<LangExprIn> cowLang;
    LangExprIn& lang = cowLang.get_or_copy();
    lang.switcher = LangExprIn::Switch::TableOrFunction;
    lang.expr.assign(getLang());
    if (!schemaName.empty()) {
        lang.schemaName.assign(schemaName);
    }
    lang.tableNameOrFunction.assign(tableOrFunctionName);
    for (const Expression& expression : expressions) {
        lang.exprs.append(expression.getLang());
    }
    lang.isNot = isNot;
    return cowLang;
}

Expression Expression::in(const StatementSelect& selectSTMT)
{
    return in(selectSTMT, false);
}

Expression Expression::notIn(const StatementSelect& selectSTMT)
{
    return in(selectSTMT, true);
}

Expression Expression::in(const Expression& expression)
{
    return in(expression, false);
}

Expression Expression::notIn(const Expression& expression)
{
    return in(expression, true);
}

Expression Expression::in(const std::list<Expression>& expressions)
{
    return in(expressions, false);
}

Expression Expression::notIn(const std::list<Expression>& expressions)
{
    return in(expressions, true);
}

Expression Expression::in(const std::string& tableOrFunctionName)
{
    return in("", tableOrFunctionName, false);
}

Expression Expression::notIn(const std::string& tableOrFunctionName)
{
    return in("", tableOrFunctionName, true);    
}

Expression Expression::in(const std::string& schemaName, const std::string& tableOrFunctionName)
{
    return in(schemaName, tableOrFunctionName, false);    
}

Expression Expression::notIn(const std::string& schemaName, const std::string& tableOrFunctionName)
{
    return in(schemaName, tableOrFunctionName, false);    
}

Expression Expression::in(const std::string& schemaName, const std::string& functionName, const Expression& expression)
{
    return in(schemaName, functionName, expression, true);    
}

Expression Expression::notIn(const std::string& schemaName, const std::string& functionName, const Expression& expression)
{
    return in(schemaName, functionName, expression, false);    
}

Expression Expression::in(const std::string& schemaName, const std::string& functionName, const std::list<Expression>& expressions)
{
    return in(schemaName, functionName, expressions, false);    
}

Expression Expression::notIn(const std::string& schemaName, const std::string& functionName, const std::list<Expression>& expressions)
{
    return in(schemaName, functionName, expressions, true);    
}

Expression::Expression(const copy_on_write_lazy_lang<LangExprExists>& exprExists)
{
    LangExpr& lang = getMutableLang();
    lang.type = LangExpr::Type::Exists;
    lang.exprExists.assign(exprExists);
}

copy_on_write_lazy_lang<LangExprExists> Expression::Exists(const StatementSelect& selectSTMT, bool isNot)
{
    copy_on_write_lazy_lang<LangExprExists> cowLang;
    LangExprExists& lang = cowLang.get_or_copy();
    lang.isNot = isNot;
    lang.selectSTMT.assign(selectSTMT.getLang());
    return cowLang;
}

Expression Expression::Exists(const StatementSelect& selectSTMT)
{
    return Expression::Exists(selectSTMT, false);
}

Expression Expression::NotExists(const StatementSelect& selectSTMT)
{
    return Expression::Exists(selectSTMT, true);
}

Expression::Expression(const CaseInternal& expressionCase)
{
    LangExpr& lang = getMutableLang();
    lang.type = LangExpr::Type::Case; 
    lang.exprCase.assign(expressionCase.getLang());
}

Expression::CaseInternal Expression::Case(const Expression& expression)
{
    return Expression::CaseInternal(expression);
}

Expression::CaseInternal::CaseInternal(const Expression& expression)
{
    LangExprCase& lang = getMutableLang();
    lang.exprCase.assign(expression.getLang());
}

Expression::CaseInternal& Expression::CaseInternal::whenAndThen(const Expression &when, const Expression &then)
{
    copy_on_write_lazy_lang<LangExprCase::Pair> pair;
    {
        LangExprCase::Pair& langPair = pair.get_or_copy();
        langPair.when.assign(when.getLang());
        langPair.then.assign(then.getLang());
    }        
    LangExprCase& lang = getMutableLang();
    lang.pairs.append(pair);
    return *this;    
}

Expression::CaseInternal& Expression::CaseInternal::else_(const Expression& expression)
{
    LangExprCase& lang = getMutableLang();
    lang.exprElse.assign(expression.getLang());
    return *this;
}    

Expression::Expression(const RaiseFunction& raiseFunction)
{
    LangExpr& lang = getMutableLang();
    lang.type = LangExpr::Type::RaiseFunction;
    lang.raiseFunction.assign(raiseFunction.getLang());
}
