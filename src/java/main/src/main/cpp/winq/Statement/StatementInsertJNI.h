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

#define WCDBJNIStatementInsertFuncName(funcName)                               \
    WCDBJNI(StatementInsert, funcName)
#define WCDBJNIStatementInsertObjectMethod(funcName, ...)                      \
    WCDBJNIObjectMethod(StatementInsert, funcName, __VA_ARGS__)
#define WCDBJNIStatementInsertObjectMethodWithNoArg(funcName)                  \
    WCDBJNIObjectMethodWithNoArg(StatementInsert, funcName)
#define WCDBJNIStatementInsertClassMethodWithNoArg(funcName)                   \
    WCDBJNIClassMethodWithNoArg(StatementInsert, funcName)

jlong WCDBJNIStatementInsertObjectMethodWithNoArg(create);
void WCDBJNIStatementInsertObjectMethod(configWith, jlong self, jlongArray expressions);
void WCDBJNIStatementInsertObjectMethod(configRecursive, jlong self);
void WCDBJNIStatementInsertObjectMethod(configTableName, jlong self, jstring tableName);
void WCDBJNIStatementInsertObjectMethod(configSchema,
                                        jlong self,
                                        WCDBJNIObjectOrStringParameter(schema));
void WCDBJNIStatementInsertObjectMethod(configConfliction, jlong self, jint action);
void WCDBJNIStatementInsertObjectMethod(configAs, jlong self, jstring alias);
void WCDBJNIStatementInsertObjectMethod(configColumns,
                                        jlong self,
                                        WCDBJNIObjectOrStringArrayParameter(columns));
void WCDBJNIStatementInsertObjectMethod(configValues,
                                        jlong self,
                                        WCDBJNIMultiTypeArrayParameter(value));
void WCDBJNIStatementInsertObjectMethod(configValuesWithBindParameters, jlong self, jint count);
void WCDBJNIStatementInsertObjectMethod(configSelect, jlong self, jlong select);
void WCDBJNIStatementInsertObjectMethod(configDefaultValues, jlong self);
void WCDBJNIStatementInsertObjectMethod(configUpsert, jlong self, jlong upsert);