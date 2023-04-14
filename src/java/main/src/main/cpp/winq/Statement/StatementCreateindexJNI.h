// Created by chenqiuwen on 2023/4/9.
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

#pragma once

#include "WCDBJNI.h"

#define WCDBJNIStatementCreateIndexFuncName(funcName)                          \
    WCDBJNI(StatementCreateIndex, funcName)
#define WCDBJNIStatementCreateIndexObjectMethod(funcName, ...)                 \
    WCDBJNIObjectMethod(StatementCreateIndex, funcName, __VA_ARGS__)
#define WCDBJNIStatementCreateIndexObjectMethodWithNoArg(funcName)             \
    WCDBJNIObjectMethodWithNoArg(StatementCreateIndex, funcName)
#define WCDBJNIStatementCreateIndexClassMethodWithNoArg(funcName)              \
    WCDBJNIClassMethodWithNoArg(StatementCreateIndex, funcName)

jlong WCDBJNIStatementCreateIndexObjectMethodWithNoArg(create);
void WCDBJNIStatementCreateIndexObjectMethod(configIndex, jlong self, jstring name);
void WCDBJNIStatementCreateIndexObjectMethod(configSchema,
                                             jlong self,
                                             WCDBJNIObjectOrStringParameter(schema));
void WCDBJNIStatementCreateIndexObjectMethod(configUnique, jlong self);
void WCDBJNIStatementCreateIndexObjectMethod(configIfNotExist, jlong self);
void WCDBJNIStatementCreateIndexObjectMethod(configTable, jlong self, jstring tableName);
void WCDBJNIStatementCreateIndexObjectMethod(configIndexedColumns,
                                             jlong self,
                                             WCDBJNIObjectOrStringArrayParameter(indexColumns));
void WCDBJNIStatementCreateIndexObjectMethod(configWhere, jlong self, jlong condition);
