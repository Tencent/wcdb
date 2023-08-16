//
// Created by sanhuazhang on 2019/05/02
//

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

#include "Statement.hpp"

namespace WCDB {

Statement::~Statement() = default;

Statement::Statement(const Statement& sql) : SQL(sql), m_syntax(sql.m_syntax)
{
}

Statement::Statement(Statement&& sql)
: SQL(std::move(sql)), m_syntax(std::move(sql.m_syntax))
{
}

Statement::Statement(const Shadow<Syntax::Identifier>& syntax)
: SQL(), m_syntax(syntax)
{
}

Statement::Statement(Shadow<Syntax::Identifier>&& syntax)
: SQL(), m_syntax(std::move(syntax))
{
}

Statement::Statement(std::shared_ptr<Syntax::Identifier>&& underlying)
: SQL(), m_syntax(std::move(underlying))
{
}

Statement& Statement::operator=(const Statement& other)
{
    m_syntax = other.m_syntax;
    SQL::operator=(other);
    return *this;
}

Statement& Statement::operator=(Statement&& other)
{
    m_syntax = std::move(other.m_syntax);
    SQL::operator=(std::move(other));
    return *this;
}

Syntax::Identifier& Statement::syntax()
{
    m_syntaxPtr = m_syntax.get();
    return SQL::syntax();
}

const Syntax::Identifier& Statement::syntax() const
{
    m_syntaxPtr = const_cast<Syntax::Identifier*>(m_syntax.get());
    return SQL::syntax();
}

bool Statement::isWriteStatement() const
{
    Syntax::Identifier::Type type = syntax().getType();
    switch (type) {
    case Syntax::Identifier::Type::AlterTableSTMT:
    case Syntax::Identifier::Type::CreateIndexSTMT:
    case Syntax::Identifier::Type::CreateTableSTMT:
    case Syntax::Identifier::Type::CreateTriggerSTMT:
    case Syntax::Identifier::Type::InsertSTMT:
    case Syntax::Identifier::Type::DeleteSTMT:
    case Syntax::Identifier::Type::UpdateSTMT:
    case Syntax::Identifier::Type::CreateViewSTMT:
    case Syntax::Identifier::Type::CreateVirtualTableSTMT:
    case Syntax::Identifier::Type::DropIndexSTMT:
    case Syntax::Identifier::Type::DropTableSTMT:
    case Syntax::Identifier::Type::DropViewSTMT:
    case Syntax::Identifier::Type::ReindexSTMT:
    case Syntax::Identifier::Type::VacuumSTMT:
        return true;
    default:
        return false;
    }
    return true;
}

} //namespace WCDB
