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

StatementDelete& StatementDelete::with(const WithClause& withClause)
{
    LangDeleteSTMT& lang = getMutableLang();
    lang.withClause.assign(withClause.getLang());
    return *this;
}

StatementDelete& StatementDelete::deleteFrom(const QualifiedTableName& qualifiedTableName)
{
    LangDeleteSTMT& lang = getMutableLang();
    lang.qualifiedTableName.assign(qualifiedTableName.getLang());
    return *this;
}

StatementDelete& StatementDelete::where(const Expression& condition)
{
    LangDeleteSTMT& lang = getMutableLang();
    lang.condition.assign(condition.getLang());
    return *this;
}

StatementDelete& StatementDelete::orderBy(const OrderingTerm& orderingTerm)
{
    LangDeleteSTMT& lang = getMutableLang();
    lang.orderingTerms.append(orderingTerm.getLang());
    return *this;
}

StatementDelete& StatementDelete::orderBy(const std::list<OrderingTerm>& orderingTerms)
{
    LangDeleteSTMT& lang = getMutableLang();
    for (const OrderingTerm& orderingTerm : orderingTerms) {
        lang.orderingTerms.append(orderingTerm.getLang());
    }
    return *this;
}

StatementDelete& StatementDelete::limit(const Expression& from, const Expression& to)
{
    LangDeleteSTMT& lang = getMutableLang();
    lang.offset = false;
    lang.limit.assign(from.getLang());
    lang.limitParameter.assign(to.getLang());
    return *this;
}

StatementDelete& StatementDelete::limit(const Expression& limit)
{
    LangDeleteSTMT& lang = getMutableLang();
    lang.offset = true;
    lang.limit.assign(limit.getLang());
    return *this;
}

StatementDelete& StatementDelete::offset(const Expression& offset)
{
    LangDeleteSTMT& lang = getMutableLang();
    lang.limitParameter.assign(offset.getLang());
    return *this;
}

Statement::Type StatementDelete::getType() const
{
    return Statement::Type::Delete;
}
