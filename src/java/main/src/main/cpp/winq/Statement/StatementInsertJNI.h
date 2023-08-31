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
#define WCDBJNIStatementInsertClassMethod(funcName, ...)                       \
    WCDBJNIClassMethod(StatementInsert, funcName, __VA_ARGS__)

jlong WCDBJNIStatementInsertClassMethodWithNoArg(create);
void WCDBJNIStatementInsertClassMethod(configWith, jlong self, jlongArray expressions);
void WCDBJNIStatementInsertClassMethod(configRecursive, jlong self);
void WCDBJNIStatementInsertClassMethod(configTableName, jlong self, jstring tableName);
void WCDBJNIStatementInsertClassMethod(configSchema,
                                       jlong self,
                                       WCDBJNIObjectOrStringParameter(schema));
void WCDBJNIStatementInsertClassMethod(configConfliction, jlong self, jint action);
void WCDBJNIStatementInsertClassMethod(configAs, jlong self, jstring alias);
void WCDBJNIStatementInsertClassMethod(configColumns,
                                       jlong self,
                                       WCDBJNIObjectOrStringArrayParameter(columns));
void WCDBJNIStatementInsertClassMethod(configValues,
                                       jlong self,
                                       WCDBJNIMultiTypeArrayParameter(value));
void WCDBJNIStatementInsertClassMethod(configValuesWithBindParameters, jlong self, jint count);
void WCDBJNIStatementInsertClassMethod(configSelect, jlong self, jlong select);
void WCDBJNIStatementInsertClassMethod(configDefaultValues, jlong self);
void WCDBJNIStatementInsertClassMethod(configUpsert, jlong self, jlong upsert);