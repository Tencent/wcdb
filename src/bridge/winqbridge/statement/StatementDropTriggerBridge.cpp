//
// Created by qiuwenchen on 2022/7/13.
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

#include "StatementDropTriggerBridge.h"
#include "Schema.hpp"
#include "StatementDropTrigger.hpp"
#include "WinqBridge.hpp"

CPPStatementDropTrigger WCDBStatementDropTriggerCreate(void)
{
    return WCDBCreateCPPBridgedObject(CPPStatementDropTrigger, WCDB::StatementDropTrigger);
}

void WCDBStatementDropTriggerConfigSchema(CPPStatementDropTrigger dropTriggerStatement,
                                          CPPSchema schema)
{
    WCDBGetObjectOrReturn(
    dropTriggerStatement, WCDB::StatementDropTrigger, cppDropTriggerStatement);
    WCDBGetObjectOrReturn(schema, WCDB::Schema, cppSchema);
    cppDropTriggerStatement->schema(*cppSchema);
}

void WCDBStatementDropTriggerConfigSchema2(CPPStatementDropTrigger dropTriggerStatement,
                                           CPPCommonValue schema)
{
    WCDBGetObjectOrReturn(
    dropTriggerStatement, WCDB::StatementDropTrigger, cppDropTriggerStatement);
    cppDropTriggerStatement->schema(WCDBCreateSchemaFromCommonValue(schema));
}

void WCDBStatementDropTriggerConfigTrigger(CPPStatementDropTrigger dropTriggerStatement,
                                           const char* _Nullable name)
{
    WCDBGetObjectOrReturn(
    dropTriggerStatement, WCDB::StatementDropTrigger, cppDropTriggerStatement);
    cppDropTriggerStatement->dropTrigger(name);
}

void WCDBStatementDropTriggerConfigIfExists(CPPStatementDropTrigger dropTriggerStatement)
{
    WCDBGetObjectOrReturn(
    dropTriggerStatement, WCDB::StatementDropTrigger, cppDropTriggerStatement);
    cppDropTriggerStatement->ifExists();
}
