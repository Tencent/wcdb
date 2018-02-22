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

#include <WINQ/WINQ.h>

namespace WCDB {

StatementInsert &StatementInsert::with(const WithClause &withClause)
{
    lang::InsertSTMT &lang = getMutableLang();
    lang.withClause.assign(withClause.getLang());
    return *this;
}

StatementInsert &StatementInsert::insertInto(const std::string &tableName)
{
    insertInto(tableName, lang::InsertSTMT::Type::Insert);
    return *this;
}

StatementInsert &
StatementInsert::insertOrReplaceInto(const std::string &tableName)
{
    insertInto(tableName, lang::InsertSTMT::Type::InsertOrReplace);
    return *this;
}

StatementInsert &
StatementInsert::insertOrRollbackInto(const std::string &tableName)
{
    insertInto(tableName, lang::InsertSTMT::Type::InsertOrRollback);
    return *this;
}

StatementInsert &
StatementInsert::insertOrAbortInto(const std::string &tableName)
{
    insertInto(tableName, lang::InsertSTMT::Type::InsertOrAbort);
    return *this;
}

StatementInsert &StatementInsert::insertOrFailInto(const std::string &tableName)
{
    insertInto(tableName, lang::InsertSTMT::Type::InsertOrFail);
    return *this;
}

StatementInsert &
StatementInsert::insertOrIgnoreInto(const std::string &tableName)
{
    insertInto(tableName, lang::InsertSTMT::Type::InsertOrIgnore);
    return *this;
}

StatementInsert &StatementInsert::withSchema(const std::string &schemaName)
{
    lang::InsertSTMT &lang = getMutableLang();
    lang.schemaName.assign(schemaName);
    return *this;
}

StatementInsert &StatementInsert::on(const Column &column)
{
    lang::InsertSTMT &lang = getMutableLang();
    lang.columns.append(column.getLang());
    return *this;
}

StatementInsert &StatementInsert::on(const std::list<Column> &columns)
{
    lang::InsertSTMT &lang = getMutableLang();
    for (const Column &column : columns) {
        lang.columns.append(column.getLang());
    }
    return *this;
}

StatementInsert &StatementInsert::values(const Expression &value)
{
    lang::InsertSTMT &lang = getMutableLang();
    lang.switcher = lang::InsertSTMT::Switch::Values;
    lang.exprs.append(value.getLang());
    return *this;
}

StatementInsert &StatementInsert::values(const std::list<Expression> &values)
{
    lang::InsertSTMT &lang = getMutableLang();
    lang.switcher = lang::InsertSTMT::Switch::Values;
    for (const Expression &value : values) {
        lang.exprs.append(value.getLang());
    }
    return *this;
}

StatementInsert &StatementInsert::values(const StatementSelect &selectSTMT)
{
    lang::InsertSTMT &lang = getMutableLang();
    lang.switcher = lang::InsertSTMT::Switch::Select;
    lang.selectSTMT.assign(selectSTMT.getLang());
    return *this;
}

StatementInsert &StatementInsert::defaultValues()
{
    lang::InsertSTMT &lang = getMutableLang();
    lang.switcher = lang::InsertSTMT::Switch::DefaultValues;
    return *this;
}

Statement::Type StatementInsert::getType() const
{
    return Statement::Type::Insert;
}

void StatementInsert::insertInto(const std::string &tableName,
                                 const lang::InsertSTMT::Type &type)
{
    lang::InsertSTMT &lang = getMutableLang();
    lang.type = type;
    lang.tableName.assign(tableName);
}

} // namespace WCDB
