//
// Created by qiuwenchen on 2022/7/15.
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

CPPStatementInsert WCDBStatementInsertCreate(void);

void WCDBStatementInsertConfigWith(CPPStatementInsert insert,
                                   const CPPCommonTableExpression* _Nullable exps,
                                   int expNum);
void WCDBStatementInsertConfigRecursive(CPPStatementInsert insert);

void WCDBStatementInsertConfigTable(CPPStatementInsert insert, const char* _Nullable name);
void WCDBStatementInsertConfigSchema(CPPStatementInsert insert, CPPSchema schema);
void WCDBStatementInsertConfigSchema2(CPPStatementInsert insert, CPPCommonValue schema);

void WCDBStatementInsertConfigConfiction(CPPStatementInsert insert,
                                         enum WCDBSyntaxConflictAction action);

void WCDBStatementInsertConfigAlias(CPPStatementInsert insert, const char* _Nullable name);

void WCDBStatementInsertConfigColumns(CPPStatementInsert insert,
                                      const CPPColumn* _Nullable columns,
                                      int colNum);
void WCDBStatementInsertConfigColumns2(CPPStatementInsert insert, CPPCommonArray columns);
void WCDBStatementInsertConfigValues(CPPStatementInsert insert,
                                     const CPPExpression* _Nullable exps,
                                     int expNum);
void WCDBStatementInsertConfigValuesWithMultiTypeArray(CPPStatementInsert insert,
                                                       CPPMultiTypeArray values);
void WCDBStatementInsertConfigValuesWithBindParameters(CPPStatementInsert insert,
                                                       int parametersCount);
void WCDBStatementInsertConfigSelect(CPPStatementInsert insert, CPPStatementSelect select);

void WCDBStatementInsertConfigDefaultValues(CPPStatementInsert insert);
void WCDBStatementInsertConfigUpsert(CPPStatementInsert insert, CPPUpsert upsert);

WCDB_EXTERN_C_END
