// Created by qiuwenchen on 2023/3/30.
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

#pragma once

#include "ObjectBridge.h"
#include <jni.h>
#include <stdlib.h>

#define WCDBJNIStringSignature "Ljava/lang/String;"

#define WCDBJNI(className, funcName) WCDBJNI##className##_##funcName

#define WCDBJNIObjectMethodWithNoArg(className, funcName)                      \
    WCDBJNI(className, funcName)(JNIEnv * env, jobject object)

#define WCDBJNIObjectMethod(className, funcName, ...)                          \
    WCDBJNI(className, funcName)(JNIEnv * env, jobject obj, __VA_ARGS__)

#define WCDBJNIClassMethodWithNoArg(className, funcName)                       \
    WCDBJNI(className, funcName)(JNIEnv * env, jclass classType)

#define WCDBJNIClassMethod(className, funcName, ...)                           \
    WCDBJNI(className, funcName)(JNIEnv * env, jclass classType, __VA_ARGS__)

#define WCDBJNIBridgeStruct(type, value)                                       \
    type value##Struct = { (CPPObject *) value }

#define WCDBJNIGetString(value)                                                \
    char *value##String = NULL;                                                \
    const jchar *value##_utf16String = NULL;                                   \
    WCDBJNIGetUTF8String(env, value, &value##String, &value##_utf16String, false);

#define WCDBJNIReleaseString(value)                                            \
    if (value##_utf16String != NULL) {                                         \
        (*env)->ReleaseStringChars(env, value, value##_utf16String);           \
    }                                                                          \
    WCDBClearAllPreAllocatedMemory();

#define WCDBJNIGetStringCritical(value)                                        \
    char *value##String = NULL;                                                \
    const jchar *value##_utf16String = NULL;                                   \
    WCDBJNIGetUTF8String(env, value, &value##String, &value##_utf16String, true);

#define WCDBJNIReleaseStringCritical(value)                                    \
    if (value##_utf16String != NULL) {                                         \
        (*env)->ReleaseStringCritical(env, value, value##_utf16String);        \
    }                                                                          \
    WCDBClearAllPreAllocatedMemory();

#define WCDBJNIGetByteArrayCritical(value)                                             \
    const unsigned char *value##Array = NULL;                                          \
    int value##Length = 0;                                                             \
    if (value != NULL) {                                                               \
        value##Length = (*env)->GetArrayLength(env, value);                            \
        value##Array                                                                   \
        = (const unsigned char *) (*env)->GetPrimitiveArrayCritical(env, value, NULL); \
    }

#define WCDBJNIReleaseByteArrayCritical(value)                                        \
    if (value##Array != NULL) {                                                       \
        (*env)->ReleasePrimitiveArrayCritical(env, value, (jbyte *) value##Array, 0); \
    }

#define WCDBJNIGetLongArray(value)                                             \
    const jlong *value##Array = NULL;                                          \
    int value##Length = 0;                                                     \
    if (value != NULL) {                                                       \
        value##Array = (*env)->GetLongArrayElements(env, value, NULL);         \
        value##Length = (*env)->GetArrayLength(env, value);                    \
    }

#define WCDBJNIReleaseLongArray(value)                                                   \
    if (value##Array != NULL) {                                                          \
        (*env)->ReleaseLongArrayElements(env, value, (jlong *) value##Array, JNI_ABORT); \
    }

#define WCDBJNIGetCppPointerArray(value)                                       \
    const void **value##Array = NULL;                                          \
    const jlong *value##LongArray = NULL;                                      \
    int value##Length = 0;                                                     \
    if (value != NULL) {                                                       \
        value##LongArray = (*env)->GetLongArrayElements(env, value, NULL);     \
        value##Length = (*env)->GetArrayLength(env, value);                    \
        if (sizeof(void *) == sizeof(jlong) || value##Length == 0) {           \
            value##Array = value##LongArray;                                   \
        } else {                                                               \
            value##Array = alloca(sizeof(void *) * value##Length);             \
            for (int i = 0; i < value##Length; i++) {                          \
                value##Array[i] = (void *) value##LongArray[i];                \
            }                                                                  \
        }                                                                      \
    }

#define WCDBJNIReleaseCppPointerArray(value)                                                 \
    if (value##LongArray != NULL) {                                                          \
        (*env)->ReleaseLongArrayElements(env, value, (jlong *) value##LongArray, JNI_ABORT); \
    }

#define WCDBJNIGetIntArray(value)                                              \
    const jint *value##Array = NULL;                                           \
    int value##Length = 0;                                                     \
    if (value != NULL) {                                                       \
        value##Array = (*env)->GetIntArrayElements(env, value, NULL);          \
        value##Length = (*env)->GetArrayLength(env, value);                    \
    }

#define WCDBJNIReleaseIntArray(value)                                                  \
    if (value##Array != NULL) {                                                        \
        (*env)->ReleaseIntArrayElements(env, value, (jint *) value##Array, JNI_ABORT); \
    }

#define WCDBJNIGetDoubleArray(value)                                           \
    const jdouble *value##Array = NULL;                                        \
    int value##Length = 0;                                                     \
    if (value != NULL) {                                                       \
        value##Array = (*env)->GetDoubleArrayElements(env, value, NULL);       \
        value##Length = (*env)->GetArrayLength(env, value);                    \
    }

#define WCDBJNIReleaseDoubleArray(value)                                                     \
    if (value##Array != NULL) {                                                              \
        (*env)->ReleaseDoubleArrayElements(env, value, (jdouble *) value##Array, JNI_ABORT); \
    }

#define WCDBJNIGetStringArray(value)                                           \
    int value##Length = 0;                                                     \
    char **value##CharArray = NULL;                                            \
    WCDBJNIGetUTF8StringArray(env, value, &value##CharArray, &value##Length);

#define WCDBJNIReleaseStringArray(value) WCDBClearAllPreAllocatedMemory();

#define WCDBJNICommonValueParameter(parameter)                                 \
    jint parameter##_type, jlong parameter##_long, jdouble parameter##_double, \
    jstring parameter##_string

#define WCDBJNICreateCommonValue(parameter)                                    \
    CPPCommonValue parameter##_common;                                         \
    parameter##_common.type = parameter##_type;                                \
    const jchar *parameter##_utf16String = NULL;                               \
    switch (parameter##_type) {                                                \
    case WCDBBridgedType_Bool:                                                 \
    case WCDBBridgedType_UInt:                                                 \
    case WCDBBridgedType_Int:                                                  \
        parameter##_common.intValue = parameter##_long;                        \
        break;                                                                 \
    case WCDBBridgedType_Double:                                               \
        parameter##_common.doubleValue = parameter##_double;                   \
        break;                                                                 \
    case WCDBBridgedType_String:                                               \
        WCDBJNIGetUTF8String(env,                                              \
                             parameter##_string,                               \
                             (char **) &parameter##_common.intValue,           \
                             &parameter##_utf16String,                         \
                             false);                                           \
        break;                                                                 \
    default:                                                                   \
        parameter##_common.intValue = parameter##_long;                        \
        break;                                                                 \
    }

#define WCDBJNITryReleaseStringInCommonValue(parameter)                               \
    if (parameter##_type == WCDBBridgedType_String                                    \
        && parameter##_common.intValue != 0 && parameter##_utf16String != NULL) {     \
        (*env)->ReleaseStringChars(env, parameter##_string, parameter##_utf16String); \
    }

#define WCDBJNIObjectOrStringParameter(parameter)                              \
    jint parameter##_type, jlong parameter##_long, jstring parameter##_string

#define WCDBJNICreateObjectOrStringCommonValue(parameter)                      \
    CPPCommonValue parameter##_common;                                         \
    parameter##_common.type = parameter##_type;                                \
    const jchar *parameter##_utf16String = NULL;                               \
    if (parameter##_type == WCDBBridgedType_String) {                          \
        WCDBJNIGetUTF8String(env,                                              \
                             parameter##_string,                               \
                             (char **) &parameter##_common.intValue,           \
                             &parameter##_utf16String,                         \
                             false);                                           \
    } else {                                                                   \
        parameter##_common.intValue = parameter##_long;                        \
    }

#define WCDBJNIObjectOrIntegerParameter(parameter)                             \
    jint parameter##_type, jlong parameter##_long

#define WCDBJNICreateObjectOrIntegerCommonValue(parameter)                     \
    CPPCommonValue parameter##_common;                                         \
    parameter##_common.type = parameter##_type;                                \
    parameter##_common.intValue = parameter##_long;

#define WCDBJNICommonArrayParameter(parameter)                                 \
    jint parameter##_type, jlongArray parameter##_longArray,                   \
    jdoubleArray parameter##_doubleArray, jobjectArray parameter##_stringArray

#define WCDBJNICreateCommonArrayWithAction(parameter, action)                                     \
    CPPCommonArray parameter##_commonArray;                                                       \
    parameter##_commonArray.type = parameter##_type;                                              \
    if (parameter##_type < WCDBBridgedType_Double || parameter##_type > WCDBBridgedType_String) { \
        WCDBJNIGetLongArray(parameter##_longArray);                                               \
        parameter##_commonArray.length = parameter##_longArrayLength;                             \
        parameter##_commonArray.buffer = (const void **) parameter##_longArrayArray;              \
        action;                                                                                   \
        WCDBJNIReleaseLongArray(parameter##_longArray);                                           \
    } else if (parameter##_type == WCDBBridgedType_String) {                                      \
        WCDBJNIGetStringArray(parameter##_stringArray);                                           \
        parameter##_commonArray.length = parameter##_stringArrayLength;                           \
        parameter##_commonArray.buffer = (const void **) parameter##_stringArrayCharArray;        \
        action;                                                                                   \
        WCDBJNIReleaseStringArray(parameter##_stringArray);                                       \
    } else {                                                                                      \
        WCDBJNIGetDoubleArray(parameter##_doubleArray);                                           \
        parameter##_commonArray.length = parameter##_doubleArrayLength;                           \
        parameter##_commonArray.buffer = (const void **) parameter##_doubleArrayArray;            \
        action;                                                                                   \
        WCDBJNIReleaseDoubleArray(parameter##_doubleArray);                                       \
    }

#define WCDBJNIObjectOrStringArrayParameter(parameter)                         \
    jint parameter##_type, jlongArray parameter##_longArray, jobjectArray parameter##_stringArray

#define WCDBJNICreateObjectOrStringArrayWithAction(parameter, action)                             \
    CPPCommonArray parameter##_commonArray;                                                       \
    parameter##_commonArray.type = parameter##_type;                                              \
    if (parameter##_type < WCDBBridgedType_Double || parameter##_type > WCDBBridgedType_String) { \
        WCDBJNIGetLongArray(parameter##_longArray);                                               \
        parameter##_commonArray.length = parameter##_longArrayLength;                             \
        parameter##_commonArray.buffer = (const void **) parameter##_longArrayArray;              \
        action;                                                                                   \
        WCDBJNIReleaseLongArray(parameter##_longArray);                                           \
    } else if (parameter##_type == WCDBBridgedType_String) {                                      \
        WCDBJNIGetStringArray(parameter##_stringArray);                                           \
        parameter##_commonArray.length = parameter##_stringArrayLength;                           \
        parameter##_commonArray.buffer = (const void **) parameter##_stringArrayCharArray;        \
        action;                                                                                   \
        WCDBJNIReleaseStringArray(parameter##_stringArray);                                       \
    }

#define WCDBJNIMultiTypeArrayParameter(parameter)                              \
    jintArray parameter##_types, jlongArray parameter##_longValues,            \
    jdoubleArray parameter##_doubleValues, jobjectArray parameter##_stringValues

#define WCDBJNICreateMultiTypeArray(parameter)                                      \
    WCDBJNIGetIntArray(parameter##_types);                                          \
    WCDBJNIGetLongArray(parameter##_longValues);                                    \
    WCDBJNIGetDoubleArray(parameter##_doubleValues);                                \
    WCDBJNIGetStringArray(parameter##_stringValues);                                \
    CPPMultiTypeArray parameter##Array;                                             \
    parameter##Array.totalLength = parameter##_typesLength;                         \
    parameter##Array.types = (const enum WCDBBridgedType *) parameter##_typesArray; \
    parameter##Array.intValues = (const long long *) parameter##_longValuesArray;   \
    parameter##Array.doubleValues = (const double *) parameter##_doubleValuesArray; \
    parameter##Array.stringValues = (const char **) parameter##_stringValuesCharArray;

#define WCDBJNIReleaseMultiTypeArray(parameter)                                \
    WCDBJNIReleaseIntArray(parameter##_types);                                 \
    WCDBJNIReleaseLongArray(parameter##_longValues);                           \
    WCDBJNIReleaseDoubleArray(parameter##_doubleValues);                       \
    WCDBJNIReleaseStringArray(parameter##_stringValues);

#define WCDBJNICreateJStringAndReturn(action)                                  \
    return WCDBJNICreateJString(env, action)

#define WCDBJNICreateJavaString(value)                                         \
    jstring j##value = WCDBJNICreateJString(env, value)

#define WCDBJNIFindClass(valueName, signature, action)                         \
    static jclass valueName = NULL;                                            \
    if (valueName == NULL) {                                                   \
        valueName = (*env)->FindClass(env, signature);                         \
        WCDBJNICreateGlobalRel(valueName);                                     \
    }                                                                          \
    assert(valueName != NULL);                                                 \
    if (valueName == NULL) {                                                   \
        action;                                                                \
    }

#define WCDBJNIGetObjectMethodId(valueName, class, methodName, signature)      \
    static jmethodID valueName = NULL;                                         \
    if (valueName == NULL) {                                                   \
        valueName = (*env)->GetMethodID(env, class, methodName, signature);    \
    }                                                                          \
    assert(valueName != NULL);

#define WCDBJNICreateGlobalRel(value)                                          \
    if (value != NULL) {                                                       \
        value = (*env)->NewGlobalRef(env, value);                              \
    }

extern JavaVM *g_vm;

#define WCDBJNITryGetVM                                                        \
    if (g_vm == NULL) {                                                        \
        (*env)->GetJavaVM(env, &g_vm);                                         \
        assert(g_vm != NULL);                                                  \
    }

#define WCDBJNITryGetEnvOr(action)                                             \
    JNIEnv *env;                                                               \
    int getEnvStat = (*g_vm)->GetEnv(g_vm, (void **) &env, JNI_VERSION_1_6);   \
    bool needDetach = false;                                                   \
    if (getEnvStat == JNI_EDETACHED) {                                         \
        if ((*g_vm)->AttachCurrentThread(g_vm, &env, NULL) != 0) {             \
            assert(0);                                                         \
            action;                                                            \
        }                                                                      \
        needDetach = JNI_TRUE;                                                 \
    }

#define WCDBJNITryDetach                                                       \
    if (needDetach) {                                                          \
        (*g_vm)->DetachCurrentThread(g_vm);                                    \
    }

void WCDBJNIDestructContext(jobject config);

void WCDBJNIClassMethod(Base, releaseObject, long long cppObject);

void WCDBJNIInitJClasses(JNIEnv *env);

jclass WCDBJNIGetDatabaseClass();
jclass WCDBJNIGetHandleClass();
jclass WCDBJNIGetExceptionClass();

void WCDBJNIGetUTF8String(
JNIEnv *env, jstring value, char **utf8String, const jchar **utf16String, bool critical);
void WCDBJNIGetUTF8StringArray(JNIEnv *env, jobjectArray value, char ***stringArray, int *length);
jstring WCDBJNICreateJString(JNIEnv *env, const char *utf8String);