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

#include "ErrorRust.h"

#include "ErrorBridge.h"
#include "assert.h"

extern void WCDBExceptionAddInfo(void* key_values_raw,
                                 const char* key,
                                 int value_type,
                                 long long int_value,
                                 double double_value,
                                 const char* string_value);

int WCDBRustErrorClassMethod(getLevel, void* error) {
    WCDBRustBridgeStruct(CPPError, error);
    return WCDBErrorGetLevel(errorStruct);
}

int WCDBRustErrorClassMethod(getCode, void* error) {
    WCDBRustBridgeStruct(CPPError, error);
    return WCDBErrorGetCode(errorStruct);
}

const char* WCDBRustErrorClassMethod(getMessage, void* error) {
    WCDBRustBridgeStruct(CPPError, error);
    return WCDBErrorGetMsg(errorStruct);
}

void WCDBRustErrorEnumerateInfoCallback(void* context, const char* key, CPPCommonValue value) {
    long long intValue = 0;
    double doubleValue = 0;
    const char* stringValue = NULL;
    switch (value.type) {
        case WCDBBridgedType_Int:
            intValue = (long long)value.intValue;
            break;
        case WCDBBridgedType_Double:
            doubleValue = value.doubleValue;
            break;
        case WCDBBridgedType_String:
            stringValue = (const char*)value.intValue;
            break;
        default:
            break;
    }
    WCDBExceptionAddInfo(context, key, value.type, intValue, doubleValue, stringValue);
}

void WCDBRustErrorObjectMethod(enumerateInfo, void* map, void* error) {
    WCDBRustBridgeStruct(CPPError, error);
    WCDBErrorEnumerateAllInfo(errorStruct, map,
                              (StringViewMapEnumerator)&WCDBRustErrorEnumerateInfoCallback);
}
