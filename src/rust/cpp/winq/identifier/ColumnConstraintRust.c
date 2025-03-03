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

#include "ColumnConstraintRust.h"

#include "ColumnConstraintBridge.h"

void* WCDBRustColumnConstraintClassMethod(create, const char* name) {
    void* ret = (void*)WCDBColumnConstraintCreate(name).innerValue;
    return ret;
}

void WCDBRustColumnConstraintClassMethod(configPrimaryKey, void* constraint) {
    WCDBRustBridgeStruct(CPPColumnConstraint, constraint);
    WCDBColumnConstraintConfigPrimaryKey(constraintStruct);
}

// void WCDBRustColumnConstraintClassMethod(configOrder, jlong constraint, jint order)
//{
//     WCDBRustBridgeStruct(CPPColumnConstraint, constraint);
//     WCDBColumnConstraintConfigOrder(constraintStruct, order);
// }
//
// void WCDBRustColumnConstraintClassMethod(configConflictAction, jlong constraint, jint
// conflictAction)
//{
//     WCDBRustBridgeStruct(CPPColumnConstraint, constraint);
//     WCDBColumnConstraintConfigCoflictAction(constraintStruct, conflictAction);
// }
//
void WCDBRustColumnConstraintClassMethod(configAutoIncrement, void* constraint) {
    WCDBRustBridgeStruct(CPPColumnConstraint, constraint);
    WCDBColumnConstraintConfigAutoIncrement(constraintStruct);
}

void WCDBRustColumnConstraintClassMethod(configNotNull, void* constraint) {
    WCDBRustBridgeStruct(CPPColumnConstraint, constraint);
    WCDBColumnConstraintConfigNotNull(constraintStruct);
}

void WCDBRustColumnConstraintClassMethod(configUnique, void* constraint) {
    WCDBRustBridgeStruct(CPPColumnConstraint, constraint);
    WCDBColumnConstraintConfigUnique(constraintStruct);
}
//
// void WCDBRustColumnConstraintClassMethod(configCheck, jlong constraint, jlong expression)
//{
//    WCDBRustBridgeStruct(CPPColumnConstraint, constraint);
//    WCDBRustBridgeStruct(CPPExpression, expression);
//    WCDBColumnConstraintConfigCheck(constraintStruct, expressionStruct);
//}
//
void WCDBRustColumnConstraintClassMethod(configDefaultValue,
                                         void* constraint,
                                         WCDBRustCommonValueParameter(value)) {
    WCDBRustBridgeStruct(CPPColumnConstraint, constraint);
    WCDBRustCreateCommonValueWithIsCritical(value, true);
    WCDBColumnConstraintConfigDefaultValue2(constraintStruct, value_common);
}
//
// void WCDBRustColumnConstraintClassMethod(configCollation, jlong constraint, jstring collation)
//{
//    WCDBRustBridgeStruct(CPPColumnConstraint, constraint);
//    WCDBRustGetStringCritical(collation);
//    WCDBColumnConstraintConfigCollation(constraintStruct, collationString);
//    WCDBRustReleaseStringCritical(collation);
//}
//
// void WCDBRustColumnConstraintClassMethod(configForeignKey, jlong constraint, jlong foreignKey)
//{
//    WCDBRustBridgeStruct(CPPColumnConstraint, constraint);
//    WCDBRustBridgeStruct(CPPForeignKey, foreignKey);
//    WCDBColumnConstraintConfigForeignKey(constraintStruct, foreignKeyStruct);
//}
//
void WCDBRustColumnConstraintClassMethod(configUnIndex, void* constraint) {
    WCDBRustBridgeStruct(CPPColumnConstraint, constraint);
    WCDBColumnConstraintConfigUnIndexed(constraintStruct);
}
