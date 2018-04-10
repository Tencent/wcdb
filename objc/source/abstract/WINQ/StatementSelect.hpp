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

#ifndef StatementSelect_hpp
#define StatementSelect_hpp

#include <WCDB/Describable.hpp>
#include <WCDB/Redirectable.hpp>
#include <WCDB/Statement.hpp>

namespace WCDB {

class StatementSelect : public CRUDStatementWithLang<Lang::SelectSTMT>,
                        public Redirectable {
public:
    StatementSelect &with(const CommonTableExpression &commonTableExpression);
    StatementSelect &
    with(const std::list<CommonTableExpression> &commonTableExpressions);
    StatementSelect &
    withRecursive(const CommonTableExpression &commonTableExpression);
    StatementSelect &withRecursive(
        const std::list<CommonTableExpression> &commonTableExpressions);

    StatementSelect &distinct();
    StatementSelect &select(const ResultColumn &resultColumn);
    StatementSelect &select(const std::list<ResultColumn> &resultColumns);
    StatementSelect &from(const TableOrSubquery &tableOrSubquery);
    StatementSelect &from(const JoinClause &joinClause);
    StatementSelect &from(const std::list<TableOrSubquery> &tableOrSubquerys);
    StatementSelect &where(const Expression &condition);
    StatementSelect &groupBy(const Expression &group);
    StatementSelect &groupBy(const std::list<Expression> &groups);
    StatementSelect &having(const Expression &having);
    StatementSelect &values(const Expression &value);
    StatementSelect &values(const std::list<Expression> &values);

    StatementSelect &union_(const SelectCore &selectCore);
    StatementSelect &unionAll(const SelectCore &selectCore);
    StatementSelect &intersect(const SelectCore &selectCore);
    StatementSelect &except(const SelectCore &selectCore);

    StatementSelect &orderBy(const OrderingTerm &orderingTerm);
    StatementSelect &orderBy(const std::list<OrderingTerm> &orderingTerms);

    StatementSelect &limit(const Expression &from, const Expression &to);
    StatementSelect &limit(const Expression &limit);
    StatementSelect &offset(const Expression &offset);

    bool isResultColumnsNotSet() const;

protected:
    virtual Expression getRedirectSource() const override;

    void compound(const Lang::SelectSTMT::Compound::Operator &compoundOperator,
                  const SelectCore &selectCore);
};

} // namespace WCDB

#endif /* StatementSelect_hpp */
