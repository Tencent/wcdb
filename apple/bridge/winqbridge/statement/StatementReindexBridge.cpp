//
// Created by 陈秋文 on 2022/7/17.
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

#include <WCDB/Schema.hpp>
#include <WCDB/StatementReindex.hpp>
#include <WCDB/StatementReindexBridge.h>
#include <WCDB/WinqBridge.hpp>

CPPStatementReIndex WCDBStatementReIndexCreate(void)
{
    return WCDBCreateCPPBridgedObject(CPPStatementReIndex, new WCDB::StatementReindex());
}

void WCDBStatementReIndexConfigColllation(CPPStatementReIndex statment,
                                          const char* _Nullable collation)
{
    WCDBGetObjectOrReturn(statment, WCDB::StatementReindex, cppStatement);
    cppStatement->collation(collation);
}

void WCDBStatementReIndexConfigTable(CPPStatementReIndex statment, const char* _Nullable name)
{
    WCDBGetObjectOrReturn(statment, WCDB::StatementReindex, cppStatement);
    cppStatement->table(name);
}

void WCDBStatementReIndexConfigIndex(CPPStatementReIndex statment, const char* _Nullable name)
{
    WCDBGetObjectOrReturn(statment, WCDB::StatementReindex, cppStatement);
    cppStatement->index(name);
}

void WCDBStatementReIndexConfigSchema(CPPStatementReIndex statment, CPPSchema schema)
{
    WCDBGetObjectOrReturn(statment, WCDB::StatementReindex, cppStatement);
    WCDBGetObjectOrReturn(schema, WCDB::Schema, cppSchema);
    cppStatement->schema(*cppSchema);
}
