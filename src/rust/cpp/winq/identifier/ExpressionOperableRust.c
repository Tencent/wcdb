// Created by chenqiuwen on 2023/4/1.
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

#include "ExpressionOperableRust.h"

#include "ExpressionOperatableBridge.h"

#include <alloca.h>

// jlong WCDBRustExpressionOperableClassMethod(nullOperate, jint operandType, jlong operand,
// jboolean isNot)
//{
//     CPPCommonValue operand_common;
//     operand_common.type = operandType;
//     operand_common.intValue = operand;
//     return (jlong) WCDBExpressionNullOperate2(operand_common, isNot).innerValue;
// }
//
void* WCDBRustExpressionOperableClassMethod(binaryOperate,
                                            int leftType,
                                            long left,
                                            WCDBRustCommonValueParameter(right),
                                            int operatorType,
                                            bool isNot) {
    CPPCommonValue left_common;
    left_common.type = leftType;
    left_common.intValue = left;
    WCDBRustCreateCommonValue(right);
    void* ret = (void*)WCDBExpressionBinaryOperate2(left_common, right_common, operatorType, isNot)
                    .innerValue;
    //    WCDBRustTryReleaseStringInCommonValue(right); // todo qixinbing : 需要释放？
    return ret;
}

void* WCDBRustExpressionOperableClassMethod(betweenOperate,
                                            int operandType,
                                            long operand,
                                            WCDBRustCommonValueParameter(left),
                                            WCDBRustCommonValueParameter(right),
                                            bool isNot) {
    CPPCommonValue operand_common;
    operand_common.type = operandType;
    operand_common.intValue = operand;
    WCDBRustCreateCommonValueWithIsCritical(left, false);
    WCDBRustCreateCommonValueWithIsCritical(right, false);
    void* ret =
        (void*)WCDBExpressionBetweenOperate2(operand_common, left_common, right_common, isNot)
            .innerValue;
    //    WCDBRustTryReleaseStringInCommonValue(left);
    //    WCDBRustTryReleaseStringInCommonValue(right);
    return ret;
}

//
// jlong WCDBRustExpressionOperableClassMethod(inOperate,
//                                           jint operandType,
//                                           jlong operand,
//                                           WCDBRustCommonArrayParameter(values),
//                                           jboolean isNot)
//{
//    CPPCommonValue operand_common;
//    operand_common.type = operandType;
//    operand_common.intValue = operand;
//    jlong ret = 0;
//    WCDBRustCreateCommonArrayWithAction(
//    values,
//    ret
//    = (jlong) WCDBExpressionInOperate(operand_common, values_commonArray, isNot).innerValue);
//    return ret;
//}
//
// jlong WCDBRustExpressionOperableClassMethod(
// inTableOperate, jint operandType, jlong operand, jstring table, jboolean isNot)
//{
//    CPPCommonValue operand_common;
//    operand_common.type = operandType;
//    operand_common.intValue = operand;
//    WCDBRustGetStringCritical(table);
//    jlong ret
//    = (jlong) WCDBExpressionInTableOperate2(operand_common, tableString, isNot).innerValue;
//    WCDBRustReleaseStringCritical(table);
//    return ret;
//}
//
// jlong WCDBRustExpressionOperableClassMethod(
// inFunctionOperate, jint operandType, jlong operand, jstring func, jboolean isNot)
//{
//    CPPCommonValue operand_common;
//    operand_common.type = operandType;
//    operand_common.intValue = operand;
//    WCDBRustGetStringCritical(func);
//    jlong ret = (jlong) WCDBExpressionInFunctionOperate2(operand_common, funcString, isNot)
//                .innerValue;
//    WCDBRustReleaseStringCritical(func);
//    return ret;
//}
//
// jlong WCDBRustExpressionOperableClassMethod(
// inSelectionOperate, jint operandType, jlong operand, jlong select, jboolean isNot)
//{
//    CPPCommonValue operand_common;
//    operand_common.type = operandType;
//    operand_common.intValue = operand;
//    WCDBRustBridgeStruct(CPPStatementSelect, select);
//    return (jlong) WCDBExpressionInSelectionOperate2(operand_common, selectStruct, isNot)
//    .innerValue;
//}
//
// jlong WCDBRustExpressionOperableClassMethod(collateOperate, jint operandType, jlong operand,
// jstring collation)
//{
//    CPPCommonValue operand_common;
//    operand_common.type = operandType;
//    operand_common.intValue = operand;
//    WCDBRustGetStringCritical(collation);
//    jlong ret
//    = (jlong) WCDBExpressionCollateOperate2(operand_common, collationString).innerValue;
//    WCDBRustReleaseStringCritical(collation);
//    return ret;
//}