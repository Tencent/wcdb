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

#include "Macro.h"
#include "ObjectBridge.h"
#include <stdlib.h>

#define WCDBRust(className, funcName) WCDBRust##className##_##funcName

#define WCDBRustObjectMethodWithNoArg(className, funcName)                     \
    WCDBRust(className, funcName)()

#define WCDBRustObjectMethod(className, funcName, ...)                         \
    WCDBRust(className, funcName)(__VA_ARGS__)

#define WCDBRustClassMethodWithNoArg(className, funcName)                      \
    WCDBRust(className, funcName)()

#define WCDBRustClassMethod(className, funcName, ...)                          \
    WCDBRust(className, funcName)(__VA_ARGS__)

#define WCDBRustBridgeStruct(type, value)                                      \
    type value##Struct = { (CPPObject *) value }

#define WCDBRustGetString(value)                                               \
    char *value##String = NULL;                                                \
    const jchar *value##_utf16String = NULL;                                   \
    WCDBRustGetUTF8String(env, value, &value##String, &value##_utf16String, false);

#define WCDBRustReleaseString(value)                                           \
    if (value##_utf16String != NULL) {                                         \
        (*env)->ReleaseStringChars(env, value, value##_utf16String);           \
    }                                                                          \
    WCDBClearAllPreAllocatedMemory();

#define WCDBRustGetStringCritical(value)                                       \
    char *value##String = NULL;                                                \
    const jchar *value##_utf16String = NULL;                                   \
    WCDBRustGetUTF8String(env, value, &value##String, &value##_utf16String, true);

#define WCDBRustReleaseStringCritical(value)                                   \
    if (value##_utf16String != NULL) {                                         \
        (*env)->ReleaseStringCritical(env, value, value##_utf16String);        \
    }                                                                          \
    WCDBClearAllPreAllocatedMemory();

#define WCDBRustGetByteArray(value)                                               \
    const unsigned char *value##Array = NULL;                                     \
    int value##Length = 0;                                                        \
    if (value != NULL) {                                                          \
        value##Length = (*env)->GetArrayLength(env, value);                       \
        value##Array                                                              \
        = (const unsigned char *) (*env)->GetByteArrayElements(env, value, NULL); \
    }

#define WCDBRustReleaseByteArray(value)                                          \
    if (value##Array != NULL) {                                                  \
        (*env)->ReleaseByteArrayElements(env, value, (jbyte *) value##Array, 0); \
    }

#define WCDBRustGetByteArrayCritical(value)                                            \
    const unsigned char *value##Array = NULL;                                          \
    int value##Length = 0;                                                             \
    if (value != NULL) {                                                               \
        value##Length = (*env)->GetArrayLength(env, value);                            \
        value##Array                                                                   \
        = (const unsigned char *) (*env)->GetPrimitiveArrayCritical(env, value, NULL); \
    }

#define WCDBRustReleaseByteArrayCritical(value)                                       \
    if (value##Array != NULL) {                                                       \
        (*env)->ReleasePrimitiveArrayCritical(env, value, (jbyte *) value##Array, 0); \
    }

#define WCDBRustGetLongArray(value)                                            \
    const jlong *value##Array = NULL;                                          \
    int value##Length = 0;                                                     \
    if (value != NULL) {                                                       \
        value##Array = (*env)->GetLongArrayElements(env, value, NULL);         \
        value##Length = (*env)->GetArrayLength(env, value);                    \
    }

#define WCDBRustReleaseLongArray(value)                                                   \
    if (value##Array != NULL) {                                                           \
        (*env)->ReleaseLongArrayElements(env, value, (jlong *) value##Array, Rust_ABORT); \
    }

#define WCDBRustGetCppPointerArrayCritical(value)                               \
    const void **value##Array = NULL;                                          \
    const jlong *value##LongArray = NULL;                                      \
    int value##Length = 0;                                                     \
    if (value != NULL) {                                                       \
        value##Length = (*env)->GetArrayLength(env, value);                    \
        value##LongArray                                                       \
        = (const jlong *) (*env)->GetPrimitiveArrayCritical(env, value, NULL); \
        if (sizeof(void *) == sizeof(jlong) || value##Length == 0) {           \
            value##Array = (const void **) value##LongArray;                   \
        } else {                                                               \
            value##Array = alloca(sizeof(void *) * value##Length);             \
            for (int i = 0; i < value##Length; i++) {                          \
                value##Array[i] = (void *) value##LongArray[i];                \
            }                                                                  \
        }                                                                      \
    }

#define WCDBRustReleaseCppPointerArrayCritical(value)                                    \
    if (value##LongArray != NULL) {                                                      \
        (*env)->ReleasePrimitiveArrayCritical(env, value, (void *) value##LongArray, 0); \
    }

#define WCDBRustGetIntArray(value)                                             \
    const int *value##Array = NULL;                                           \
    int value##Length = 0;                                                     \
    if (value != NULL) {                                                       \
        value##Array = (*env)->GetIntArrayElements(env, value, NULL);          \
        value##Length = (*env)->GetArrayLength(env, value);                    \
    }

#define WCDBRustReleaseIntArray(value)                                                  \
    if (value##Array != NULL) {                                                         \
        (*env)->ReleaseIntArrayElements(env, value, (jint *) value##Array, Rust_ABORT); \
    }

#define WCDBRustGetDoubleArray(value)                                           \
    const jdouble *value##Array = NULL;                                        \
    int value##Length = 0;                                                     \
    if (value != NULL) {                                                       \
        value##Array = (*env)->GetDoubleArrayElements(env, value, NULL);       \
        value##Length = (*env)->GetArrayLength(env, value);                    \
    }

#define WCDBRustReleaseDoubleArray(value)                                                     \
    if (value##Array != NULL) {                                                               \
        (*env)->ReleaseDoubleArrayElements(env, value, (jdouble *) value##Array, Rust_ABORT); \
    }

#define WCDBRustGetStringArray(value)                                          \
    int value##Length = 0;                                                     \
    char **value##CharArray = NULL;                                            \
    WCDBRustGetUTF8StringArray(env, value, &value##CharArray, &value##Length);

#define WCDBRustReleaseStringArray(value) WCDBClearAllPreAllocatedMemory();

#define WCDBRustCommonValueParameter(parameter)                                \
    int parameter##_type, long long parameter##_long,                          \
    double parameter##_double, const char* parameter##_string

#define WCDBRustCreateCommonValue(parameter)                                   \
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
        parameter##_common.intValue = (long long) parameter##_string;          \
        break;                                                                 \
    default:                                                                   \
        parameter##_common.intValue = parameter##_long;                        \
        break;                                                                 \
    }

#define WCDBRustObjectOrStringParameter(parameter)                             \
    int parameter##_type, void* parameter##_object, const char* parameter##_string

#define WCDBRustCreateObjectOrStringCommonValue(parameter, isCritical)         \
    CPPCommonValue parameter##_common;                                         \
    parameter##_common.type = parameter##_type;                                \
    if (parameter##_type == WCDBBridgedType_String) {                          \
        parameter##_common.intValue = (long long) parameter##_string;          \
    } else {                                                                   \
        parameter##_common.intValue = (long long) parameter##_object;          \
    }

#define WCDBRustObjectOrIntegerParameter(parameter)                            \
    jint parameter##_type, jlong parameter##_long

#define WCDBRustCreateObjectOrIntegerCommonValue(parameter)                    \
    CPPCommonValue parameter##_common;                                         \
    parameter##_common.type = parameter##_type;                                \
    parameter##_common.intValue = parameter##_long;

#define WCDBRustCommonArrayParameter(parameter)                                \
    jint parameter##_type, jlongArray parameter##_longArray,                   \
    jdoubleArray parameter##_doubleArray, jobjectArray parameter##_stringArray

#define WCDBRustCreateCommonArrayWithAction(parameter, action)                                    \
    CPPCommonArray parameter##_commonArray;                                                       \
    parameter##_commonArray.type = parameter##_type;                                              \
    if (parameter##_type < WCDBBridgedType_Double || parameter##_type > WCDBBridgedType_String) { \
        WCDBRustGetLongArray(parameter##_longArray);                                              \
        parameter##_commonArray.length = parameter##_longArrayLength;                             \
        parameter##_commonArray.buffer = (const void **) parameter##_longArrayArray;              \
        action;                                                                                   \
        WCDBRustReleaseLongArray(parameter##_longArray);                                          \
    } else if (parameter##_type == WCDBBridgedType_String) {                                      \
        WCDBRustGetStringArray(parameter##_stringArray);                                          \
        parameter##_commonArray.length = parameter##_stringArrayLength;                           \
        parameter##_commonArray.buffer = (const void **) parameter##_stringArrayCharArray;        \
        action;                                                                                   \
        WCDBRustReleaseStringArray(parameter##_stringArray);                                      \
    } else {                                                                                      \
        WCDBRustGetDoubleArray(parameter##_doubleArray);                                          \
        parameter##_commonArray.length = parameter##_doubleArrayLength;                           \
        parameter##_commonArray.buffer = (const void **) parameter##_doubleArrayArray;            \
        action;                                                                                   \
        WCDBRustReleaseDoubleArray(parameter##_doubleArray);                                      \
    }

#define WCDBRustObjectOrStringArrayParameter(parameter)                                           \
    int parameter##_type, void** parameter##_voidArray,                                           \
    const char** parameter##_stringArray,                                                         \
    int parameter##_arrayLen

#define WCDBRustCreateObjectOrStringArrayCriticalWithAction(parameter, action)                    \
    CPPCommonArray parameter##_commonArray;                                                       \
    parameter##_commonArray.type = parameter##_type;                                              \
    if (parameter##_type < WCDBBridgedType_Double || parameter##_type > WCDBBridgedType_String) { \
        parameter##_commonArray.length = parameter##_arrayLen;                                    \
        parameter##_commonArray.buffer = (const void **) parameter##_voidArray;                   \
        action;                                                                                   \
    } else if (parameter##_type == WCDBBridgedType_String) {                                      \
        parameter##_commonArray.length = parameter##_arrayLen;                                    \
        parameter##_commonArray.buffer = (const void **) parameter##_stringArray;                 \
        action;                                                                                   \
    }

#define WCDBRustMultiTypeArrayParameter(parameter)                              \
    int* parameter##_types, long* parameter##_longValues,                       \
    double* parameter##_doubleValues, void** parameter##_stringValues,          \
    int parameter##_arrayLen

#define WCDBRustCreateMultiTypeArray(parameter)                                \
    CPPMultiTypeArray parameter##Array;                                        \
    parameter##Array.totalLength = parameter##_arrayLen;                       \
    parameter##Array.types = (const enum WCDBBridgedType *) parameter##_types; \
    parameter##Array.intValues = (const long long *) parameter##_longValues;   \
    parameter##Array.doubleValues = (const double *) parameter##_doubleValues; \
    parameter##Array.stringValues = (const char **) parameter##_stringValues;

#define WCDBRustReleaseMultiTypeArray(parameter)                               \
    WCDBRustReleaseIntArray(parameter##_types);                                \
    WCDBRustReleaseLongArray(parameter##_longValues);                          \
    WCDBRustReleaseDoubleArray(parameter##_doubleValues);                      \
    WCDBRustReleaseStringArray(parameter##_stringValues);

#define WCDBRustCreateJStringAndReturn(action)                                 \
    return WCDBRustCreateJString(env, action)

#define WCDBRustCreateJavaString(value)                                        \
    jstring j##value = WCDBRustCreateJString(env, value)

#define WCDBRustFindClass(valueName, signature, action)                        \
    static jclass valueName = NULL;                                            \
    if (valueName == NULL) {                                                   \
        valueName = (*env)->FindClass(env, signature);                         \
        WCDBRustCreateGlobalRel(valueName);                                    \
    }                                                                          \
    assert(valueName != NULL);                                                 \
    if (valueName == NULL) {                                                   \
        action;                                                                \
    }

#define WCDBRustGetObjectMethodId(valueName, class, methodName, signature)     \
    static jmethodID valueName = NULL;                                         \
    if (valueName == NULL) {                                                   \
        valueName = (*env)->GetMethodID(env, class, methodName, signature);    \
    }                                                                          \
    assert(valueName != NULL);

#define WCDBRustCreateGlobalRel(value)                                         \
    if (value != NULL) {                                                       \
        value = (*env)->NewGlobalRef(env, value);                              \
    }

//extern JavaVM *g_vm;
//
//#define WCDBRustTryGetVM                                                        \
//    if (g_vm == NULL) {                                                        \
//        (*env)->GetJavaVM(env, &g_vm);                                         \
//        assert(g_vm != NULL);                                                  \
//    }
//
//#define WCDBRustTryGetEnvOr(action)                                             \
//    RustEnv *env;                                                               \
//    int getEnvStat = (*g_vm)->GetEnv(g_vm, (void **) &env, Rust_VERSION_1_6);   \
//    bool needDetach = false;                                                   \
//    if (getEnvStat == Rust_EDETACHED) {                                         \
//        if ((*g_vm)->AttachCurrentThread(g_vm, &env, NULL) != 0) {             \
//            assert(0);                                                         \
//            action;                                                            \
//        }                                                                      \
//        needDetach = Rust_TRUE;                                                 \
//    }
//
//#define WCDBRustTryDetach                                                       \
//    if (needDetach) {                                                          \
//        (*g_vm)->DetachCurrentThread(g_vm);                                    \
//    }
//
WCDB_EXTERN_C_BEGIN
//
//void WCDBRustDestructContext(jobject config);

void WCDBRustClassMethod(Base, releaseObject, void* cppObject);

//void WCDBRustInitJClasses(RustEnv *env);
//
//jclass WCDBRustGetDatabaseClass();
//jclass WCDBRustGetHandleClass();
//jclass WCDBRustGetExceptionClass();
//
//void WCDBRustGetUTF8String(
//RustEnv *env, jstring value, char **utf8String, const jchar **utf16String, bool critical);
//void WCDBRustGetUTF8StringArray(RustEnv *env, jobjectArray value, char ***stringArray, int *length);
//jstring WCDBRustCreateJString(RustEnv *env, const char *utf8String);

WCDB_EXTERN_C_END
