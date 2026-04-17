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

#include "StatementAttachRust.h"

#include "StatementAttachBridge.h"

void* WCDBRustStatementAttachClassMethodWithNoArg(createCppObj) {
    return (void*)WCDBStatementAttachCreate().innerValue;
}

void WCDBRustStatementAttachClassMethod(configPath,
                                        void* self,
                                        WCDBRustObjectOrStringParameter(path)) {
    WCDBRustBridgeStruct(CPPStatementAttach, self);
    WCDBRustCreateObjectOrStringCommonValue(path, true);
    WCDBStatementAttachConfigPath2(selfStruct, path_common);
    // WCDBRustTryReleaseStringInCommonValue(path);
}

void WCDBRustStatementAttachClassMethod(configSchema,
                                        void* self,
                                        WCDBRustObjectOrStringParameter(schema)) {
    WCDBRustBridgeStruct(CPPStatementAttach, self);
    WCDBRustCreateObjectOrStringCommonValue(schema, true);
    WCDBStatementAttachConfigSchema2(selfStruct, schema_common);
    // WCDBRustTryReleaseStringInCommonValue(schema);
}

void WCDBRustStatementAttachClassMethod(configKey,
                                        void* self,
                                        WCDBRustObjectOrStringParameter(key)) {
    WCDBRustBridgeStruct(CPPStatementAttach, self);
    WCDBRustCreateObjectOrStringCommonValue(key, true);
    WCDBStatementAttachConfigKey2(selfStruct, key_common);
    // WCDBRustTryReleaseStringInCommonValue(key);
}
