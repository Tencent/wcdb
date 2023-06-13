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

#include "StatementDropViewBridge.h"
#include "Schema.hpp"
#include "StatementDropView.hpp"
#include "WinqBridge.hpp"

CPPStatementDropView WCDBStatementDropViewCreate(void)
{
    return WCDBCreateCPPBridgedObject(CPPStatementDropView, WCDB::StatementDropView);
}

void WCDBStatementDropViewConfigSchema(CPPStatementDropView dropViewStatement, CPPSchema schema)
{
    WCDBGetObjectOrReturn(dropViewStatement, WCDB::StatementDropView, cppDropViewStatement);
    WCDBGetObjectOrReturn(schema, WCDB::Schema, cppSchema);
    cppDropViewStatement->schema(*cppSchema);
}

void WCDBStatementDropViewConfigSchema2(CPPStatementDropView dropViewStatement,
                                        CPPCommonValue schema)
{
    WCDBGetObjectOrReturn(dropViewStatement, WCDB::StatementDropView, cppDropViewStatement);
    cppDropViewStatement->schema(WCDBCreateSchemaFromCommonValue(schema));
}

void WCDBStatementDropViewConfigView(CPPStatementDropView dropViewStatement,
                                     const char* _Nullable name)
{
    WCDBGetObjectOrReturn(dropViewStatement, WCDB::StatementDropView, cppDropViewStatement);
    cppDropViewStatement->dropView(name);
}

void WCDBStatementDropViewConfigIfExists(CPPStatementDropView dropViewStatement)
{
    WCDBGetObjectOrReturn(dropViewStatement, WCDB::StatementDropView, cppDropViewStatement);
    cppDropViewStatement->ifExists();
}
