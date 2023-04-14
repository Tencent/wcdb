// Created by chenqiuwen on 2023/4/8.
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

#define WCDBJNITableConstraintFuncName(funcName)                               \
    WCDBJNI(TableConstraint, funcName)
#define WCDBJNITableConstraintObjectMethod(funcName, ...)                      \
    WCDBJNIObjectMethod(TableConstraint, funcName, __VA_ARGS__)
#define WCDBJNITableConstraintClassMethodWithNoArg(funcName)                   \
    WCDBJNIClassMethodWithNoArg(TableConstraint, funcName)
#define WCDBJNITableConstraintClassMethod(funcName, ...)                       \
    WCDBJNIClassMethod(TableConstraint, funcName, __VA_ARGS__)

jlong WCDBJNITableConstraintObjectMethod(create, jstring name);
void WCDBJNITableConstraintObjectMethod(configPrimaryKey, jlong constraint);
void WCDBJNITableConstraintObjectMethod(configUnique, jlong constraint);
void WCDBJNITableConstraintObjectMethod(configIndexedColumn,
                                        jlong constraint,
                                        WCDBJNIObjectOrStringArrayParameter(indexedColumns));
void WCDBJNITableConstraintObjectMethod(configConfliction, jlong constraint, int conflictAction);
void WCDBJNITableConstraintObjectMethod(configCheckCondition, jlong constraint, jlong expression);
void WCDBJNITableConstraintObjectMethod(configForeignKey,
                                        jlong constraint,
                                        WCDBJNIObjectOrStringArrayParameter(columns),
                                        jlong foreignKey);
