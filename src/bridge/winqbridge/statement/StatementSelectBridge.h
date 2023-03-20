//
// Created by qiuwenchen on 2022/5/30.
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

CPPStatementSelect WCDBStatementSelectCreate(void);
void WCDBStatementSelectConfigWith(CPPStatementSelect select,
                                   const CPPCommonTableExpression* _Nullable exps,
                                   int expNum);
void WCDBStatementSelectConfigRecursive(CPPStatementSelect select);
void WCDBStatementSelectConfigResultColumns(CPPStatementSelect select,
                                            const CPPResultColumn* _Nullable resultColumn,
                                            int columnNum);
void WCDBStatementSelectConfigDistinct(CPPStatementSelect select);
void WCDBStatementSelectConfigFromTableOrSubqueries(CPPStatementSelect select,
                                                    const CPPTableOrSubquery* _Nullable tableOrSubqueries,
                                                    int queryNum);
void WCDBStatementSelectConfigWhere(CPPStatementSelect select, CPPExpression condition);
void WCDBStatementSelectConfigGroups(CPPStatementSelect select,
                                     const CPPExpression* _Nullable exps,
                                     int expNum);
void WCDBStatementSelectConfigHaving(CPPStatementSelect select, CPPExpression expression);
void WCDBStatementSelectConfigWindow(CPPStatementSelect select, const char* _Nullable windowName);
void WCDBStatementSelectConfigAs(CPPStatementSelect select, CPPWindowDef windowDef);
void WCDBStatementSelectConfigValues(CPPStatementSelect select,
                                     const CPPExpression* _Nullable exps,
                                     int expNum);
void WCDBStatementSelectConfigUnion(CPPStatementSelect select);
void WCDBStatementSelectConfigUnionAll(CPPStatementSelect select);
void WCDBStatementSelectConfigIntersect(CPPStatementSelect select);
void WCDBStatementSelectConfigExcept(CPPStatementSelect select);
void WCDBStatementSelectConfigOrders(CPPStatementSelect select,
                                     const CPPOrderingTerm* _Nullable orders,
                                     int orderNum);
void WCDBStatementSelectConfigLimitFromTo(CPPStatementSelect select,
                                          CPPExpression from,
                                          CPPExpression to);
void WCDBStatementSelectConfigLimitFrom(CPPStatementSelect select, CPPExpression limit);
void WCDBStatementSelectConfigLimitOffset(CPPStatementSelect select, CPPExpression offset);

WCDB_EXTERN_C_END
