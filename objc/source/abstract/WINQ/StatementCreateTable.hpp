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

#ifndef StatementCreateTable_hpp
#define StatementCreateTable_hpp

#include <WCDB/Describable.hpp>
#include <WCDB/Statement.hpp>

namespace WCDB {

class StatementCreateTable : public StatementWithLang<Lang::CreateTableSTMT> {

public:
    StatementCreateTable();
    StatementCreateTable &createTable(const std::string &tableName);
    StatementCreateTable &temp(bool temp = true);
    StatementCreateTable &ifNotExists(bool ifNotExists = true);
    StatementCreateTable &withSchema(const std::string &schemaName);
    StatementCreateTable &as(const StatementSelect &selectSTMT);
    StatementCreateTable &define(const ColumnDef &columnDef);
    StatementCreateTable &define(const std::list<ColumnDef> &columnDefs);
    StatementCreateTable &
    addTableConstraint(const TableConstraint &tableConstraint);
    StatementCreateTable &
    addTableConstraints(const std::list<TableConstraint> &tableConstraints);
    StatementCreateTable &withoutRowID();
};

} // namespace WCDB

#endif /* StatementCreateTable_hpp */
