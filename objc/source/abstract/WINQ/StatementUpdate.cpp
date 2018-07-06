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

StatementUpdate &StatementUpdate::with(const WithClause &withClause)
{
    Lang::UpdateSTMT &lang = getMutableLang();
    lang.withClause.assign(withClause.getCOWLang());
    return *this;
}

StatementUpdate &StatementUpdate::update(const QualifiedTableName &qualifiedTableName)
{
    update(qualifiedTableName, Lang::UpdateSTMT::Type::Update);
    return *this;
}

StatementUpdate &StatementUpdate::updateOrRollback(const QualifiedTableName &qualifiedTableName)
{
    update(qualifiedTableName, Lang::UpdateSTMT::Type::UpdateOrRollback);
    return *this;
}

StatementUpdate &StatementUpdate::updateOrAbort(const QualifiedTableName &qualifiedTableName)
{
    update(qualifiedTableName, Lang::UpdateSTMT::Type::UpdateOrAbort);
    return *this;
}

StatementUpdate &StatementUpdate::updateOrReplace(const QualifiedTableName &qualifiedTableName)
{
    update(qualifiedTableName, Lang::UpdateSTMT::Type::UpdateOrReplace);
    return *this;
}

StatementUpdate &StatementUpdate::updateOrFail(const QualifiedTableName &qualifiedTableName)
{
    update(qualifiedTableName, Lang::UpdateSTMT::Type::UpdateOrFail);
    return *this;
}

StatementUpdate &StatementUpdate::updateOrIgnore(const QualifiedTableName &qualifiedTableName)
{
    update(qualifiedTableName, Lang::UpdateSTMT::Type::UpdateOrIgnore);
    return *this;
}

StatementUpdate &StatementUpdate::set(const Column &column, const Expression &expression)
{
    Lang::CopyOnWriteLazyLang<Lang::UpdateSTMT::KeyValue> cowKeyValue;
    Lang::UpdateSTMT::KeyValue &keyValue = cowKeyValue.get_or_copy();
    keyValue.keys.append(column.getCOWLang());
    keyValue.value.assign(expression.getCOWLang());

    Lang::UpdateSTMT &lang = getMutableLang();
    lang.keyValues.append(cowKeyValue);
    return *this;
}

StatementUpdate &
StatementUpdate::set(const std::list<Column> &columns, const Expression &expression)
{
    Lang::CopyOnWriteLazyLang<Lang::UpdateSTMT::KeyValue> cowKeyValue;
    Lang::UpdateSTMT::KeyValue &keyValue = cowKeyValue.get_or_copy();
    for (const Column &column : columns) {
        keyValue.keys.append(column.getCOWLang());
    }
    keyValue.value.assign(expression.getCOWLang());

    Lang::UpdateSTMT &lang = getMutableLang();
    lang.keyValues.append(cowKeyValue);
    return *this;
}

StatementUpdate &StatementUpdate::where(const Expression &condition)
{
    Lang::UpdateSTMT &lang = getMutableLang();
    lang.condition.assign(condition.getCOWLang());
    return *this;
}

StatementUpdate &StatementUpdate::orderBy(const OrderingTerm &orderingTerm)
{
    Lang::UpdateSTMT &lang = getMutableLang();
    lang.orderingTerms.append(orderingTerm.getCOWLang());
    return *this;
}

StatementUpdate &StatementUpdate::orderBy(const std::list<OrderingTerm> &orderingTerms)
{
    Lang::UpdateSTMT &lang = getMutableLang();
    for (const OrderingTerm &orderingTerm : orderingTerms) {
        lang.orderingTerms.append(orderingTerm.getCOWLang());
    }
    return *this;
}

StatementUpdate &StatementUpdate::limit(const Expression &from, const Expression &to)
{
    Lang::UpdateSTMT &lang = getMutableLang();
    lang.offset = false;
    lang.limit.assign(from.getCOWLang());
    lang.limitParameter.assign(to.getCOWLang());
    return *this;
}

StatementUpdate &StatementUpdate::limit(const Expression &limit)
{
    Lang::UpdateSTMT &lang = getMutableLang();
    lang.offset = true;
    lang.limit.assign(limit.getCOWLang());
    return *this;
}

StatementUpdate &StatementUpdate::offset(const Expression &offset)
{
    Lang::UpdateSTMT &lang = getMutableLang();
    lang.limitParameter.assign(offset.getCOWLang());
    return *this;
}

void StatementUpdate::update(const QualifiedTableName &qualifiedTableName,
                             const Lang::UpdateSTMT::Type &type)
{
    Lang::UpdateSTMT &lang = getMutableLang();
    lang.qualifiedTableName.assign(qualifiedTableName.getCOWLang());
    lang.type = type;
}

bool StatementUpdate::isLimited() const
{
    const Lang::UpdateSTMT &lang = getCOWLang().get<Lang::UpdateSTMT>();
    return !lang.orderingTerms.empty() || !lang.limit.empty();
}

} // namespace WCDB
