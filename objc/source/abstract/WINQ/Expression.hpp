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

class Expression : public DescribableWithLang<Lang::Expr>, public Operable, public Redirectable {
public:
    Expression();

    template<typename T>
    Expression(const T &t,
               typename std::enable_if<LiteralValueConvertible<T>::value>::type * = nullptr)
    {
        setLiteralValue(LiteralValueConvertible<T>::as(t));
    }

    Expression(const LiteralValue &literalValue);

    Expression(const BindParameter &bindParameter);

    Expression(const Column &column);
    Expression &withTable(const std::string &tableName);
    Expression &withSchema(const std::string &schemaName);

    static Expression function(const std::string &functionName,
                               bool distinct,
                               const std::list<Expression> &parameters);
    static const Expression &lastInsertedRowid();

    class All : public DescribableWithLang<Lang::ExprFunction>, public FunctionOperable {
    public:
        static const Expression::All &default_();
        Expression function(const std::string &functionName,
                            __unused bool distinct = false) const override;
        Expression function(const std::string &functionName,
                            const std::list<Expression> &otherParameters,
                            bool distinct) const override;
    };

    Expression(const std::list<Expression> &expressions);

    Expression &escape(const Expression &expr);

    static Expression Exists(const StatementSelect &selectSTMT);
    static Expression NotExists(const StatementSelect &selectSTMT);
    Expression(const StatementSelect &selectSTMT, bool isNot = false, bool exists = false);

    class CaseInternal : public DescribableWithLang<Lang::ExprCase> {
    public:
        CaseInternal();
        CaseInternal(const Expression &expression);
        //TODO refactor -> when(...).then(...)
        CaseInternal &whenAndThen(const Expression &when, const Expression &then);
        CaseInternal &else_(const Expression &expression);
    };
    static Expression::CaseInternal case_(const Expression &expression);
    static Expression::CaseInternal case_();
    Expression(const CaseInternal &expressionCase);

    Expression(const RaiseFunction &raiseFunction);

    Expression(const Lang::CopyOnWriteLazyLang<Lang::Expr> &expr);

    operator std::list<WCDB::Expression>() const;

protected:
    friend class Operable;
    Expression(const Lang::CopyOnWriteLazyLang<Lang::ExprUnaryOperation> &exprUnaryOperation);
    Expression(const Lang::CopyOnWriteLazyLang<Lang::ExprBinaryOperation> &exprBinaryOperation);
    Expression(const Lang::CopyOnWriteLazyLang<Lang::ExprFunction> &exprFunction);
    Expression(const Lang::CopyOnWriteLazyLang<Lang::ExprCast> &exprCast);
    Expression(const Lang::CopyOnWriteLazyLang<Lang::ExprPattern> &exprPattern);
    Expression(const Lang::CopyOnWriteLazyLang<Lang::ExprNull> &exprNull);
    Expression(const Lang::CopyOnWriteLazyLang<Lang::ExprBetween> &exprBetween);
    Expression(const Lang::CopyOnWriteLazyLang<Lang::ExprIn> &exprIn);
    Expression(const Lang::CopyOnWriteLazyLang<Lang::ExprExists> &exprExists);
    Expression(const Lang::CopyOnWriteLazyLang<Lang::ExprCollate> &exprCollate);

    Lang::CopyOnWriteLazyLang<Lang::Expr> getExpressionLang() const override;

    Expression getRedirectSource() const override;

    void setLiteralValue(const LiteralValue &literalValue);
};

} // namespace WCDB

#endif /* Expression_hpp */
