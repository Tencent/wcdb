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

jlong WCDBJNIStatementCreateTriggerObjectMethodWithNoArg(createCppObj);

void WCDBJNIStatementCreateTriggerObjectMethod(configTrigger, jlong self, jstring name);
void WCDBJNIStatementCreateTriggerObjectMethod(configSchema,
                                               jlong self,
                                               WCDBJNIObjectOrStringParameter(schema));
void WCDBJNIStatementCreateTriggerObjectMethod(configTemp, jlong self);
void WCDBJNIStatementCreateTriggerObjectMethod(configIfNotExist, jlong self);

void WCDBJNIStatementCreateTriggerObjectMethod(configBefore, jlong self);
void WCDBJNIStatementCreateTriggerObjectMethod(configAfter, jlong self);
void WCDBJNIStatementCreateTriggerObjectMethod(configInsteadOf, jlong self);

void WCDBJNIStatementCreateTriggerObjectMethod(configDelete, jlong self);
void WCDBJNIStatementCreateTriggerObjectMethod(configInsert, jlong self);
void WCDBJNIStatementCreateTriggerObjectMethod(configUpdate, jlong self);

void WCDBJNIStatementCreateTriggerObjectMethod(configColumns,
                                               jlong self,
                                               WCDBJNIObjectOrStringArrayParameter(columns));
void WCDBJNIStatementCreateTriggerObjectMethod(configTable, jlong self, jstring table);
void WCDBJNIStatementCreateTriggerObjectMethod(configForEachRow, jlong self);
void WCDBJNIStatementCreateTriggerObjectMethod(configWhen, jlong self, jlong expression);

void WCDBJNIStatementCreateTriggerObjectMethod(executeInsert, jlong self, jlong insert);
void WCDBJNIStatementCreateTriggerObjectMethod(executeUpdate, jlong self, jlong update);
void WCDBJNIStatementCreateTriggerObjectMethod(executeDelete, jlong self, jlong delete_);
void WCDBJNIStatementCreateTriggerObjectMethod(executeSelect, jlong self, jlong select);
