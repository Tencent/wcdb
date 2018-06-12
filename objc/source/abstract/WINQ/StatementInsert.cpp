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

StatementInsert &StatementInsert::with(const WithClause &withClause)
{
    Lang::InsertSTMT &lang = getMutableLang();
    lang.withClause.assign(withClause.getCOWLang());
    return *this;
}

StatementInsert &StatementInsert::insertInto(const std::string &tableName)
{
    insertInto(tableName, Lang::InsertSTMT::Type::Insert);
    return *this;
}

StatementInsert &
StatementInsert::insertOrReplaceInto(const std::string &tableName)
{
    insertInto(tableName, Lang::InsertSTMT::Type::InsertOrReplace);
    return *this;
}

StatementInsert &
StatementInsert::insertOrRollbackInto(const std::string &tableName)
{
    insertInto(tableName, Lang::InsertSTMT::Type::InsertOrRollback);
    return *this;
}

StatementInsert &
StatementInsert::insertOrAbortInto(const std::string &tableName)
{
    insertInto(tableName, Lang::InsertSTMT::Type::InsertOrAbort);
    return *this;
}

StatementInsert &StatementInsert::insertOrFailInto(const std::string &tableName)
{
    insertInto(tableName, Lang::InsertSTMT::Type::InsertOrFail);
    return *this;
}

StatementInsert &
StatementInsert::insertOrIgnoreInto(const std::string &tableName)
{
    insertInto(tableName, Lang::InsertSTMT::Type::InsertOrIgnore);
    return *this;
}

StatementInsert &StatementInsert::withSchema(const std::string &schemaName)
{
    Lang::InsertSTMT &lang = getMutableLang();
    lang.schemaName.assign(schemaName);
    return *this;
}

StatementInsert &StatementInsert::on(const Column &column)
{
    Lang::InsertSTMT &lang = getMutableLang();
    lang.columns.append(column.getCOWLang());
    return *this;
}

StatementInsert &StatementInsert::on(const std::list<Column> &columns)
{
    Lang::InsertSTMT &lang = getMutableLang();
    for (const Column &column : columns) {
        lang.columns.append(column.getCOWLang());
    }
    return *this;
}

StatementInsert &StatementInsert::values(const Expression &value)
{
    Lang::InsertSTMT &lang = getMutableLang();
    lang.switcher = Lang::InsertSTMT::Switch::Values;
    lang.exprs.append(value.getCOWLang());
    return *this;
}

StatementInsert &StatementInsert::values(const std::list<Expression> &values)
{
    Lang::InsertSTMT &lang = getMutableLang();
    lang.switcher = Lang::InsertSTMT::Switch::Values;
    for (const Expression &value : values) {
        lang.exprs.append(value.getCOWLang());
    }
    return *this;
}

StatementInsert &StatementInsert::values(const StatementSelect &selectSTMT)
{
    Lang::InsertSTMT &lang = getMutableLang();
    lang.switcher = Lang::InsertSTMT::Switch::Select;
    lang.selectSTMT.assign(selectSTMT.getCOWLang());
    return *this;
}

StatementInsert &StatementInsert::defaultValues()
{
    Lang::InsertSTMT &lang = getMutableLang();
    lang.switcher = Lang::InsertSTMT::Switch::DefaultValues;
    return *this;
}

void StatementInsert::insertInto(const std::string &tableName,
                                 const Lang::InsertSTMT::Type &type)
{
    Lang::InsertSTMT &lang = getMutableLang();
    lang.type = type;
    lang.tableName.assign(tableName);
}

bool StatementInsert::isValuesNotSet() const
{
    const Lang::InsertSTMT &lang = getCOWLang().get<Lang::InsertSTMT>();
    switch (lang.switcher) {
        case Lang::InsertSTMT::Switch::Values:
            return lang.exprs.empty();
        case Lang::InsertSTMT::Switch::NotSet:
            return true;
        default:
            return false;
    }
}

bool StatementInsert::isColumnsNotSet() const
{
    return getCOWLang().get<Lang::InsertSTMT>().columns.empty();
}

bool StatementInsert::isReplace() const
{
    const Lang::InsertSTMT &lang = getCOWLang().get<Lang::InsertSTMT>();
    return lang.type == Lang::InsertSTMT::Type::InsertOrReplace;
}

} // namespace WCDB
