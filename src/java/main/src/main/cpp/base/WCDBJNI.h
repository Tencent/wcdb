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

#define WCDBJNIGetObjectArray(value)                                           \
    void **value##ObjectArray = NULL;                                          \
    int value##Length = (*env)->GetArrayLength(env, value);                    \
    jlong *value##LongArray = (*env)->GetLongArrayElements(env, value, NULL);  \
    if (sizeof(void *) == sizeof(jlong)) {                                     \
        value##ObjectArray = (void **) value##LongArray;                       \
    } else {                                                                   \
        value##ObjectArray = alloca(sizeof(void *) * value##Length);           \
        for (int i = 0; i < value##Length; i++) {                              \
            value##ObjectArray[i] = (void *) value##LongArray[i];              \
        }                                                                      \
    }

#define WCDBJNIReleaseObjectArray(value)                                             \
    if (value##LongArray != NULL) {                                                  \
        (*env)->ReleaseLongArrayElements(env, value, (jlong *) value##LongArray, 0); \
    }

#define WCDBJNIGetString(value)                                                \
    const char *value##String = NULL;                                          \
    if (value != NULL) {                                                       \
        value##String = (*env)->GetStringUTFChars(env, value, 0);              \
    }

#define WCDBJNIReleaseString(value)                                            \
    if (value##String != NULL) {                                               \
        (*env)->ReleaseStringUTFChars(env, value, value##String);              \
    }

#define WCDBJNIGetByteArray(value)                                             \
    const jbyte *value##Array = NULL;                                          \
    int value##Length = 0;                                                     \
    if (value != NULL) {                                                       \
        value##Array = (*env)->GetByteArrayElements(env, value, NULL);         \
        value##Length = (*env)->GetArrayLength(env, value);                    \
    }

#define WCDBJNIReleaseByteArray(value)                                           \
    if (value##Array != NULL) {                                                  \
        (*env)->ReleaseByteArrayElements(env, value, (jbyte *) value##Array, 0); \
    }

#define WCDBJNIGetLongArray(value)                                             \
    const jlong *value##Array = NULL;                                          \
    int value##Length = 0;                                                     \
    if (value != NULL) {                                                       \
        value##Array = (*env)->GetLongArrayElements(env, value, NULL);         \
        value##Length = (*env)->GetArrayLength(env, value);                    \
    }

#define WCDBJNIReleaseLongArray(value)                                           \
    if (value##Array != NULL) {                                                  \
        (*env)->ReleaseLongArrayElements(env, value, (jlong *) value##Array, 0); \
    }

#define WCDBJNIGetIntArray(value)                                              \
    const jint *value##Array = NULL;                                           \
    int value##Length = 0;                                                     \
    if (value != NULL) {                                                       \
        value##Array = (*env)->GetIntArrayElements(env, value, NULL);          \
        value##Length = (*env)->GetArrayLength(env, value);                    \
    }

#define WCDBJNIReleaseIntArray(value)                                          \
    if (value##Array != NULL) {                                                \
        (*env)->ReleaseIntArrayElements(env, value, (jint *) value##Array, 0); \
    }

#define WCDBJNIGetDoubleArray(value)                                           \
    const jdouble *value##Array = NULL;                                        \
    int value##Length = 0;                                                     \
    if (value != NULL) {                                                       \
        value##Array = (*env)->GetDoubleArrayElements(env, value, NULL);       \
        value##Length = (*env)->GetArrayLength(env, value);                    \
    }

#define WCDBJNIReleaseDoubleArray(value)                                             \
    if (value##Array != NULL) {                                                      \
        (*env)->ReleaseDoubleArrayElements(env, value, (jdouble *) value##Array, 0); \
    }

#define WCDBJNIGetStringArray(value)                                                    \
    int value##Length = 0;                                                              \
    jstring *value##JString = NULL;                                                     \
    const char **value##CharArray = NULL;                                               \
    char *value##StringBuffer = NULL;                                                   \
    if (value != NULL) {                                                                \
        value##Length = (*env)->GetArrayLength(env, value);                             \
        if (value##Length > 0) {                                                        \
            value##CharArray = alloca(value##Length * sizeof(const char *));            \
            if (value##Length <= 500) {                                                 \
                value##JString = alloca(value##Length * sizeof(jstring));               \
                for (int i = 0; i < value##Length; i++) {                               \
                    value##JString[i]                                                   \
                    = (jstring) (*env)->GetObjectArrayElement(env, value, i);           \
                    if (value##JString[i] != NULL) {                                    \
                        value##CharArray[i] = (const char *) (*env)->GetStringUTFChars( \
                        env, value##JString[i], NULL);                                  \
                    } else {                                                            \
                        value##CharArray[i] = NULL;                                     \
                    }                                                                   \
                }                                                                       \
            } else {                                                                    \
                __WCDBJNIGetStringArray(                                                \
                env, value, &value##Length, value##CharArray, &value##StringBuffer);    \
            }                                                                           \
        }                                                                               \
    }

#define WCDBJNIReleaseStringArray(value)                                       \
    if (value##Length <= 500) {                                                \
        for (int i = 0; i < value##Length; i++) {                              \
            if (value##CharArray[i] != NULL) {                                 \
                (*env)->ReleaseStringUTFChars(                                 \
                env, value##JString[i], value##CharArray[i]);                  \
                if (value##JString[i] != NULL) {                               \
                    (*env)->DeleteLocalRef(env, value##JString[i]);            \
                }                                                              \
            }                                                                  \
        }                                                                      \
    } else if (value##StringBuffer != NULL) {                                  \
        free(value##StringBuffer);                                             \
    }

#define WCDBJNICommonValueParameter(parameter)                                 \
    jint parameter##_type, jlong parameter##_long, jdouble parameter##_double, \
    jstring parameter##_string

#define WCDBJNICreateCommonValue(parameter)                                    \
    CPPCommonValue parameter##_common;                                         \
    parameter##_common.type = parameter##_type;                                \
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
        parameter##_common.intValue                                            \
        = (long long) (*env)->GetStringUTFChars(env, parameter##_string, 0);   \
        break;                                                                 \
    default:                                                                   \
        parameter##_common.intValue = parameter##_long;                        \
        break;                                                                 \
    }

#define WCDBJNITryReleaseStringInCommonValue(parameter)                                   \
    if (parameter##_type == WCDBBridgedType_String && parameter##_common.intValue != 0) { \
        (*env)->ReleaseStringUTFChars(                                                    \
        env, parameter##_string, (const char *) parameter##_common.intValue);             \
    }

#define WCDBJNIObjectOrStringParameter(parameter)                              \
    jint parameter##_type, jlong parameter##_long, jstring parameter##_string

#define WCDBJNICreateObjectOrStringCommonValue(parameter)                      \
    CPPCommonValue parameter##_common;                                         \
    parameter##_common.type = parameter##_type;                                \
    if (parameter##_type == WCDBBridgedType_String) {                          \
        parameter##_common.intValue                                            \
        = (long long) (*env)->GetStringUTFChars(env, parameter##_string, 0);   \
    } else {                                                                   \
        parameter##_common.intValue = parameter##_long;                        \
    }

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

#define WCDBJNICreateStringAndReturn(action)                                   \
    const char *textValue = action;                                            \
    if (textValue == NULL) {                                                   \
        return NULL;                                                           \
    }                                                                          \
    return (*env)->NewStringUTF(env, textValue);

#define WCDBJNICreateJavaString(value)                                         \
    jstring j##value = NULL;                                                   \
    if (value != NULL) {                                                       \
        j##value = (*env)->NewStringUTF(env, value);                           \
    }

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

void WCDBJNIClassMethod(Base, releaseObject, long long cppObject);

void __WCDBJNIGetStringArray(JNIEnv *env,
                             jobjectArray stringArray,
                             int *length,
                             const char **charArray,
                             char **charBuffer);