//
// Created by qiuwenchen on 2022/6/12.
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

#include "WinqBridge.h"

WCDB_EXTERN_C_BEGIN

CPPTableConstraint WCDBTableConstraintCreate(const char* _Nullable name);
void WCDBTableConstraintConfigPrimaryKey(CPPTableConstraint constraint);
void WCDBTableConstraintConfigUnique(CPPTableConstraint constraint);
void WCDBTableConstraintConfigIndexedColumn(CPPTableConstraint constraint,
                                            const CPPIndexedColumn* _Nullable columns,
                                            int colNum);
void WCDBTableConstraintConfigIndexedColumn2(CPPTableConstraint constraint,
                                             CPPCommonArray columns);
void WCDBTableConstraintConfigConfliction(CPPTableConstraint constraint,
                                          enum WCDBSyntaxConflictAction conflict);
void WCDBTableConstraintConfigCheckCondition(CPPTableConstraint constraint,
                                             CPPExpression expression);
void WCDBTableConstraintConfigForeignKey(CPPTableConstraint constraint,
                                         const CPPColumn* _Nullable columns,
                                         int colNum,
                                         CPPForeignKey foreignKey);

void WCDBTableConstraintConfigForeignKey2(CPPTableConstraint constraint,
                                          CPPCommonArray columns,
                                          CPPForeignKey foreignKey);

WCDB_EXTERN_C_END
