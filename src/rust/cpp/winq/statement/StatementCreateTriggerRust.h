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

#include "WCDBRust.h"

#define WCDBRustStatementCreateTriggerFuncName(funcName) WCDBRust(StatementCreateTrigger, funcName)
#define WCDBRustStatementCreateTriggerObjectMethod(funcName, ...) \
    WCDBRustObjectMethod(StatementCreateTrigger, funcName, __VA_ARGS__)
#define WCDBRustStatementCreateTriggerObjectMethodWithNoArg(funcName) \
    WCDBRustObjectMethodWithNoArg(StatementCreateTrigger, funcName)
#define WCDBRustStatementCreateTriggerClassMethodWithNoArg(funcName) \
    WCDBRustClassMethodWithNoArg(StatementCreateTrigger, funcName)
#define WCDBRustStatementCreateTriggerClassMethod(funcName, ...) \
    WCDBRustClassMethod(StatementCreateTrigger, funcName, __VA_ARGS__)

void* WCDBRustStatementCreateTriggerClassMethodWithNoArg(createCppObj);

void WCDBRustStatementCreateTriggerClassMethod(configTrigger, void* self, const char* name);
void WCDBRustStatementCreateTriggerClassMethod(configSchema,
                                               void* self,
                                               WCDBRustObjectOrStringParameter(schema));
void WCDBRustStatementCreateTriggerClassMethod(configTemp, void* self);
void WCDBRustStatementCreateTriggerClassMethod(configIfNotExist, void* self);

void WCDBRustStatementCreateTriggerClassMethod(configBefore, void* self);
void WCDBRustStatementCreateTriggerClassMethod(configAfter, void* self);
void WCDBRustStatementCreateTriggerClassMethod(configInsteadOf, void* self);

void WCDBRustStatementCreateTriggerClassMethod(configDelete, void* self);
void WCDBRustStatementCreateTriggerClassMethod(configInsert, void* self);
void WCDBRustStatementCreateTriggerClassMethod(configUpdate, void* self);

void WCDBRustStatementCreateTriggerClassMethod(configColumns,
                                               void* self,
                                               WCDBRustObjectOrStringArrayParameter(columns));
void WCDBRustStatementCreateTriggerClassMethod(configTable, void* self, const char* table);
void WCDBRustStatementCreateTriggerClassMethod(configForEachRow, void* self);
void WCDBRustStatementCreateTriggerClassMethod(configWhen, void* self, void* expression);

void WCDBRustStatementCreateTriggerClassMethod(executeInsert, void* self, void* insert);
void WCDBRustStatementCreateTriggerClassMethod(executeUpdate, void* self, void* update);
void WCDBRustStatementCreateTriggerClassMethod(executeDelete, void* self, void* delete_);
void WCDBRustStatementCreateTriggerClassMethod(executeSelect, void* self, void* select);
