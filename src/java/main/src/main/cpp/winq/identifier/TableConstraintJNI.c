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

#include "TableConstraintJNI.h"
#include "TableConstraintBridge.h"

jlong WCDBJNITableConstraintClassMethod(create, jstring name)
{
    WCDBJNIGetStringCritical(name);
    jlong ret = (jlong) WCDBTableConstraintCreate(nameString).innerValue;
    WCDBJNIReleaseStringCritical(name);
    return ret;
}

void WCDBJNITableConstraintClassMethod(configPrimaryKey, jlong constraint)
{
    WCDBJNIBridgeStruct(CPPTableConstraint, constraint);
    WCDBTableConstraintConfigPrimaryKey(constraintStruct);
}

void WCDBJNITableConstraintClassMethod(configUnique, jlong constraint)
{
    WCDBJNIBridgeStruct(CPPTableConstraint, constraint);
    WCDBTableConstraintConfigUnique(constraintStruct);
}

void WCDBJNITableConstraintClassMethod(configIndexedColumn,
                                       jlong constraint,
                                       WCDBJNIObjectOrStringArrayParameter(indexedColumns))
{
    WCDBJNIBridgeStruct(CPPTableConstraint, constraint);
    WCDBJNICreateObjectOrStringArrayCriticalWithAction(
    indexedColumns,
    WCDBTableConstraintConfigIndexedColumn2(constraintStruct, indexedColumns_commonArray));
}

void WCDBJNITableConstraintClassMethod(configConfliction, jlong constraint, int conflictAction)
{
    WCDBJNIBridgeStruct(CPPTableConstraint, constraint);
    WCDBTableConstraintConfigConfliction(constraintStruct, conflictAction);
}

void WCDBJNITableConstraintClassMethod(configCheckCondition, jlong constraint, jlong expression)
{
    WCDBJNIBridgeStruct(CPPTableConstraint, constraint);
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBTableConstraintConfigCheckCondition(constraintStruct, expressionStruct);
}

void WCDBJNITableConstraintClassMethod(configForeignKey,
                                       jlong constraint,
                                       WCDBJNIObjectOrStringArrayParameter(columns),
                                       jlong foreignKey)
{
    WCDBJNIBridgeStruct(CPPTableConstraint, constraint);
    WCDBJNIBridgeStruct(CPPForeignKey, foreignKey);
    WCDBJNICreateObjectOrStringArrayCriticalWithAction(
    columns, WCDBTableConstraintConfigForeignKey2(constraintStruct, columns_commonArray, foreignKeyStruct));
}
