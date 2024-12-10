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

//jlong WCDBRustHandleStatementClassMethod(getError, jlong self)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    return (jlong) WCDBHandleStatementGetError(selfStruct).innerValue;
//}
//
//jboolean WCDBRustHandleStatementClassMethod(prepare, jlong self, jlong statement)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    return WCDBHandleStatementPrepare(selfStruct, (CPPObject *) statement);
//}
//
//jboolean WCDBRustHandleStatementClassMethod(prepareSQL, jlong self, jstring sql)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    WCDBRustGetString(sql);
//    jboolean ret = WCDBHandleStatementPrepareSQL(selfStruct, sqlString);
//    WCDBRustReleaseString(sql);
//    return ret;
//}
//
//jboolean WCDBRustHandleStatementClassMethod(checkPrepared, jlong self)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    return WCDBHandleStatementCheckPrepared(selfStruct);
//}
//
//jboolean WCDBRustHandleStatementClassMethod(step, jlong self)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    return WCDBHandleStatementStep(selfStruct);
//}
//
//void WCDBRustHandleStatementClassMethod(reset, jlong self)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    WCDBHandleStatementReset(selfStruct);
//}
//
//void WCDBRustHandleStatementClassMethod(clearBindings, jlong self)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    WCDBHandleStatementClearBindings(selfStruct);
//}
//
//void WCDBRustHandleStatementClassMethod(finalize, jlong self)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    WCDBHandleStatementFinalize(selfStruct);
//}
//
//jboolean WCDBRustHandleStatementClassMethod(isDone, jlong self)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    return WCDBHandleStatementIsDone(selfStruct);
//}

void WCDBRustHandleStatementClassMethod(bindInteger, void* self, long long value, int index)
{
    WCDBRustBridgeStruct(CPPHandleStatement, self);
    WCDBHandleStatementBindInteger(selfStruct, index, value);
}

//void WCDBRustHandleStatementClassMethod(bindDouble, jlong self, jdouble value, jint index)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    WCDBHandleStatementBindDouble(selfStruct, index, value);
//}
//
//void WCDBRustHandleStatementClassMethod(bindText, jlong self, jstring value, jint index)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    int valueLength = 0;
//    const jchar *valueUTF16String = NULL;
//    if (value != NULL) {
//        valueLength = (*env)->GetStringLength(env, value);
//        valueUTF16String = (*env)->GetStringCritical(env, value, 0);
//    }
//    WCDBHandleStatementBindText16(
//    selfStruct, index, (const short *) valueUTF16String, valueLength);
//    if (valueUTF16String != NULL) {
//        (*env)->ReleaseStringCritical(env, value, valueUTF16String);
//    }
//}
//
//void WCDBRustHandleStatementClassMethod(bindBLOB, jlong self, jbyteArray value, jint index)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    WCDBRustGetByteArrayCritical(value);
//    WCDBHandleStatementBindBlob(selfStruct, index, valueArray, valueLength);
//    WCDBRustReleaseByteArrayCritical(value);
//}
//
//void WCDBRustHandleStatementClassMethod(bindNull, jlong self, jint index)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    WCDBHandleStatementBindNull(selfStruct, index);
//}
//
//jint WCDBRustHandleStatementClassMethod(bindParameterIndex, jlong self, jstring parameterName)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    WCDBRustGetString(parameterName);
//    jint index = WCDBHandleStatementBindParameterIndex(selfStruct, parameterNameString);
//    WCDBRustReleaseString(parameterName);
//    return index;
//}
//
//jint WCDBRustHandleStatementClassMethod(getColumnType, jlong self, jint index)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    return WCDBHandleStatementGetColumnType(selfStruct, index);
//}

long long WCDBRustHandleStatementClassMethod(getInteger, void* self, int index)
{
    WCDBRustBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementGetInteger(selfStruct, index);
}

//jdouble WCDBRustHandleStatementClassMethod(getDouble, jlong self, jint index)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    return WCDBHandleStatementGetDouble(selfStruct, index);
//}
//
//jstring WCDBRustHandleStatementClassMethod(getText, jlong self, jint index)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    const jchar *utf16Value
//    = (const jchar *) WCDBHandleStatementGetText16(selfStruct, index);
//    jsize utf16ValueLength = WCDBHandleStatementGetText16Length(selfStruct, index);
//    return (*env)->NewString(env, utf16Value, utf16ValueLength);
//}
//
//jbyteArray WCDBRustHandleStatementClassMethod(getBLOB, jlong self, jint index)
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
//jint WCDBRustHandleStatementClassMethod(getColumnCount, jlong self)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    return WCDBHandleStatementGetColumnCount(selfStruct);
//}
//
//jstring WCDBRustHandleStatementClassMethod(getColumnName, jlong self, jint index)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    WCDBRustCreateJStringAndReturn(WCDBHandleStatementGetColumnName(selfStruct, index));
//}
//
//jstring WCDBRustHandleStatementClassMethod(getOriginalColumnName, jlong self, jint index)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    WCDBRustCreateJStringAndReturn(WCDBHandleStatementGetOriginalColumnName(selfStruct, index));
//}
//
//jstring WCDBRustHandleStatementClassMethod(getColumnTableName, jlong self, jint index)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    WCDBRustCreateJStringAndReturn(WCDBHandleStatementGetColumnTableName(selfStruct, index));
//}
//
//jboolean WCDBRustHandleStatementClassMethod(isReadOnly, jlong self)
//{
//    WCDBRustBridgeStruct(CPPHandleStatement, self);
//    return WCDBHandleStatementIsReadOnly(selfStruct);
//}
