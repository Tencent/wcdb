//
// Created by qiuwenchen on 2022/6/27.
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

#include "StatementAlterTableBridge.h"
#include "Column.hpp"
#include "ColumnDef.hpp"
#include "ObjectBridge.hpp"
#include "Schema.hpp"
#include "StatementAlterTable.hpp"
#include "WinqBridge.hpp"

CPPStatementAlterTable WCDBStatementAlterTableCreate()
{
    return WCDBCreateCPPBridgedObject(CPPStatementAlterTable, WCDB::StatementAlterTable);
}

void WCDBStatementAlterTableConfigTable(CPPStatementAlterTable alterTable,
                                        const char* _Nullable name)
{
    WCDBGetObjectOrReturn(alterTable, WCDB::StatementAlterTable, cppAlterTable);
    cppAlterTable->alterTable(name);
}

void WCDBStatementAlterTableConfigSchema(CPPStatementAlterTable alterTable, CPPSchema schema)
{
    WCDBGetObjectOrReturn(alterTable, WCDB::StatementAlterTable, cppAlterTable);
    WCDBGetObjectOrReturn(schema, WCDB::Schema, cppSchema);
    cppAlterTable->schema(*cppSchema);
}

void WCDBStatementAlterTableConfigSchema2(CPPStatementAlterTable alterTable, CPPCommonValue schema)
{
    WCDBGetObjectOrReturn(alterTable, WCDB::StatementAlterTable, cppAlterTable);
    cppAlterTable->schema(WCDBCreateSchemaFromCommonValue(schema));
}

void WCDBStatementAlterTableConfigRenameToTable(CPPStatementAlterTable alterTable,
                                                const char* _Nullable name)
{
    WCDBGetObjectOrReturn(alterTable, WCDB::StatementAlterTable, cppAlterTable);
    cppAlterTable->renameToTable(name);
}

void WCDBStatementAlterTableConfigRenameColumn(CPPStatementAlterTable alterTable, CPPColumn column)
{
    WCDBGetObjectOrReturn(alterTable, WCDB::StatementAlterTable, cppAlterTable);
    WCDBGetObjectOrReturn(column, WCDB::Column, cppColumn);
    cppAlterTable->renameColumn(*cppColumn);
}

void WCDBStatementAlterTableConfigRenameColumn2(CPPStatementAlterTable alterTable,
                                                CPPCommonValue column)
{
    WCDBGetObjectOrReturn(alterTable, WCDB::StatementAlterTable, cppAlterTable);
    cppAlterTable->renameColumn(WCDBCreateColumnFromCommonValue(column));
}

void WCDBStatementAlterTableConfigRenameToColumn(CPPStatementAlterTable alterTable, CPPColumn column)
{
    WCDBGetObjectOrReturn(alterTable, WCDB::StatementAlterTable, cppAlterTable);
    WCDBGetObjectOrReturn(column, WCDB::Column, cppColumn);
    cppAlterTable->toColumn(*cppColumn);
}

void WCDBStatementAlterTableConfigRenameToColumn2(CPPStatementAlterTable alterTable,
                                                  CPPCommonValue column)
{
    WCDBGetObjectOrReturn(alterTable, WCDB::StatementAlterTable, cppAlterTable);
    cppAlterTable->toColumn(WCDBCreateColumnFromCommonValue(column));
}

void WCDBStatementAlterTableConfigAddColumn(CPPStatementAlterTable alterTable, CPPColumnDef columndef)
{
    WCDBGetObjectOrReturn(alterTable, WCDB::StatementAlterTable, cppAlterTable);
    WCDBGetObjectOrReturn(columndef, WCDB::ColumnDef, cppColumnDef);
    cppAlterTable->addColumn(*cppColumnDef);
}
