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

#pragma once

#include "Statement.hpp"

namespace WCDB {

class WCDB_API StatementCreateVirtualTable
: public SpecifiedSyntax<Syntax::CreateVirtualTableSTMT, Statement> {
public:
    using SpecifiedSyntax<Syntax::CreateVirtualTableSTMT, Statement>::SpecifiedSyntax;
    StatementCreateVirtualTable();
    ~StatementCreateVirtualTable() override;

    StatementCreateVirtualTable& createVirtualTable(const UnsafeStringView& table);
    StatementCreateVirtualTable& schema(const Schema& schema);

    StatementCreateVirtualTable& ifNotExists();

    StatementCreateVirtualTable& usingModule(const UnsafeStringView& module);

    StatementCreateVirtualTable& argument(const UnsafeStringView& argument);
    StatementCreateVirtualTable& argument(const SQL& argument);
    static StringView externalContent(const UnsafeStringView& tableName);
};

} // namespace WCDB
