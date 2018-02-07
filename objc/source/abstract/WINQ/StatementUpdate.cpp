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

StatementUpdate &StatementUpdate::with(const WithClause &withClause)
{
    lang::UpdateSTMT &lang = getMutableLang();
    lang.withClause.assign(withClause.getLang());
    return *this;
}

StatementUpdate &
StatementUpdate::update(const QualifiedTableName &qualifiedTableName)
{
    update(qualifiedTableName, lang::UpdateSTMT::Type::Update);
    return *this;
}

StatementUpdate &
StatementUpdate::updateOrRollback(const QualifiedTableName &qualifiedTableName)
{
    update(qualifiedTableName, lang::UpdateSTMT::Type::UpdateOrRollback);
    return *this;
}

StatementUpdate &
StatementUpdate::updateOrAbort(const QualifiedTableName &qualifiedTableName)
{
    update(qualifiedTableName, lang::UpdateSTMT::Type::UpdateOrAbort);
    return *this;
}

StatementUpdate &
StatementUpdate::updateOrReplace(const QualifiedTableName &qualifiedTableName)
{
    update(qualifiedTableName, lang::UpdateSTMT::Type::UpdateOrReplace);
    return *this;
}

StatementUpdate &
StatementUpdate::updateOrFail(const QualifiedTableName &qualifiedTableName)
{
    update(qualifiedTableName, lang::UpdateSTMT::Type::UpdateOrFail);
    return *this;
}

StatementUpdate &
StatementUpdate::updateOrIgnore(const QualifiedTableName &qualifiedTableName)
{
    update(qualifiedTableName, lang::UpdateSTMT::Type::UpdateOrIgnore);
    return *this;
}

StatementUpdate &StatementUpdate::set(const std::string &columnName,
                                      const Expression &expression)
{
    lang::copy_on_write_lazy_lang<lang::UpdateSTMT::KeyValue> cowKeyValue;
    lang::UpdateSTMT::KeyValue &keyValue = cowKeyValue.get_or_copy();
    keyValue.keys.append(columnName);
    keyValue.value.assign(expression.getLang());

    lang::UpdateSTMT &lang = getMutableLang();
    lang.keyValues.append(cowKeyValue);
    return *this;
}

StatementUpdate &StatementUpdate::set(const std::list<std::string> &columnNames,
                                      const Expression &expression)
{
    lang::copy_on_write_lazy_lang<lang::UpdateSTMT::KeyValue> cowKeyValue;
    lang::UpdateSTMT::KeyValue &keyValue = cowKeyValue.get_or_copy();
    for (const std::string &columnName : columnNames) {
        keyValue.keys.append(columnName);
    }
    keyValue.value.assign(expression.getLang());

    lang::UpdateSTMT &lang = getMutableLang();
    lang.keyValues.append(cowKeyValue);
    return *this;
}

StatementUpdate &StatementUpdate::where(const Expression &condition)
{
    lang::UpdateSTMT &lang = getMutableLang();
    lang.condition.assign(condition.getLang());
    return *this;
}

StatementUpdate &StatementUpdate::orderBy(const OrderingTerm &orderingTerm)
{
    lang::UpdateSTMT &lang = getMutableLang();
    lang.orderingTerm.append(orderingTerm.getLang());
    return *this;
}

StatementUpdate &
StatementUpdate::orderBy(const std::list<OrderingTerm> &orderingTerms)
{
    lang::UpdateSTMT &lang = getMutableLang();
    for (const OrderingTerm &orderingTerm : orderingTerms) {
        lang.orderingTerm.append(orderingTerm.getLang());
    }
    return *this;
}

StatementUpdate &StatementUpdate::limit(const Expression &from,
                                        const Expression &to)
{
    lang::UpdateSTMT &lang = getMutableLang();
    lang.offset = false;
    lang.limit.assign(from.getLang());
    lang.limitParameter.assign(to.getLang());
    return *this;
}

StatementUpdate &StatementUpdate::limit(const Expression &limit)
{
    lang::UpdateSTMT &lang = getMutableLang();
    lang.offset = true;
    lang.limit.assign(limit.getLang());
    return *this;
}

StatementUpdate &StatementUpdate::offset(const Expression &offset)
{
    lang::UpdateSTMT &lang = getMutableLang();
    lang.limitParameter.assign(offset.getLang());
    return *this;
}

Statement::Type StatementUpdate::getType() const
{
    return Statement::Type::Update;
}

void StatementUpdate::update(const QualifiedTableName &qualifiedTableName,
                             const lang::UpdateSTMT::Type &type)
{
    lang::UpdateSTMT &lang = getMutableLang();
    lang.qualifiedTableName.assign(qualifiedTableName.getLang());
    lang.type = type;
}

} // namespace WCDB
