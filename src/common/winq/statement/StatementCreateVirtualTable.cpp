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

StatementCreateVirtualTable::StatementCreateVirtualTable() = default;

StatementCreateVirtualTable::~StatementCreateVirtualTable() = default;

StatementCreateVirtualTable&
StatementCreateVirtualTable::createVirtualTable(const UnsafeStringView& table)
{
    syntax().table = table;
    return *this;
}

StatementCreateVirtualTable& StatementCreateVirtualTable::schema(const Schema& schema)
{
    syntax().schema = schema;
    return *this;
}

StatementCreateVirtualTable& StatementCreateVirtualTable::ifNotExists()
{
    syntax().ifNotExists = true;
    return *this;
}

StatementCreateVirtualTable&
StatementCreateVirtualTable::usingModule(const UnsafeStringView& module)
{
    syntax().module = module;
    return *this;
}

StatementCreateVirtualTable&
StatementCreateVirtualTable::argument(const UnsafeStringView& argument)
{
    syntax().arguments.push_back(StringView(argument));
    return *this;
}

StatementCreateVirtualTable& StatementCreateVirtualTable::argument(const SQL& argument)
{
    syntax().arguments.push_back(argument.getDescription());
    return *this;
}

StringView StatementCreateVirtualTable::externalContent(const UnsafeStringView& tableName)
{
    return StringView::formatted("content='%s'", tableName.data());
}

} // namespace WCDB
