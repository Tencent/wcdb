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

#include "HandleStatementRust.h"

#include "HandleStatementBridge.h"

void* WCDBRustHandleStatementClassMethod(getError, void* self) {
    WCDBRustBridgeStruct(CPPHandleStatement, self);
    return (void*)WCDBHandleStatementGetError(selfStruct).innerValue;
}

bool WCDBRustHandleStatementClassMethod(prepare, void* self, void* statement) {
    WCDBRustBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementPrepare(selfStruct, (CPPObject*)statement);
}

// bool WCDBRustHandleStatementClassMethod(prepareSQL, void* self, jstring sql)
//{
//     WCDBRustBridgeStruct(CPPHandleStatement, self);
//     WCDBRustGetString(sql);
//     bool ret = WCDBHandleStatementPrepareSQL(selfStruct, sqlString);
//     WCDBRustReleaseString(sql);
//     return ret;
// }
//
// bool WCDBRustHandleStatementClassMethod(checkPrepared, void* self)
//{
//     WCDBRustBridgeStruct(CPPHandleStatement, self);
//     return WCDBHandleStatementCheckPrepared(selfStruct);
// }

bool WCDBRustHandleStatementClassMethod(step, void* self) {
    WCDBRustBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementStep(selfStruct);
}

void WCDBRustHandleStatementClassMethod(reset, void* self) {
    WCDBRustBridgeStruct(CPPHandleStatement, self);
    WCDBHandleStatementReset(selfStruct);
}

// void WCDBRustHandleStatementClassMethod(clearBindings, void* self)
//{
//     WCDBRustBridgeStruct(CPPHandleStatement, self);
//     WCDBHandleStatementClearBindings(selfStruct);
// }

void WCDBRustHandleStatementClassMethod(finalize, void* self) {
    WCDBRustBridgeStruct(CPPHandleStatement, self);
    WCDBHandleStatementFinalize(selfStruct);
}

bool WCDBRustHandleStatementClassMethod(isDone, void* self) {
    WCDBRustBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementIsDone(selfStruct);
}

void WCDBRustHandleStatementClassMethod(bindInteger, void* self, long long value, int index) {
    WCDBRustBridgeStruct(CPPHandleStatement, self);
    WCDBHandleStatementBindInteger(selfStruct, index, value);
}

void WCDBRustHandleStatementClassMethod(bindDouble, void* self, double value, int index) {
    WCDBRustBridgeStruct(CPPHandleStatement, self);
    WCDBHandleStatementBindDouble(selfStruct, index, value);
}

void WCDBRustHandleStatementClassMethod(bindText, void* self, const char* value, int index) {
    WCDBRustBridgeStruct(CPPHandleStatement, self);
    WCDBHandleStatementBindText(selfStruct, index, value);
}

// void WCDBRustHandleStatementClassMethod(bindBLOB, void* self, jbyteArray value, jint index)
//{
//     WCDBRustBridgeStruct(CPPHandleStatement, self);
//     WCDBRustGetByteArrayCritical(value);
//     WCDBHandleStatementBindBlob(selfStruct, index, valueArray, valueLength);
//     WCDBRustReleaseByteArrayCritical(value);
// }

void WCDBRustHandleStatementClassMethod(bindNull, void* self, int index) {
    WCDBRustBridgeStruct(CPPHandleStatement, self);
    WCDBHandleStatementBindNull(selfStruct, index);
}

// jint WCDBRustHandleStatementClassMethod(bindParameterIndex, void* self, jstring parameterName)
//{
//     WCDBRustBridgeStruct(CPPHandleStatement, self);
//     WCDBRustGetString(parameterName);
//     jint index = WCDBHandleStatementBindParameterIndex(selfStruct, parameterNameString);
//     WCDBRustReleaseString(parameterName);
//     return index;
// }
//
// jint WCDBRustHandleStatementClassMethod(getColumnType, void* self, jint index)
//{
//     WCDBRustBridgeStruct(CPPHandleStatement, self);
//     return WCDBHandleStatementGetColumnType(selfStruct, index);
// }

long long WCDBRustHandleStatementClassMethod(getInteger, void* self, int index) {
    WCDBRustBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementGetInteger(selfStruct, index);
}

double WCDBRustHandleStatementClassMethod(getDouble, void* self, int index) {
    WCDBRustBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementGetDouble(selfStruct, index);
}

const char* WCDBRustHandleStatementClassMethod(getText, void* self, int index) {
    WCDBRustBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementGetText(selfStruct, index);
}
//
// jbyteArray WCDBRustHandleStatementClassMethod(getBLOB, void* self, jint index)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    jbyte *buffer = (jbyte *) WCDBHandleStatementGetBlob(selfStruct, index);
//    jsize size = (jsize) WCDBHandleStatementGetColumnSize(selfStruct, index);
//    if (buffer == NULL || size == 0) {
//        return (*env)->NewByteArray(env, 0);
//    }
//    jbyteArray array = (*env)->NewByteArray(env, size);
//    if (array != NULL) {
//        (*env)->SetByteArrayRegion(env, array, 0, size, buffer);
//    }
//    return array;
//}
//
// jint WCDBRustHandleStatementClassMethod(getColumnCount, void* self)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    return WCDBHandleStatementGetColumnCount(selfStruct);
//}
//
// jstring WCDBRustHandleStatementClassMethod(getColumnName, void* self, jint index)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    WCDBRustCreateJStringAndReturn(WCDBHandleStatementGetColumnName(selfStruct, index));
//}
//
// jstring WCDBRustHandleStatementClassMethod(getOriginalColumnName, void* self, jint index)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    WCDBRustCreateJStringAndReturn(WCDBHandleStatementGetOriginalColumnName(selfStruct, index));
//}
//
// jstring WCDBRustHandleStatementClassMethod(getColumnTableName, void* self, jint index)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    WCDBRustCreateJStringAndReturn(WCDBHandleStatementGetColumnTableName(selfStruct, index));
//}
//
// bool WCDBRustHandleStatementClassMethod(isReadOnly, void* self)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    return WCDBHandleStatementIsReadOnly(selfStruct);
//}
