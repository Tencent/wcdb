//
// Created by qiuwenchen on 2022/7/3.
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

CPPStatementDelete WCDBStatementDeleteCreate(void);
void WCDBStatementDeleteConfigWith(CPPStatementDelete deleteStatement,
                                   const CPPCommonTableExpression* _Nullable expressions,
                                   int expNum);
void WCDBStatementDeleteConfigRecursive(CPPStatementDelete deleteStatement);
void WCDBStatementDeleteConfigDeleteFrom(CPPStatementDelete deleteStatement,
                                         CPPQualifiedTable table);

void WCDBStatementDeleteConfigDeleteFrom2(CPPStatementDelete deleteStatement,
                                          CPPCommonValue table);

void WCDBStatementDeleteConfigWhere(CPPStatementDelete deleteStatement, CPPExpression expression);
void WCDBStatementDeleteConfigOrder(CPPStatementDelete deleteStatement,
                                    const CPPOrderingTerm* _Nullable orders,
                                    int orderNum);
void WCDBStatementDeleteConfigLimitRange(CPPStatementDelete deleteStatement,
                                         CPPExpression from,
                                         CPPExpression to);
void WCDBStatementDeleteConfigLimitCount(CPPStatementDelete deleteStatement,
                                         CPPExpression limit);
void WCDBStatementDeleteConfigOffset(CPPStatementDelete deleteStatement, CPPExpression offset);

void WCDBStatementDeleteConfigLimitRange2(CPPStatementDelete deleteStatement,
                                          CPPCommonValue from,
                                          CPPCommonValue to);
void WCDBStatementDeleteConfigLimitCount2(CPPStatementDelete deleteStatement,
                                          CPPCommonValue limit);
void WCDBStatementDeleteConfigOffset2(CPPStatementDelete deleteStatement, CPPCommonValue offset);

WCDB_EXTERN_C_END
