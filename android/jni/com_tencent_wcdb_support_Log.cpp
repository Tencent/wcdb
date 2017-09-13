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

#include "JNIHelp.h"
#include "Logger.h"
#include "ModuleLoader.h"

#include <android/log.h>
#include <assert.h>
#include <jni.h>
#include <stdlib.h>

namespace wcdb {

static JavaVM *g_jvm = NULL;
static jobject g_callbackObj = NULL;
static jmethodID g_callbackPrintLnMethod = NULL;

const static wcdb_logfunc_t LOGGERS[] = {
    NULL,                // LOGGER_NONE
    __android_log_write, // LOGGER_DEFAULT
};

static int jniCallbackLogger(int priority, const char *tag, const char *msg)
{
    assert(g_callbackObj && g_callbackPrintLnMethod && g_jvm);

    JNIEnv *env = NULL;
    bool attached = false;
    jint ret = g_jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (ret == JNI_EDETACHED) {
        jint ret = g_jvm->AttachCurrentThread(&env, NULL);
        assert(ret == JNI_OK);
        (void) ret;
        attached = true;
    }

    jstring tagStr = env->NewStringUTF(tag);
    jstring msgStr = env->NewStringUTF(msg);
    env->CallVoidMethod(g_callbackObj, g_callbackPrintLnMethod, priority,
                        tagStr, msgStr);
    env->ExceptionClear();
    env->DeleteLocalRef(tagStr);
    env->DeleteLocalRef(msgStr);

    if (attached)
        g_jvm->DetachCurrentThread();
    return 0;
}

static void
nativeSetLogger(JNIEnv *env, jclass cls, jint logger, jobject callback)
{
    if (g_callbackObj)
        env->DeleteGlobalRef(g_callbackObj);

    if (callback) {
        g_callbackObj = env->NewGlobalRef(callback);
        wcdb_set_log_function(jniCallbackLogger);
    } else {
        if (logger < 0 || logger > NELEM(LOGGERS)) {
            jniThrowException(env, "java/lang/IllegalArgumentException",
                              "Invalid preset logger.");
            return;
        }

        g_callbackObj = NULL;
        wcdb_set_log_function(LOGGERS[logger]);
    }
}

static void nativePrintLn(
    JNIEnv *env, jclass cls, jint priority, jstring tagStr, jstring msgStr)
{
    if (!tagStr || !msgStr)
        return;

    const char *tag = env->GetStringUTFChars(tagStr, NULL);
    const char *msg = env->GetStringUTFChars(msgStr, NULL);
    wcdb_log_write(priority, tag, msg);
    env->ReleaseStringUTFChars(tagStr, tag);
    env->ReleaseStringUTFChars(msgStr, msg);
}

static const JNINativeMethod sMethods[] = {
    {"nativeSetLogger", "(ILcom/tencent/wcdb/support/Log$LogCallback;)V",
     (void *) nativeSetLogger},
    {"nativePrintLn", "(ILjava/lang/String;Ljava/lang/String;)V",
     (void *) nativePrintLn},
};

static int register_wcdb_Log(JavaVM *vm, JNIEnv *env)
{
    g_jvm = vm;

    jclass cls = env->FindClass("com/tencent/wcdb/support/Log$LogCallback");
    if (!cls)
        return -1;

    g_callbackPrintLnMethod = env->GetMethodID(
        cls, "println", "(ILjava/lang/String;Ljava/lang/String;)V");
    if (!g_callbackPrintLnMethod)
        return -1;

    return jniRegisterNativeMethods(env, "com/tencent/wcdb/support/Log",
                                    sMethods, NELEM(sMethods));
}
WCDB_JNI_INIT(Log, register_wcdb_Log)

static int unregister_wcdb_Log(JavaVM *vm, JNIEnv *env)
{
    if (g_callbackObj)
        env->DeleteGlobalRef(g_callbackObj);
    return 0;
}
WCDB_JNI_FINI(Log, unregister_wcdb_Log)
}