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

#include "LiteralValueRust.h"

#include "LiteralValueBridge.h"

void* WCDBRustLiteralValueClassMethod(create, WCDBRustCommonValueParameter(value)) {
    WCDBRustCreateCommonValue(value) return (void*)WCDBLiteralValueCreate(value_common).innerValue;
}

long long WCDBRustLiteralValueClassMethod(createWithInt64, long long value) {
    return (long long)WCDBLiteralValueCreateWithInt64(value).innerValue;
}

// jlong WCDBRustLiteralValueClassMethod(createWithBool, jboolean value)
//{
//     return (jlong) WCDBLiteralValueCreateWithBool(value).innerValue;
// }
//
// jlong WCDBRustLiteralValueClassMethod(createWithDouble, jdouble value)
//{
//     return (jlong) WCDBLiteralValueCreateWithDouble(value).innerValue;
// }
//
// jlong WCDBRustLiteralValueClassMethod(createWithString, jstring value)
//{
//     WCDBRustGetStringCritical(value);
//     jlong result = (jlong) WCDBLiteralValueCreateWithString(valueString).innerValue;
//     WCDBRustReleaseStringCritical(value);
//     return result;
// }

long long WCDBRustLiteralValueClassMethodWithNoArg(createWithCurrentTime) {
    return (long long)WCDBLiteralValueCreateWithCurrentTime().innerValue;
}

long long WCDBRustLiteralValueClassMethodWithNoArg(createWithCurrentDate) {
    return (long long)WCDBLiteralValueCreateWithCurrentDate().innerValue;
}

long long WCDBRustLiteralValueClassMethodWithNoArg(createWithCurrentTimestamp) {
    return (long long)WCDBLiteralValueCreateWithCurrentTimestamp().innerValue;
}
