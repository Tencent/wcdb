//
// Created by qiuwenchen on 2022/5/29.
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

CPPColumnConstraint WCDBColumnConstraintCreate(const char* _Nullable name);
void WCDBColumnConstraintConfigPrimaryKey(CPPColumnConstraint constraint);
void WCDBColumnConstraintConfigOrder(CPPColumnConstraint constraint,
                                     enum WCDBSyntaxOrder order);
void WCDBColumnConstraintConfigCoflictAction(CPPColumnConstraint constraint,
                                             enum WCDBSyntaxConflictAction conflictAction);
void WCDBColumnConstraintConfigAutoIncrement(CPPColumnConstraint constraint);
void WCDBColumnConstraintConfigNotNull(CPPColumnConstraint constraint);
void WCDBColumnConstraintConfigUnique(CPPColumnConstraint constraint);
void WCDBColumnConstraintConfigCheck(CPPColumnConstraint constraint, CPPExpression expression);
void WCDBColumnConstraintConfigDefaultValue(CPPColumnConstraint constraint,
                                            CPPExpression expression);
void WCDBColumnConstraintConfigDefaultValue2(CPPColumnConstraint constraint,
                                             CPPCommonValue expression);
void WCDBColumnConstraintConfigCollation(CPPColumnConstraint constraint,
                                         const char* _Nullable collation);
void WCDBColumnConstraintConfigForeignKey(CPPColumnConstraint constraint,
                                          CPPForeignKey foreignKey);
void WCDBColumnConstraintConfigUnIndexed(CPPColumnConstraint constraint);

WCDB_EXTERN_C_END
