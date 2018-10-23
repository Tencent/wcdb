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

StatementCreateTrigger& StatementCreateTrigger::createTrigger(const SyntaxString& trigger)
{
    syntax.temp = false;
    syntax.schema = Schema();
    syntax.trigger = trigger;
    return *this;
}

StatementCreateTrigger&
StatementCreateTrigger::createTrigger(const Schema& schema, const SyntaxString& trigger)
{
    syntax.temp = false;
    syntax.schema = schema;
    syntax.trigger = trigger;
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::createTempTrigger(const SyntaxString& trigger)
{
    syntax.temp = true;
    syntax.schema = Schema();
    syntax.trigger = trigger;
    return *this;
}

StatementCreateTrigger&
StatementCreateTrigger::createTempTrigger(const Schema& schema, const SyntaxString& trigger)
{
    syntax.temp = true;
    syntax.schema = schema;
    syntax.trigger = trigger;
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::ifNotExists()
{
    syntax.ifNotExists = true;
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::before()
{
    syntax.specifiedTiming = true;
    syntax.timing = SyntaxType::Timing::Before;
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::after()
{
    syntax.specifiedTiming = true;
    syntax.timing = SyntaxType::Timing::After;
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::insteadOf()
{
    syntax.specifiedTiming = true;
    syntax.timing = SyntaxType::Timing::InsteadOf;
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::delete_()
{
    syntax.event = SyntaxType::Event::Delete;
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::insert()
{
    syntax.event = SyntaxType::Event::Insert;
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::update()
{
    syntax.event = SyntaxType::Event::Update;
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::updateOf(const Columns& columns)
{
    syntax.event = SyntaxType::Event::Update;
    syntax.columns = columns;
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::on(const SyntaxString& table)
{
    syntax.table = table;
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::forEachRow()
{
    syntax.forEachFow = true;
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::when(const Expression& condition)
{
    syntax.useCondition = true;
    syntax.condition = condition;
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::execute(const StatementInsert& statement)
{
    syntax.stmts.push_back(SyntaxType::STMT::Insert);
    syntax.inserts.push_back(statement);
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::execute(const StatementUpdate& statement)
{
    syntax.stmts.push_back(SyntaxType::STMT::Update);
    syntax.updates.push_back(statement);
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::execute(const StatementDelete& statement)
{
    syntax.stmts.push_back(SyntaxType::STMT::Delete);
    syntax.deletes.push_back(statement);
    return *this;
}

StatementCreateTrigger& StatementCreateTrigger::execute(const StatementSelect& statement)
{
    syntax.stmts.push_back(SyntaxType::STMT::Select);
    syntax.selects.push_back(statement);
    return *this;
}

} // namespace WCDB
