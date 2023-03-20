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

#include "WINQ.h"

namespace WCDB {

StatementCreateTable::StatementCreateTable() = default;

StatementCreateTable::~StatementCreateTable() = default;

StatementCreateTable& StatementCreateTable::createTable(const UnsafeStringView& table)
{
    syntax().table = table;
    return *this;
}

StatementCreateTable& StatementCreateTable::schema(const Schema& schema)
{
    syntax().schema = schema;
    return *this;
}

StatementCreateTable& StatementCreateTable::temp()
{
    syntax().temp = true;
    return *this;
}

StatementCreateTable& StatementCreateTable::ifNotExists()
{
    syntax().ifNotExists = true;
    return *this;
}

StatementCreateTable& StatementCreateTable::as(const StatementSelect& select)
{
    syntax().switcher = SyntaxType::Switch::Select;
    syntax().select = select;
    return *this;
}

StatementCreateTable& StatementCreateTable::define(const ColumnDef& columnDef)
{
    syntax().switcher = SyntaxType::Switch::ColumnDefs;
    syntax().columnDefs.push_back(columnDef);
    return *this;
}

StatementCreateTable& StatementCreateTable::constraint(const TableConstraint& constraint)
{
    syntax().tableConstraints.push_back(constraint);
    return *this;
}

StatementCreateTable& StatementCreateTable::withoutRowID()
{
    syntax().withoutRowid = true;
    return *this;
}

} // namespace WCDB
