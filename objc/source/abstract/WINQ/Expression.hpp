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
#include <WCDB/Operable.hpp>
#include <WCDB/Redirectable.hpp>

namespace WCDB {

class Expression : public DescribableWithLang<lang::Expr>,
                   public Operable,
                   public Redirectable {
public:
    template <typename T>
    Expression(const T &t,
               typename std::enable_if<LiteralValueConvertible<T>::value>::type
                   * = nullptr)
    {
        setLiteralValue(LiteralValueConvertible<T>::as(t));
    }

    Expression(const LiteralValue &literalValue);

    Expression(const BindParameter &bindParameter);

    Expression(const Column &column);
    Expression &withTable(const std::string &tableName);
    Expression &withSchema(const std::string &schemaName);

    static Expression Function(const std::string &functionName);
    static Expression Function(const std::string &functionName,
                               const Expression &parameter,
                               bool distinct = false);
    static Expression Function(const std::string &functionName,
                               const std::list<Expression> &parameters,
                               bool distinct = false);

    class All : public DescribableWithLang<lang::ExprFunction>,
                public FunctionOperable {
    public:
        virtual Expression
        function(const std::string &functionName,
                 __unused bool distinct = false) const override;
    };

    Expression(const std::list<Expression> &expressions);

    Expression &withEscape(const Expression &expr);

    static Expression Exists(const StatementSelect &selectSTMT);
    static Expression NotExists(const StatementSelect &selectSTMT);
    Expression(const StatementSelect &selectSTMT,
               bool isNot = false,
               bool exists = false);

    class CaseInternal : public DescribableWithLang<lang::ExprCase> {
    public:
        CaseInternal();
        CaseInternal(const Expression &expression);
        CaseInternal &whenAndThen(const Expression &when,
                                  const Expression &then);
        CaseInternal &else_(const Expression &expression);
    };
    static Expression::CaseInternal Case(const Expression &expression);
    static Expression::CaseInternal Case();
    Expression(const CaseInternal &expressionCase);

    Expression(const RaiseFunction &raiseFunction);

    Expression(const lang::copy_on_write_lazy_lang<lang::Expr> &expr);

protected:
    friend class Operable;
    Expression(const lang::copy_on_write_lazy_lang<lang::ExprUnaryOperation>
                   &exprUnaryOperation);
    Expression(const lang::copy_on_write_lazy_lang<lang::ExprBinaryOperation>
                   &exprBinaryOperation);
    Expression(
        const lang::copy_on_write_lazy_lang<lang::ExprFunction> &exprFunction);
    Expression(const lang::copy_on_write_lazy_lang<lang::ExprCast> &exprCast);
    Expression(
        const lang::copy_on_write_lazy_lang<lang::ExprPattern> &exprPattern);
    Expression(const lang::copy_on_write_lazy_lang<lang::ExprNull> &exprNull);
    Expression(
        const lang::copy_on_write_lazy_lang<lang::ExprBetween> &exprBetween);
    Expression(const lang::copy_on_write_lazy_lang<lang::ExprIn> &exprIn);
    Expression(
        const lang::copy_on_write_lazy_lang<lang::ExprExists> &exprExists);
    Expression(
        const lang::copy_on_write_lazy_lang<lang::ExprCollate> &exprCollate);

    virtual lang::copy_on_write_lazy_lang<lang::Expr>
    getExpressionLang() const override;

    virtual Expression getRedirectSource() const override;

    void setLiteralValue(const LiteralValue &literalValue);
};

} // namespace WCDB

#endif /* Expression_hpp */
