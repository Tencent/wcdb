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

StatementCreateTrigger& StatementCreateTrigger::createTrigger(const std::string& triggerName, bool ifNotExists, bool temp)
{
    LangCreateTriggerSTMT& lang = getMutableLang();
    lang.triggerName.assign(triggerName);
    lang.ifNotExists = ifNotExists;
    lang.temp = temp;
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::withSchema(const std::string& schemaName)
{
    LangCreateTriggerSTMT& lang = getMutableLang();
    lang.schemaName.assign(schemaName);
    return *this;
}


StatementCreateTrigger& StatementCreateTrigger::before()
{
    LangCreateTriggerSTMT& lang = getMutableLang();
    lang.type = LangCreateTriggerSTMT::Type::Before; 
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::after()
{
    LangCreateTriggerSTMT& lang = getMutableLang();
    lang.type = LangCreateTriggerSTMT::Type::After;
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::insteadOf()
{
    LangCreateTriggerSTMT& lang = getMutableLang();
    lang.type = LangCreateTriggerSTMT::Type::InsteadOf;
    return *this;
}


StatementCreateTrigger& StatementCreateTrigger::delete_()
{
    LangCreateTriggerSTMT& lang = getMutableLang();
    lang.operation = LangCreateTriggerSTMT::Operation::Delete;
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::insert()
{
    LangCreateTriggerSTMT& lang = getMutableLang();
    lang.operation = LangCreateTriggerSTMT::Operation::Insert;
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::update()
{
    LangCreateTriggerSTMT& lang = getMutableLang();
    lang.operation = LangCreateTriggerSTMT::Operation::Update;
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::updateOf(const std::string& columnName)
{
    LangCreateTriggerSTMT& lang = getMutableLang();
    lang.operation = LangCreateTriggerSTMT::Operation::Update;
    lang.columnNames.append(columnName);
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::updateOf(const std::list<std::string>& columnNames)
{
    LangCreateTriggerSTMT& lang = getMutableLang();
    lang.operation = LangCreateTriggerSTMT::Operation::Update;
    for (const std::string& columnName : columnNames) {
        lang.columnNames.append(columnName);
    }
    return *this;
}


StatementCreateTrigger& StatementCreateTrigger::on(const std::string& tableName)
{
    LangCreateTriggerSTMT& lang = getMutableLang();
    lang.tableName.assign(tableName);
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::forEachRow()
{
    LangCreateTriggerSTMT& lang = getMutableLang();
    lang.forEachRow = true;
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::when(const Expression& expression)
{
    LangCreateTriggerSTMT& lang = getMutableLang();
    lang.expr.assign(expression.getLang());
    return *this;
}


StatementCreateTrigger& StatementCreateTrigger::byAddingTriggeredStatement(const CRUDStatement& CRUDSTMT)
{
    LangCreateTriggerSTMT& lang = getMutableLang();
    lang.STMTs.append(CRUDSTMT.getCRUDLang());;
    return *this;
}

Statement::Type StatementCreateTrigger::getType() const 
{
    return Statement::Type::CreateTrigger;
}
