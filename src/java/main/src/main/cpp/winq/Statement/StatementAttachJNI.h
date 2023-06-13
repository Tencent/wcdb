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

#include "WCDBJNI.h"

#pragma once
#define WCDBJNIStatementAttachFuncName(funcName)                               \
    WCDBJNI(StatementAttach, funcName)
#define WCDBJNIStatementAttachObjectMethod(funcName, ...)                      \
    WCDBJNIObjectMethod(StatementAttach, funcName, __VA_ARGS__)
#define WCDBJNIStatementAttachClassMethodWithNoArg(funcName)                   \
    WCDBJNIClassMethodWithNoArg(StatementAttach, funcName)
#define WCDBJNIStatementAttachClassMethod(funcName, ...)                       \
    WCDBJNIClassMethod(StatementAttach, funcName, __VA_ARGS__)

jlong WCDBJNIStatementAttachClassMethodWithNoArg(createCppObj);
void WCDBJNIStatementAttachObjectMethod(configPath,
                                        jlong self,
                                        WCDBJNIObjectOrStringParameter(path));
void WCDBJNIStatementAttachObjectMethod(configSchema,
                                        jlong self,
                                        WCDBJNIObjectOrStringParameter(schema));
void WCDBJNIStatementAttachObjectMethod(configKey,
                                        jlong self,
                                        WCDBJNIObjectOrStringParameter(key));