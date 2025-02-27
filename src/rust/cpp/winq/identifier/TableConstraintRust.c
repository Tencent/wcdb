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

#include "TableConstraintRust.h"

#include "TableConstraintBridge.h"

void* WCDBRustTableConstraintClassMethod(create, const char* name) {
    void* ret = (void*)WCDBTableConstraintCreate(name).innerValue;
    return ret;
}

void WCDBRustTableConstraintClassMethod(configPrimaryKey, void* constraint) {
    WCDBRustBridgeStruct(CPPTableConstraint, constraint);
    WCDBTableConstraintConfigPrimaryKey(constraintStruct);
}

// void WCDBRustTableConstraintClassMethod(configUnique, jlong constraint)
//{
//     WCDBRustBridgeStruct(CPPTableConstraint, constraint);
//     WCDBTableConstraintConfigUnique(constraintStruct);
// }
//
void WCDBRustTableConstraintClassMethod(configIndexedColumn,
                                        void* constraint,
                                        WCDBRustObjectOrStringArrayParameter(indexedColumns)) {
    WCDBRustBridgeStruct(CPPTableConstraint, constraint);
    WCDBRustCreateObjectOrStringArrayCriticalWithAction(
        indexedColumns,
        WCDBTableConstraintConfigIndexedColumn2(constraintStruct, indexedColumns_commonArray));
}

// void WCDBRustTableConstraintClassMethod(configConfliction, jlong constraint, int conflictAction)
//{
//     WCDBRustBridgeStruct(CPPTableConstraint, constraint);
//     WCDBTableConstraintConfigConfliction(constraintStruct, conflictAction);
// }
//
// void WCDBRustTableConstraintClassMethod(configCheckCondition, jlong constraint, jlong expression)
//{
//     WCDBRustBridgeStruct(CPPTableConstraint, constraint);
//     WCDBRustBridgeStruct(CPPExpression, expression);
//     WCDBTableConstraintConfigCheckCondition(constraintStruct, expressionStruct);
// }
//
// void WCDBRustTableConstraintClassMethod(configForeignKey,
//                                        jlong constraint,
//                                        WCDBRustObjectOrStringArrayParameter(columns),
//                                        jlong foreignKey)
//{
//     WCDBRustBridgeStruct(CPPTableConstraint, constraint);
//     WCDBRustBridgeStruct(CPPForeignKey, foreignKey);
//     WCDBRustCreateObjectOrStringArrayCriticalWithAction(
//     columns, WCDBTableConstraintConfigForeignKey2(constraintStruct, columns_commonArray,
//     foreignKeyStruct));
// }
