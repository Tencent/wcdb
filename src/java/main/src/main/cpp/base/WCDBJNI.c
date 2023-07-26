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

#include "WCDBJNI.h"
#include "ObjectBridge.h"
#include "assert.h"
#include <string.h>

JavaVM *g_vm = NULL;

void WCDBJNIDestructContext(jobject config)
{
    WCDBJNITryGetEnvOr(return );
    (*env)->DeleteGlobalRef(env, config);
    WCDBJNITryDetach;
}

void WCDBJNIClassMethod(Base, releaseObject, long long cppObject)
{
    WCDBReleaseCPPObject((CPPObject *) cppObject);
}

jclass g_databaseClass = NULL;
jclass g_handleClass = NULL;
jclass g_exceptionClass = NULL;

void WCDBJNIInitJClasses(JNIEnv *env)
{
    g_databaseClass = (*env)->FindClass(env, "com/tencent/wcdb/core/Database");
    WCDBJNICreateGlobalRel(g_databaseClass);
    assert(g_databaseClass != NULL);

    g_handleClass = (*env)->FindClass(env, "com/tencent/wcdb/core/Handle");
    WCDBJNICreateGlobalRel(g_handleClass);
    assert(g_handleClass != NULL);

    g_exceptionClass = (*env)->FindClass(env, "com/tencent/wcdb/base/WCDBException");
    WCDBJNICreateGlobalRel(g_exceptionClass);
    assert(g_exceptionClass != NULL);
}

jclass WCDBJNIGetDatabaseClass()
{
    return g_databaseClass;
}

jclass WCDBJNIGetHandleClass()
{
    return g_handleClass;
}

jclass WCDBJNIGetExceptionClass()
{
    return g_exceptionClass;
}

void __WCDBJNIGetStringArray(JNIEnv *env,
                             jobjectArray stringArray,
                             int *length,
                             const char **charArray,
                             char **charBuffer)
{
    int totalLength = 0;
    int *stringLength = alloca(*length * sizeof(int));
    for (int i = 0; i < *length; i++) {
        jstring curString = (jstring) (*env)->GetObjectArrayElement(env, stringArray, i);
        stringLength[i]
        = curString == NULL ? 0 : (int) (*env)->GetStringLength(env, curString) + 1;
        totalLength += stringLength[i];
        (*env)->DeleteLocalRef(env, curString);
    }
    if (*length > 0) {
        *charBuffer = malloc(totalLength * sizeof(char));
        if (*charBuffer == NULL) {
            *length = 0;
        } else {
            char *curBuffer = *charBuffer;
            for (int i = 0; i < *length; i++) {
                jstring curString
                = (jstring) (*env)->GetObjectArrayElement(env, stringArray, i);
                const char *curChar
                = (const char *) (*env)->GetStringUTFChars(env, curString, NULL);
                memcpy(curBuffer, curChar, stringLength[i]);
                charArray[i] = curBuffer;
                curBuffer += stringLength[i];
                (*env)->ReleaseStringUTFChars(env, curString, curChar);
                (*env)->DeleteLocalRef(env, curString);
            }
        }
    }
}