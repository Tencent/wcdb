// Created by qiuwenchen on 2023/4/11.
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

#define WCDBJNIStatementCreateTableFuncName(funcName)                          \
    WCDBJNI(StatementCreateTable, funcName)
#define WCDBJNIStatementCreateTableObjectMethod(funcName, ...)                 \
    WCDBJNIObjectMethod(StatementCreateTable, funcName, __VA_ARGS__)
#define WCDBJNIStatementCreateTableObjectMethodWithNoArg(funcName)             \
    WCDBJNIObjectMethodWithNoArg(StatementCreateTable, funcName)
#define WCDBJNIStatementCreateTableClassMethodWithNoArg(funcName)              \
    WCDBJNIClassMethodWithNoArg(StatementCreateTable, funcName)
#define WCDBJNIStatementCreateTableClassMethod(funcName, ...)                  \
    WCDBJNIClassMethod(StatementCreateTable, funcName, __VA_ARGS__)

jlong WCDBJNIStatementCreateTableClassMethodWithNoArg(create);
void WCDBJNIStatementCreateTableClassMethod(configTableName, jlong self, jstring tableName);
void WCDBJNIStatementCreateTableClassMethod(configSchema,
                                            jlong self,
                                            WCDBJNIObjectOrStringParameter(schema));
void WCDBJNIStatementCreateTableClassMethod(configTemp, jlong self);
void WCDBJNIStatementCreateTableClassMethod(configIfNotExist, jlong self);
void WCDBJNIStatementCreateTableClassMethod(configAs, jlong self, jlong select);
void WCDBJNIStatementCreateTableClassMethod(configColumn, jlong self, jlong column);
void WCDBJNIStatementCreateTableClassMethod(configColumns, jlong self, jlongArray columns);
void WCDBJNIStatementCreateTableClassMethod(configConstraints, jlong self, jlongArray constraints);
void WCDBJNIStatementCreateTableClassMethod(configWithoutRowid, jlong self);