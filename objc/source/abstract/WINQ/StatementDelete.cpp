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

#include <WCDB/Assertion.hpp>
#include <WCDB/WINQ.h>

namespace WCDB {

StatementDelete &StatementDelete::with(const WithClause &withClause)
{
    Lang::DeleteSTMT &lang = getMutableLang();
    lang.withClause.assign(withClause.getCOWLang());
    return *this;
}

StatementDelete &
StatementDelete::deleteFrom(const QualifiedTableName &qualifiedTableName)
{
    Lang::DeleteSTMT &lang = getMutableLang();
    lang.qualifiedTableName.assign(qualifiedTableName.getCOWLang());
    return *this;
}

StatementDelete &StatementDelete::where(const Expression &condition)
{
    Lang::DeleteSTMT &lang = getMutableLang();
    lang.condition.assign(condition.getCOWLang());
    return *this;
}

StatementDelete &StatementDelete::orderBy(const OrderingTerm &orderingTerm)
{
    Lang::DeleteSTMT &lang = getMutableLang();
    lang.orderingTerms.append(orderingTerm.getCOWLang());
    return *this;
}

StatementDelete &
StatementDelete::orderBy(const std::list<OrderingTerm> &orderingTerms)
{
    Lang::DeleteSTMT &lang = getMutableLang();
    for (const OrderingTerm &orderingTerm : orderingTerms) {
        lang.orderingTerms.append(orderingTerm.getCOWLang());
    }
    return *this;
}

StatementDelete &StatementDelete::limit(const Expression &from,
                                        const Expression &to)
{
    Lang::DeleteSTMT &lang = getMutableLang();
    lang.offset = false;
    lang.limit.assign(from.getCOWLang());
    lang.limitParameter.assign(to.getCOWLang());
    return *this;
}

StatementDelete &StatementDelete::limit(const Expression &limit)
{
    Lang::DeleteSTMT &lang = getMutableLang();
    lang.offset = true;
    lang.limit.assign(limit.getCOWLang());
    return *this;
}

StatementDelete &StatementDelete::offset(const Expression &offset)
{
    Lang::DeleteSTMT &lang = getMutableLang();
    lang.limitParameter.assign(offset.getCOWLang());
    return *this;
}

bool StatementDelete::isLimited() const
{
    const Lang::DeleteSTMT &lang = getCOWLang().get<Lang::DeleteSTMT>();
    return !lang.orderingTerms.empty() || !lang.limit.empty();
}

} // namespace WCDB
