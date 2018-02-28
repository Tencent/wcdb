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

StatementCreateTrigger::StatementCreateTrigger()
{
    getMutableLang().ifNotExists = true;
}

StatementCreateTrigger &
StatementCreateTrigger::createTrigger(const std::string &triggerName)
{
    Lang::CreateTriggerSTMT &lang = getMutableLang();
    lang.triggerName.assign(triggerName);
    return *this;
}

StatementCreateTrigger &
StatementCreateTrigger::withSchema(const std::string &schemaName)
{
    Lang::CreateTriggerSTMT &lang = getMutableLang();
    lang.schemaName.assign(schemaName);
    return *this;
}

StatementCreateTrigger &StatementCreateTrigger::ifNotExists(bool ifNotExists)
{
    Lang::CreateTriggerSTMT &lang = getMutableLang();
    lang.ifNotExists = ifNotExists;
    return *this;
}

StatementCreateTrigger &StatementCreateTrigger::temp(bool temp)
{
    Lang::CreateTriggerSTMT &lang = getMutableLang();
    lang.temp = temp;
    return *this;
}

StatementCreateTrigger &StatementCreateTrigger::before()
{
    Lang::CreateTriggerSTMT &lang = getMutableLang();
    lang.type = Lang::CreateTriggerSTMT::Type::Before;
    return *this;
}

StatementCreateTrigger &StatementCreateTrigger::after()
{
    Lang::CreateTriggerSTMT &lang = getMutableLang();
    lang.type = Lang::CreateTriggerSTMT::Type::After;
    return *this;
}

StatementCreateTrigger &StatementCreateTrigger::insteadOf()
{
    Lang::CreateTriggerSTMT &lang = getMutableLang();
    lang.type = Lang::CreateTriggerSTMT::Type::InsteadOf;
    return *this;
}

StatementCreateTrigger &StatementCreateTrigger::delete_()
{
    Lang::CreateTriggerSTMT &lang = getMutableLang();
    lang.operation = Lang::CreateTriggerSTMT::Operation::Delete;
    return *this;
}

StatementCreateTrigger &StatementCreateTrigger::insert()
{
    Lang::CreateTriggerSTMT &lang = getMutableLang();
    lang.operation = Lang::CreateTriggerSTMT::Operation::Insert;
    return *this;
}

StatementCreateTrigger &StatementCreateTrigger::update()
{
    Lang::CreateTriggerSTMT &lang = getMutableLang();
    lang.operation = Lang::CreateTriggerSTMT::Operation::Update;
    return *this;
}

StatementCreateTrigger &StatementCreateTrigger::updateOf(const Column &column)
{
    Lang::CreateTriggerSTMT &lang = getMutableLang();
    lang.operation = Lang::CreateTriggerSTMT::Operation::Update;
    lang.columns.append(column.getLang());
    return *this;
}

StatementCreateTrigger &
StatementCreateTrigger::updateOf(const std::list<Column> &columns)
{
    Lang::CreateTriggerSTMT &lang = getMutableLang();
    lang.operation = Lang::CreateTriggerSTMT::Operation::Update;
    for (const Column &column : columns) {
        lang.columns.append(column.getLang());
    }
    return *this;
}

StatementCreateTrigger &StatementCreateTrigger::on(const std::string &tableName)
{
    Lang::CreateTriggerSTMT &lang = getMutableLang();
    lang.tableName.assign(tableName);
    return *this;
}

StatementCreateTrigger &StatementCreateTrigger::forEachRow()
{
    Lang::CreateTriggerSTMT &lang = getMutableLang();
    lang.forEachRow = true;
    return *this;
}

StatementCreateTrigger &
StatementCreateTrigger::when(const Expression &expression)
{
    Lang::CreateTriggerSTMT &lang = getMutableLang();
    lang.expr.assign(expression.getLang());
    return *this;
}

StatementCreateTrigger &
StatementCreateTrigger::run(const CRUDStatement &statement)
{
    Lang::CreateTriggerSTMT &lang = getMutableLang();
    lang.STMTs.append(statement.getCRUDLang());
    return *this;
}

Statement::Type StatementCreateTrigger::getType() const
{
    return Statement::Type::CreateTrigger;
}

} // namespace WCDB
