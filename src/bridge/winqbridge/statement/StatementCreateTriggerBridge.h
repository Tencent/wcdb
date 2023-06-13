//
// Created by qiuwenchen on 2022/7/13.
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

#include "WinqBridge.h"

WCDB_EXTERN_C_BEGIN

CPPStatementCreateTrigger WCDBStatementCreateTriggerCreate(void);
void WCDBStatementCreateTriggerConfigTrigger(CPPStatementCreateTrigger createTrigger,
                                             const char* _Nullable name);
void WCDBStatementCreateTriggerConfigSchema(CPPStatementCreateTrigger createTrigger,
                                            CPPSchema schema);
void WCDBStatementCreateTriggerConfigSchema2(CPPStatementCreateTrigger createTrigger,
                                             CPPCommonValue schema);
void WCDBStatementCreateTriggerConfigTemp(CPPStatementCreateTrigger createTrigger);
void WCDBStatementCreateTriggerConfigIfNotExist(CPPStatementCreateTrigger createTrigger);

void WCDBStatementCreateTriggerConfigBefore(CPPStatementCreateTrigger createTrigger);
void WCDBStatementCreateTriggerConfigAfter(CPPStatementCreateTrigger createTrigger);
void WCDBStatementCreateTriggerConfigInsteadOf(CPPStatementCreateTrigger createTrigger);

void WCDBStatementCreateTriggerConfigDelete(CPPStatementCreateTrigger createTrigger);
void WCDBStatementCreateTriggerConfigInsert(CPPStatementCreateTrigger createTrigger);
void WCDBStatementCreateTriggerConfigUpdate(CPPStatementCreateTrigger createTrigger);

void WCDBStatementCreateTriggerConfigColumns(CPPStatementCreateTrigger createTrigger,
                                             const CPPColumn* _Nullable columns,
                                             int colNum);
void WCDBStatementCreateTriggerConfigColumns2(CPPStatementCreateTrigger createTrigger,
                                              CPPCommonArray columns);
void WCDBStatementCreateTriggerConfigTable(CPPStatementCreateTrigger createTrigger,
                                           const char* _Nullable name);
void WCDBStatementCreateTriggerConfigForEachRow(CPPStatementCreateTrigger createTrigger);
void WCDBStatementCreateTriggerConfigWhen(CPPStatementCreateTrigger createTrigger,
                                          CPPExpression expression);

void WCDBStatementCreateTriggerExecuteInsert(CPPStatementCreateTrigger createTrigger,
                                             CPPStatementInsert insert);
void WCDBStatementCreateTriggerExecuteUpdate(CPPStatementCreateTrigger createTrigger,
                                             CPPStatementUpdate update);
void WCDBStatementCreateTriggerExecuteDelete(CPPStatementCreateTrigger createTrigger,
                                             CPPStatementDelete delete_);
void WCDBStatementCreateTriggerExecuteSelect(CPPStatementCreateTrigger createTrigger,
                                             CPPStatementSelect select);

WCDB_EXTERN_C_END
