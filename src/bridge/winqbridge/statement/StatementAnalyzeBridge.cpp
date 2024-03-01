//
// Created by qiuwenchen on 2022/6/29.
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

#include "StatementAnalyzeBridge.h"
#include "ObjectBridge.hpp"
#include "Schema.hpp"
#include "StatementAnalyze.hpp"
#include "WinqBridge.hpp"

CPPStatementAnalyze WCDBStatementAnalyzeCreate(void)
{
    return WCDBCreateCPPBridgedObject(CPPStatementAnalyze, WCDB::StatementAnalyze);
}

void WCDBStatementAnalyzeToAnalyze(CPPStatementAnalyze analyze)
{
    WCDBGetObjectOrReturn(analyze, WCDB::StatementAnalyze, cppAnalyze);
    cppAnalyze->analyze();
}

void WCDBStatementAnalyzeConfigSchema(CPPStatementAnalyze analyze, CPPSchema schema)
{
    WCDBGetObjectOrReturn(analyze, WCDB::StatementAnalyze, cppAnalyze);
    WCDBGetObjectOrReturn(schema, WCDB::Schema, cppSchema);
    cppAnalyze->schema(*cppSchema);
}

void WCDBStatementAnalyzeConfigSchema2(CPPStatementAnalyze analyze, CPPCommonValue schema)
{
    WCDBGetObjectOrReturn(analyze, WCDB::StatementAnalyze, cppAnalyze);
    cppAnalyze->schema(WCDBCreateSchemaFromCommonValue(schema));
}

void WCDBStatementAnalyzeConfigTable(CPPStatementAnalyze analyze, const char* _Nullable table)
{
    WCDBGetObjectOrReturn(analyze, WCDB::StatementAnalyze, cppAnalyze);
    cppAnalyze->table(table);
}

void WCDBStatementAnalyzeConfigIndex(CPPStatementAnalyze analyze, const char* _Nullable index)
{
    WCDBGetObjectOrReturn(analyze, WCDB::StatementAnalyze, cppAnalyze);
    cppAnalyze->index(index);
}
