// Created by chenqiuwen on 2023/4/3.
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

#define WCDBJNIColumnConstraintFuncName(funcName)                              \
    WCDBJNI(ColumnConstraint, funcName)
#define WCDBJNIColumnConstraintObjectMethod(funcName, ...)                     \
    WCDBJNIObjectMethod(ColumnConstraint, funcName, __VA_ARGS__)
#define WCDBJNIColumnConstraintClassMethodWithNoArg(funcName)                  \
    WCDBJNIClassMethodWithNoArg(ColumnConstraint, funcName)
#define WCDBJNIColumnConstraintClassMethod(funcName, ...)                      \
    WCDBJNIClassMethod(ColumnConstraint, funcName, __VA_ARGS__)

jlong WCDBJNIColumnConstraintClassMethod(create, jstring name);

void WCDBJNIColumnConstraintClassMethod(configPrimaryKey, jlong constraint);

void WCDBJNIColumnConstraintClassMethod(configOrder, jlong constraint, jint order);

void WCDBJNIColumnConstraintClassMethod(configConflictAction, jlong constraint, jint conflictAction);

void WCDBJNIColumnConstraintClassMethod(configAutoIncrement, jlong constraint);

void WCDBJNIColumnConstraintClassMethod(configNotNull, jlong constraint);

void WCDBJNIColumnConstraintClassMethod(configUnique, jlong constraint);

void WCDBJNIColumnConstraintClassMethod(configCheck, jlong constraint, jlong expression);

void WCDBJNIColumnConstraintClassMethod(configDefaultValue,
                                        jlong constraint,
                                        WCDBJNICommonValueParameter(value));

void WCDBJNIColumnConstraintClassMethod(configCollation, jlong constraint, jstring collation);

void WCDBJNIColumnConstraintClassMethod(configForeignKey, jlong constraint, jlong foreignKey);

void WCDBJNIColumnConstraintClassMethod(configUnindexed, jlong constraint);
