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

jlong WCDBJNIHandleStatementObjectMethod(getError, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    return (jlong) WCDBHandleStatementGetError(selfStruct).innerValue;
}

jboolean WCDBJNIHandleStatementObjectMethod(prepare, jlong self, jlong statement)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementPrepare(selfStruct, (CPPObject*) statement);
}

jboolean WCDBJNIHandleStatementObjectMethod(prepareSQL, jlong self, jstring sql)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBJNIGetString(sql);
    jboolean ret = WCDBHandleStatementPrepareSQL(selfStruct, sqlString);
    WCDBJNIReleaseString(sql);
    return ret;
}

jboolean WCDBJNIHandleStatementObjectMethod(checkPrepared, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementCheckPrepared(selfStruct);
}

jboolean WCDBJNIHandleStatementObjectMethod(step, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementStep(selfStruct);
}

void WCDBJNIHandleStatementObjectMethod(reset, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBHandleStatementReset(selfStruct);
}

void WCDBJNIHandleStatementObjectMethod(finalize, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBHandleStatementFinalize(selfStruct);
}

jboolean WCDBJNIHandleStatementObjectMethod(isDone, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementIsDone(selfStruct);
}

void WCDBJNIHandleStatementObjectMethod(bindInteger, jlong self, jlong value, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBHandleStatementBindInteger(selfStruct, index, value);
}

void WCDBJNIHandleStatementObjectMethod(bindDouble, jlong self, jdouble value, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBHandleStatementBindDouble(selfStruct, index, value);
}

void WCDBJNIHandleStatementObjectMethod(bindText, jlong self, jstring value, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBJNIGetString(value);
    WCDBHandleStatementBindText(selfStruct, index, valueString);
}

void WCDBJNIHandleStatementObjectMethod(bindBLOB, jlong self, jbyteArray value, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBJNIGetByteArray(value);
    WCDBHandleStatementBindBlob(selfStruct, index, valueArray, valueLength);
    WCDBJNIReleaseByteArray(value);
}

void WCDBJNIHandleStatementObjectMethod(bindNull, jlong self, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBHandleStatementBindNull(selfStruct, index);
}

jint WCDBJNIHandleStatementObjectMethod(bindParameterIndex, jlong self, jstring parameterName)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBJNIGetString(parameterName);
    jint index = WCDBHandleStatementBindParameterIndex(selfStruct, parameterNameString);
    WCDBJNIReleaseString(parameterName);
    return index;
}

jint WCDBJNIHandleStatementObjectMethod(getColumnType, jlong self, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementGetColumnType(selfStruct, index);
}

jlong WCDBJNIHandleStatementObjectMethod(getInteger, jlong self, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementGetInteger(selfStruct, index);
}

jdouble WCDBJNIHandleStatementObjectMethod(getDouble, jlong self, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementGetDouble(selfStruct, index);
}

jstring WCDBJNIHandleStatementObjectMethod(getText, jlong self, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBJNICreateStringAndReturn(WCDBHandleStatementGetText(selfStruct, index));
}

jbyteArray WCDBJNIHandleStatementObjectMethod(getBLOB, jlong self, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    jbyte* buffer = WCDBHandleStatementGetBlob(selfStruct, index);
    jsize size = WCDBHandleStatementGetColumnSize(selfStruct, index);
    if (buffer == NULL || size == 0) {
        return (*env)->NewByteArray(env, 0);
    }
    jbyteArray array = (*env)->NewByteArray(env, size);
    (*env)->SetByteArrayRegion(env, array, 0, size, buffer);
    return array;
}

jint WCDBJNIHandleStatementObjectMethod(getColumnCount, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementGetColumnCount(selfStruct);
}

jstring WCDBJNIHandleStatementObjectMethod(getColumnName, jlong self, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBJNICreateStringAndReturn(WCDBHandleStatementGetColumnName(selfStruct, index));
}

jstring WCDBJNIHandleStatementObjectMethod(getOriginalColumnName, jlong self, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBJNICreateStringAndReturn(WCDBHandleStatementGetOriginalColumnName(selfStruct, index));
}

jstring WCDBJNIHandleStatementObjectMethod(getColumnTableName, jlong self, jint index)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    WCDBJNICreateStringAndReturn(WCDBHandleStatementGetColumnTableName(selfStruct, index));
}

jboolean WCDBJNIHandleStatementObjectMethod(isReadOnly, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandleStatement, self);
    return WCDBHandleStatementIsReadOnly(selfStruct);
}
