// Created by qiuwenchen on 2023/4/12.
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

#define WCDBJNIStatementDropTableFuncName(funcName)                            \
    WCDBJNI(StatementDropTable, funcName)
#define WCDBJNIStatementDropTableObjectMethod(funcName, ...)                   \
    WCDBJNIObjectMethod(StatementDropTable, funcName, __VA_ARGS__)
#define WCDBJNIStatementDropTableObjectMethodWithNoArg(funcName)               \
    WCDBJNIObjectMethodWithNoArg(StatementDropTable, funcName)
#define WCDBJNIStatementDropTableClassMethodWithNoArg(funcName)                \
    WCDBJNIClassMethodWithNoArg(StatementDropTable, funcName)

jlong WCDBJNIStatementDropTableObjectMethodWithNoArg(create);
void WCDBJNIStatementDropTableObjectMethod(configSchema,
                                           jlong self,
                                           WCDBJNIObjectOrStringParameter(schema));
void WCDBJNIStatementDropTableObjectMethod(configTableName, jlong self, jstring tableName);
void WCDBJNIStatementDropTableObjectMethod(configIfExist, jlong self);