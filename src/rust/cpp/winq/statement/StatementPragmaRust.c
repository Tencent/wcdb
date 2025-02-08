// Created by chenqiuwen on 2023/4/12.
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

#include "StatementPragmaRust.h"

#include "StatementPragmaBridge.h"

void* WCDBRustStatementPragmaClassMethodWithNoArg(create) {
    return (void*)WCDBStatementPragmaCreate().innerValue;
}

// void WCDBJNIStatementPragmaClassMethod(configSchema,
//                                        jlong self,
//                                        WCDBJNIObjectOrStringParameter(schema))
// {
//     WCDBJNIBridgeStruct(CPPStatementPragma, self);
//     WCDBJNICreateObjectOrStringCommonValue(schema, true);
//     WCDBStatementPragmaConfigSchema2(selfStruct, schema_common);
//     WCDBJNITryReleaseStringInCommonValue(schema);
// }

void WCDBRustStatementPragmaClassMethod(configPragma, void* self, void* pragma) {
    WCDBRustBridgeStruct(CPPStatementPragma, self);
    WCDBRustBridgeStruct(CPPPragma, pragma);
    WCDBStatementPragmaConfigPragma(selfStruct, pragmaStruct);
}

void WCDBRustStatementPragmaClassMethod(configToValue,
                                        void* self,
                                        WCDBRustCommonValueParameter(value)) {
    WCDBRustBridgeStruct(CPPStatementPragma, self);
    WCDBRustCreateCommonValueWithIsCritical(value, true);
    WCDBStatementPragmaConfigToValue2(selfStruct, value_common);
    // WCDBRustTryReleaseStringInCommonValue(value);
}
//
// void WCDBJNIStatementPragmaClassMethod(configToValue, jlong self,
// WCDBJNICommonValueParameter(value))
// {
//     WCDBJNIBridgeStruct(CPPStatementPragma, self);
//     WCDBJNICreateCommonValue(value, true);
//     WCDBStatementPragmaConfigToValue2(selfStruct, value_common);
//     WCDBJNITryReleaseStringInCommonValue(value);
// }
//
// void WCDBJNIStatementPragmaClassMethod(configWithValue, jlong self,
// WCDBJNICommonValueParameter(value))
// {
//     WCDBJNIBridgeStruct(CPPStatementPragma, self);
//     WCDBJNICreateCommonValue(value, true);
//     WCDBStatementPragmaConfigWithValue2(selfStruct, value_common);
//     WCDBJNITryReleaseStringInCommonValue(value);
// }
