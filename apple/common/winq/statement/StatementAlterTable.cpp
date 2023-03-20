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

StatementAlterTable::StatementAlterTable() = default;

StatementAlterTable::~StatementAlterTable() = default;

StatementAlterTable& StatementAlterTable::alterTable(const UnsafeStringView& table)
{
    syntax().table = table;
    return *this;
}

StatementAlterTable& StatementAlterTable::schema(const Schema& schema)
{
    syntax().schema = schema;
    return *this;
}

StatementAlterTable& StatementAlterTable::renameToTable(const UnsafeStringView& newTable)
{
    syntax().switcher = Syntax::AlterTableSTMT::Switch::RenameTable;
    syntax().newTable = newTable;
    return *this;
}

StatementAlterTable& StatementAlterTable::renameColumn(const Column& oldColumn)
{
    syntax().switcher = Syntax::AlterTableSTMT::Switch::RenameColumn;
    syntax().column = oldColumn;
    return *this;
}

StatementAlterTable& StatementAlterTable::toColumn(const Column& newColumn)
{
    syntax().newColumn = newColumn;
    return *this;
}

StatementAlterTable& StatementAlterTable::addColumn(const ColumnDef& columnDef)
{
    syntax().switcher = Syntax::AlterTableSTMT::Switch::AddColumn;
    syntax().columnDef = columnDef;
    return *this;
}

} // namespace WCDB
