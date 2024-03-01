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

#pragma once

#include "WCDBJNI.h"

#define WCDBJNIStatementCreateTriggerFuncName(funcName)                        \
    WCDBJNI(StatementCreateTrigger, funcName)
#define WCDBJNIStatementCreateTriggerObjectMethod(funcName, ...)               \
    WCDBJNIObjectMethod(StatementCreateTrigger, funcName, __VA_ARGS__)
#define WCDBJNIStatementCreateTriggerObjectMethodWithNoArg(funcName)           \
    WCDBJNIObjectMethodWithNoArg(StatementCreateTrigger, funcName)
#define WCDBJNIStatementCreateTriggerClassMethodWithNoArg(funcName)            \
    WCDBJNIClassMethodWithNoArg(StatementCreateTrigger, funcName)
#define WCDBJNIStatementCreateTriggerClassMethod(funcName, ...)                \
    WCDBJNIClassMethod(StatementCreateTrigger, funcName, __VA_ARGS__)

jlong WCDBJNIStatementCreateTriggerClassMethodWithNoArg(createCppObj);

void WCDBJNIStatementCreateTriggerClassMethod(configTrigger, jlong self, jstring name);
void WCDBJNIStatementCreateTriggerClassMethod(configSchema,
                                              jlong self,
                                              WCDBJNIObjectOrStringParameter(schema));
void WCDBJNIStatementCreateTriggerClassMethod(configTemp, jlong self);
void WCDBJNIStatementCreateTriggerClassMethod(configIfNotExist, jlong self);

void WCDBJNIStatementCreateTriggerClassMethod(configBefore, jlong self);
void WCDBJNIStatementCreateTriggerClassMethod(configAfter, jlong self);
void WCDBJNIStatementCreateTriggerClassMethod(configInsteadOf, jlong self);

void WCDBJNIStatementCreateTriggerClassMethod(configDelete, jlong self);
void WCDBJNIStatementCreateTriggerClassMethod(configInsert, jlong self);
void WCDBJNIStatementCreateTriggerClassMethod(configUpdate, jlong self);

void WCDBJNIStatementCreateTriggerClassMethod(configColumns,
                                              jlong self,
                                              WCDBJNIObjectOrStringArrayParameter(columns));
void WCDBJNIStatementCreateTriggerClassMethod(configTable, jlong self, jstring table);
void WCDBJNIStatementCreateTriggerClassMethod(configForEachRow, jlong self);
void WCDBJNIStatementCreateTriggerClassMethod(configWhen, jlong self, jlong expression);

void WCDBJNIStatementCreateTriggerClassMethod(executeInsert, jlong self, jlong insert);
void WCDBJNIStatementCreateTriggerClassMethod(executeUpdate, jlong self, jlong update);
void WCDBJNIStatementCreateTriggerClassMethod(executeDelete, jlong self, jlong delete_);
void WCDBJNIStatementCreateTriggerClassMethod(executeSelect, jlong self, jlong select);
