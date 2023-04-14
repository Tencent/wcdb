//
// Created by qiuwenchen on 2022/7/12.
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

#include "StatementDropTableBridge.h"
#include "Schema.hpp"
#include "StatementDropTable.hpp"
#include "WinqBridge.hpp"

CPPStatementDropTable WCDBStatementDropTableCreate(void)
{
    return WCDBCreateCPPBridgedObject(CPPStatementDropTable, WCDB::StatementDropTable);
}

void WCDBStatementDropTableConfigSchema(CPPStatementDropTable dropTableStatement, CPPSchema schema)
{
    WCDBGetObjectOrReturn(dropTableStatement, WCDB::StatementDropTable, cppDropTableStatement);
    WCDBGetObjectOrReturn(schema, WCDB::Schema, cppSchema);
    cppDropTableStatement->schema(*cppSchema);
}

void WCDBStatementDropTableConfigSchema2(CPPStatementDropTable dropTableStatement,
                                         CPPCommonValue schema)
{
    WCDBGetObjectOrReturn(dropTableStatement, WCDB::StatementDropTable, cppDropTableStatement);
    cppDropTableStatement->schema(WCDBCreateSchemaFromCommonValue(schema));
}

void WCDBStatementDropTableConfigTable(CPPStatementDropTable dropTableStatement,
                                       const char* _Nullable name)
{
    WCDBGetObjectOrReturn(dropTableStatement, WCDB::StatementDropTable, cppDropTableStatement);
    cppDropTableStatement->dropTable(name);
}

void WCDBStatementDropTableConfigIfExists(CPPStatementDropTable dropTableStatement)
{
    WCDBGetObjectOrReturn(dropTableStatement, WCDB::StatementDropTable, cppDropTableStatement);
    cppDropTableStatement->ifExists();
}
