// Created by chenqiuwen on 2023/6/11.
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

#include "StatementAnalyzeJNI.h"
#include "StatementAnalyzeBridge.h"

jlong WCDBJNIStatementAnalyzeClassMethodWithNoArg(createCppObj)
{
    return (jlong) WCDBStatementAnalyzeCreate().innerValue;
}

void WCDBJNIStatementAnalyzeObjectMethod(toAnalyze, jlong analyze)
{
    WCDBJNIBridgeStruct(CPPStatementAnalyze, analyze);
    WCDBStatementAnalyzeToAnalyze(analyzeStruct);
}

void WCDBJNIStatementAnalyzeObjectMethod(configSchema,
                                         jlong analyze,
                                         WCDBJNIObjectOrStringParameter(schema))
{
    WCDBJNIBridgeStruct(CPPStatementAnalyze, analyze);
    WCDBJNICreateObjectOrStringCommonValue(schema);
    WCDBStatementAnalyzeConfigSchema2(analyzeStruct, schema_common);
    WCDBJNITryReleaseStringInCommonValue(schema);
}

void WCDBJNIStatementAnalyzeObjectMethod(configTable, jlong analyze, jstring table)
{
    WCDBJNIBridgeStruct(CPPStatementAnalyze, analyze);
    WCDBJNIGetString(table);
    WCDBStatementAnalyzeConfigTable(analyzeStruct, tableString);
    WCDBJNIReleaseString(table);
}

void WCDBJNIStatementAnalyzeObjectMethod(configIndex, jlong analyze, jstring index)
{
    WCDBJNIBridgeStruct(CPPStatementAnalyze, analyze);
    WCDBJNIGetString(index);
    WCDBStatementAnalyzeConfigIndex(analyzeStruct, indexString);
    WCDBJNIReleaseString(index);
}
