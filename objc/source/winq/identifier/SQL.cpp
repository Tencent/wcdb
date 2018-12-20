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
#include <WCDB/SQL.hpp>

namespace WCDB {

SQL::SQL()
{
}

SQL::SQL(Type type, const SQL& sql)
{
    WCTRemedialAssert(
    type == sql.getType(), String::formatted("Invalid WINQ assignment."), return;);
    m_syntax = sql.m_syntax;
}

SQL::Type SQL::getType() const
{
    return m_syntax->getType();
}

void SQL::iterate(const Iterator& iterator)
{
    return m_syntax->iterate(iterator);
}

String SQL::getDescription() const
{
    // TODO: check SQLite keyword here.
    return m_syntax->getDescription();
}

Syntax::Identifier* SQL::getSyntaxIdentifier() const
{
    return m_syntax.get();
}

SQL::~SQL()
{
}

} // namespace WCDB
