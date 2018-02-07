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

#ifndef Expression_hpp
#define Expression_hpp

#include <WCDB/Describable.hpp>

class Expression : public DescribableWithLang<LangExpr> {
public:
    Expression(const LiteralValue& literalValue);
    
    Expression(const BindParameter& bindParameter);
    
    class Column : public DescribableWithLang<LangExprColumn> {
    public:
        Column(const std::string& columnName);
        Column& withTable(const std::string& tableName);
        Column& withSchema(const std::string& schemaName);
    };
    static Expression::Column ColumnNamed(const std::string& columnName);
    Expression(const Expression::Column& expressionColumn);
    
    Expression operator - () const;
    Expression operator + () const;
    Expression operator ! () const;
    Expression operator ~ () const;
    
    Expression concat(const Expression& operand) const;
    Expression operator * (const Expression& operand) const;
    Expression operator / (const Expression& operand) const;
    Expression operator % (const Expression& operand) const;
    Expression operator + (const Expression& operand) const;
    Expression operator - (const Expression& operand) const;
    Expression operator << (const Expression& operand) const;
    Expression operator >> (const Expression& operand) const;
    Expression operator & (const Expression& operand) const;
    Expression operator | (const Expression& operand) const;
    Expression operator < (const Expression& operand) const;
    Expression operator <= (const Expression& operand) const;
    Expression operator > (const Expression& operand) const;
    Expression operator >= (const Expression& operand) const;
    Expression operator == (const Expression& operand) const;
    Expression operator != (const Expression& operand) const;
    Expression is(const Expression& operand) const;
    Expression isNot(const Expression& operand) const;
    Expression operator && (const Expression& operand) const;
    Expression operator || (const Expression& operand) const;
    
    static Expression Function(const std::string& functionName);
    static Expression Function(const std::string& functionName, const Expression& parameter, bool distinct = false);
    static Expression Function(const std::string& functionName, const std::list<Expression>& parameters, bool distinct = false);
    static Expression FunctionAll(const std::string& functionName);

    Expression(const std::list<Expression>& expressions);
    
    static Expression Cast(const Expression& expression, const ColumnType& columnType);
    
    Expression& withCollate(const std::string& collationName);
    
    Expression like(const Expression& expr) const;
    Expression glob(const Expression& expr) const;
    Expression regexp(const Expression& expr) const;
    Expression match(const Expression& expr) const;
    Expression notLike(const Expression& expr) const;
    Expression notGlob(const Expression& expr) const;
    Expression notRegexp(const Expression& expr) const;
    Expression notMatch(const Expression& expr) const;
    Expression& withEscape(const Expression& expr);
    
    Expression isNull() const;
    Expression notNull() const;
    
    Expression between(const Expression& left, const Expression& right) const;
    Expression notBetween(const Expression& left, const Expression& right) const;
    
    Expression in(const StatementSelect& selectSTMT);
    Expression notIn(const StatementSelect& selectSTMT);
    Expression in(const Expression& expression);
    Expression notIn(const Expression& expression);
    Expression in(const std::list<Expression>& expressions);
    Expression notIn(const std::list<Expression>& expressions);
    Expression in(const std::string& tableOrFunctionName);
    Expression notIn(const std::string& tableOrFunctionName);
    Expression in(const std::string& schemaName, const std::string& tableOrFunctionName);
    Expression notIn(const std::string& schemaName, const std::string& tableOrFunctionName);
    Expression in(const std::string& schemaName, const std::string& functionName, const Expression& expression);
    Expression notIn(const std::string& schemaName, const std::string& functionName, const Expression& expression);
    Expression in(const std::string& schemaName, const std::string& functionName, const std::list<Expression>& expressions);
    Expression notIn(const std::string& schemaName, const std::string& functionName, const std::list<Expression>& expressions);
    
    static Expression Exists(const StatementSelect& selectSTMT);
    static Expression NotExists(const StatementSelect& selectSTMT);
        
    class CaseInternal : public DescribableWithLang<LangExprCase> {
    public:
        CaseInternal(const Expression& expression);
        CaseInternal& whenAndThen(const Expression& when, const Expression& then);
        CaseInternal& else_(const Expression& expression);
    };    
    static Expression::CaseInternal Case(const Expression& expression);
    Expression(const CaseInternal& expressionCase);
    
    Expression(const RaiseFunction& raiseFunction);
protected:
    Expression(const copy_on_write_lazy_lang<LangExprUnaryOperation>& exprUnaryOperation);
    Expression(const copy_on_write_lazy_lang<LangExprBinaryOperation>& exprBinaryOperation);
    Expression(const copy_on_write_lazy_lang<LangExprFunction>& exprFunction);
    Expression(const copy_on_write_lazy_lang<LangExprCast>& exprCast);
    Expression(const copy_on_write_lazy_lang<LangExprPattern>& exprPattern);
    Expression(const copy_on_write_lazy_lang<LangExprNull>& exprNull);
    Expression(const copy_on_write_lazy_lang<LangExprBetween>& exprBetween);
    Expression(const copy_on_write_lazy_lang<LangExprIn>& exprIn);    
    Expression(const copy_on_write_lazy_lang<LangExprExists>& exprExists);    
    
    copy_on_write_lazy_lang<LangExprUnaryOperation> unaryOperation(const LangExprUnaryOperation::Operator& unaryOperator) const;
    copy_on_write_lazy_lang<LangExprBinaryOperation> binaryOperation(const LangExprBinaryOperation::Operator& binaryOperator, const Expression& operand) const; 
    copy_on_write_lazy_lang<LangExprPattern> pattern(const LangExprPattern::Type& type, const Expression& operand, bool isNot) const;
    copy_on_write_lazy_lang<LangExprNull> null(bool isNull) const;
    copy_on_write_lazy_lang<LangExprBetween> between(const Expression& left, const Expression& right, bool isNot) const;
    
    copy_on_write_lazy_lang<LangExprIn> in(const StatementSelect& selectSTMT, bool isNot);
    copy_on_write_lazy_lang<LangExprIn> in(const Expression& expression, bool isNot);
    copy_on_write_lazy_lang<LangExprIn> in(const std::list<Expression>& expressions, bool isNot);
    copy_on_write_lazy_lang<LangExprIn> in(const std::string& schemaName, const std::string& tableOrFunctionName, bool isNot);
    copy_on_write_lazy_lang<LangExprIn> in(const std::string& schemaName, const std::string& tableFunctionName, const Expression& expression, bool isNot);
    copy_on_write_lazy_lang<LangExprIn> in(const std::string& schemaName, const std::string& tableFunctionName, const std::list<Expression>& expressions, bool isNot);
    
    static copy_on_write_lazy_lang<LangExprExists> Exists(const StatementSelect& selectSTMT, bool isNot);
    
};

#endif /* Expression_hpp */
