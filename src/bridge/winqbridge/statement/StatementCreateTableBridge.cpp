//
// Created by qiuwenchen on 2022/6/30.
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

#include "StatementCreateTableBridge.h"
#include "ColumnDef.hpp"
#include "Schema.hpp"
#include "StatementCreateTable.hpp"
#include "StatementSelect.hpp"
#include "TableConstraint.hpp"
#include "WinqBridge.hpp"

CPPStatementCreateTable WCDBStatementCreateTableCreate(void)
{
    return WCDBCreateCPPBridgedObject(CPPStatementCreateTable, WCDB::StatementCreateTable);
}

void WCDBStatementCreateTableConfigTable(CPPStatementCreateTable createTable,
                                         const char* _Nullable name)
{
    WCDBGetObjectOrReturn(createTable, WCDB::StatementCreateTable, cppCreateTable);
    cppCreateTable->createTable(name);
}

void WCDBStatementCreateTableConfigSchema(CPPStatementCreateTable createTable, CPPSchema schema)
{
    WCDBGetObjectOrReturn(createTable, WCDB::StatementCreateTable, cppCreateTable);
    WCDBGetObjectOrReturn(schema, WCDB::Schema, cppSchema);
    cppCreateTable->schema(*cppSchema);
}

void WCDBStatementCreateTableConfigSchema2(CPPStatementCreateTable createTable,
                                           CPPCommonValue schema)
{
    WCDBGetObjectOrReturn(createTable, WCDB::StatementCreateTable, cppCreateTable);
    cppCreateTable->schema(WCDBCreateSchemaFromCommonValue(schema));
}

void WCDBStatementCreateTableConfigTemp(CPPStatementCreateTable createTable)
{
    WCDBGetObjectOrReturn(createTable, WCDB::StatementCreateTable, cppCreateTable);
    cppCreateTable->temp();
}

void WCDBStatementCreateTableConfigIfNotExist(CPPStatementCreateTable createTable)
{
    WCDBGetObjectOrReturn(createTable, WCDB::StatementCreateTable, cppCreateTable);
    cppCreateTable->ifNotExists();
}

void WCDBStatementCreateTableConfigAs(CPPStatementCreateTable createTable, CPPStatementSelect select)
{
    WCDBGetObjectOrReturn(createTable, WCDB::StatementCreateTable, cppCreateTable);
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    cppCreateTable->as(*cppSelect);
}

void WCDBStatementCreateTableConfigColumns(CPPStatementCreateTable createTable,
                                           const CPPColumnDef* _Nullable columns,
                                           int columnNum)
{
    WCDBGetObjectOrReturn(createTable, WCDB::StatementCreateTable, cppCreateTable);
    WCDBGetCPPSyntaxListOrReturn(WCDB::ColumnDef, cppColumns, columns, columnNum);
    cppCreateTable->syntax().switcher
    = WCDB::StatementCreateTable::SyntaxType::Switch::ColumnDefs;
    cppCreateTable->syntax().columnDefs = cppColumns;
}

void WCDBStatementCreateTableConfigColumn(CPPStatementCreateTable createTable, CPPColumnDef column)
{
    WCDBGetObjectOrReturn(createTable, WCDB::StatementCreateTable, cppCreateTable);
    WCDBGetObjectOrReturn(column, WCDB::ColumnDef, cppColumn);
    cppCreateTable->define(*cppColumn);
}

void WCDBStatementCreateTableConfigTableConstraints(CPPStatementCreateTable createTable,
                                                    const CPPTableConstraint* _Nullable constraints,
                                                    int constraintNum)
{
    WCDBGetObjectOrReturn(createTable, WCDB::StatementCreateTable, cppCreateTable);
    WCDBGetCPPSyntaxListOrReturn(
    WCDB::TableConstraint, cppConstraints, constraints, constraintNum);
    cppCreateTable->syntax().tableConstraints = cppConstraints;
}

void WCDBStatementCreateTableConfigWithoutRowId(CPPStatementCreateTable createTable)
{
    WCDBGetObjectOrReturn(createTable, WCDB::StatementCreateTable, cppCreateTable);
    cppCreateTable->withoutRowID();
}
