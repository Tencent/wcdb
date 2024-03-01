// Created by qiuwenchen on 2023/4/20.
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

#include "HandleStatementJNI.h"
#include "HandleStatementBridge.h"

jlong WCDBJNIHandleStatementClassMethod(getError, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    return (jlong) WCDBHandleStatementGetError(selfStruct).innerValue;
}

jboolean WCDBJNIHandleStatementClassMethod(prepare, jlong self, jlong statement)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementPrepare(selfStruct, (CPPObject *) statement);
}

jboolean WCDBJNIHandleStatementClassMethod(prepareSQL, jlong self, jstring sql)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBJNIGetString(sql);
    jboolean ret = WCDBHandleStatementPrepareSQL(selfStruct, sqlString);
    WCDBJNIReleaseString(sql);
    return ret;
}

jboolean WCDBJNIHandleStatementClassMethod(checkPrepared, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementCheckPrepared(selfStruct);
}

jboolean WCDBJNIHandleStatementClassMethod(step, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementStep(selfStruct);
}

void WCDBJNIHandleStatementClassMethod(reset, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBHandleStatementReset(selfStruct);
}

void WCDBJNIHandleStatementClassMethod(clearBindings, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBHandleStatementClearBindings(selfStruct);
}

void WCDBJNIHandleStatementClassMethod(finalize, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBHandleStatementFinalize(selfStruct);
}

jboolean WCDBJNIHandleStatementClassMethod(isDone, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementIsDone(selfStruct);
}

void WCDBJNIHandleStatementClassMethod(bindInteger, jlong self, jlong value, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBHandleStatementBindInteger(selfStruct, index, value);
}

void WCDBJNIHandleStatementClassMethod(bindDouble, jlong self, jdouble value, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBHandleStatementBindDouble(selfStruct, index, value);
}

void WCDBJNIHandleStatementClassMethod(bindText, jlong self, jstring value, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    int valueLength = 0;
    const jchar *valueUTF16String = NULL;
    if (value != NULL) {
        valueLength = (*env)->GetStringLength(env, value);
        valueUTF16String = (*env)->GetStringCritical(env, value, 0);
    }
    WCDBHandleStatementBindText16(
    selfStruct, index, (const short *) valueUTF16String, valueLength);
    if (valueUTF16String != NULL) {
        (*env)->ReleaseStringCritical(env, value, valueUTF16String);
    }
}

void WCDBJNIHandleStatementClassMethod(bindBLOB, jlong self, jbyteArray value, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBJNIGetByteArrayCritical(value);
    WCDBHandleStatementBindBlob(selfStruct, index, valueArray, valueLength);
    WCDBJNIReleaseByteArrayCritical(value);
}

void WCDBJNIHandleStatementClassMethod(bindNull, jlong self, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBHandleStatementBindNull(selfStruct, index);
}

jint WCDBJNIHandleStatementClassMethod(bindParameterIndex, jlong self, jstring parameterName)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBJNIGetString(parameterName);
    jint index = WCDBHandleStatementBindParameterIndex(selfStruct, parameterNameString);
    WCDBJNIReleaseString(parameterName);
    return index;
}

jint WCDBJNIHandleStatementClassMethod(getColumnType, jlong self, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementGetColumnType(selfStruct, index);
}

jlong WCDBJNIHandleStatementClassMethod(getInteger, jlong self, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementGetInteger(selfStruct, index);
}

jdouble WCDBJNIHandleStatementClassMethod(getDouble, jlong self, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementGetDouble(selfStruct, index);
}

jstring WCDBJNIHandleStatementClassMethod(getText, jlong self, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    const jchar *utf16Value
    = (const jchar *) WCDBHandleStatementGetText16(selfStruct, index);
    jsize utf16ValueLength = WCDBHandleStatementGetText16Length(selfStruct, index);
    return (*env)->NewString(env, utf16Value, utf16ValueLength);
}

jbyteArray WCDBJNIHandleStatementClassMethod(getBLOB, jlong self, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    jbyte *buffer = (jbyte *) WCDBHandleStatementGetBlob(selfStruct, index);
    jsize size = (jsize) WCDBHandleStatementGetColumnSize(selfStruct, index);
    if (buffer == NULL || size == 0) {
        return (*env)->NewByteArray(env, 0);
    }
    jbyteArray array = (*env)->NewByteArray(env, size);
    if (array != NULL) {
        (*env)->SetByteArrayRegion(env, array, 0, size, buffer);
    }
    return array;
}

jint WCDBJNIHandleStatementClassMethod(getColumnCount, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementGetColumnCount(selfStruct);
}

jstring WCDBJNIHandleStatementClassMethod(getColumnName, jlong self, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBJNICreateJStringAndReturn(WCDBHandleStatementGetColumnName(selfStruct, index));
}

jstring WCDBJNIHandleStatementClassMethod(getOriginalColumnName, jlong self, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBJNICreateJStringAndReturn(WCDBHandleStatementGetOriginalColumnName(selfStruct, index));
}

jstring WCDBJNIHandleStatementClassMethod(getColumnTableName, jlong self, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBJNICreateJStringAndReturn(WCDBHandleStatementGetColumnTableName(selfStruct, index));
}

jboolean WCDBJNIHandleStatementClassMethod(isReadOnly, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementIsReadOnly(selfStruct);
}
