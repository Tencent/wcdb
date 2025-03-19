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

#include "DatabaseRust.h"

#include "CoreBridge.h"
#include "FTSBridge.h"

#include <assert.h>
#include <string.h>

//#define WCDBRustTryGetDatabaseMethodId(name, signature, action)                        \
//    static jmethodID g_methodId = NULL;                                               \
//    if (g_methodId == NULL) {                                                         \
//        g_methodId                                                                    \
//        = (*env)->GetStaticMethodID(env, WCDBRustGetDatabaseClass(), name, signature); \
//        if (g_methodId == NULL) {                                                     \
//            assert(0);                                                                \
//            action;                                                                   \
//        }                                                                             \
//    }

void* WCDBRustDatabaseClassMethod(getError, void* self) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    return (void*)WCDBDatabaseGetError(selfStruct).innerValue;
}

void* WCDBRustDatabaseClassMethod(getTag, void* self) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    return (void*)WCDBDatabaseGetTag(selfStruct);
}

void WCDBRustDatabaseClassMethod(setTag, void* self, long tag) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    WCDBDatabaseSetTag(selfStruct, tag);
}

const char* WCDBRustDatabaseClassMethod(getPath, void* self) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseGetPath(selfStruct);
}

// typedef struct StringEnumeratorContext {
//     JNIEnv* env;
//     jobject array;
//     jclass arrayClass;
// } StringEnumeratorContext;
//
// void WCDBRustStringEnumerator(StringEnumeratorContext* context, const char* string)
//{
//     JNIEnv* env = context->env;
//     WCDBRustGetObjectMethodId(g_addMethod, context->arrayClass, "add", "(Ljava/lang/Object;)Z");
//     if (g_addMethod == NULL) {
//         return;
//     }
//     WCDBRustCreateJavaString(string);
//     (*env)->CallBooleanMethod(env, context->array, g_addMethod, jstring);
// }
//
// jobject WCDBRustDatabaseClassMethod(getPaths, jlong self)
//{
//     WCDBRustBridgeStruct(CPPDatabase, self);
//     WCDBRustFindClass(g_arrayClass, "java/util/ArrayList", return NULL);
//     WCDBRustGetObjectMethodId(g_arrayInit, g_arrayClass, "<init>", "()V");
//     jobject arrayList = (*env)->NewObject(env, g_arrayClass, g_arrayInit);
//     StringEnumeratorContext context;
//     context.env = env;
//     context.array = arrayList;
//     context.arrayClass = g_arrayClass;
//     WCDBDatabaseGetPaths(selfStruct, &context, (WCDBStringEnumerater) WCDBRustStringEnumerator);
//     return arrayList;
// }

void* WCDBRustDatabaseClassMethod(getHandle, void* self, bool writeHint) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    return (void*)WCDBDatabaseGetHandle(selfStruct, writeHint).innerValue;
}

bool WCDBRustDatabaseClassMethod(canOpen, void* self) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseCanOpen(selfStruct);
}

bool WCDBRustDatabaseClassMethod(isOpened, void* self) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseIsOpened(selfStruct);
}

bool WCDBRustDatabaseClassMethod(isBlockaded, void* self) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseIsBlockaded(selfStruct);
}

//
// typedef void (*DatabaseCloseCallback)();
//
// typedef struct CloseDatabaseContext {
//    JNIEnv* env;
//    jobject callback;
//} CloseDatabaseContext;
//
// void WCDBRustDatabaseCloseCallback(CloseDatabaseContext* context)
//{
//    JNIEnv* env = context->env;
//    WCDBRustTryGetDatabaseMethodId(
//    "onClose", "(" WCDBRustDatabaseSignature "$CloseCallBack;)V", return );
//    (*env)->CallStaticVoidMethod(
//    env, WCDBRustGetDatabaseClass(), g_methodId, context->callback);
//}

void WCDBRustDatabase_close(void* self, void* context, WCDBDatabaseCloseCallback callback) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    WCDBDatabaseClose(selfStruct, context, callback);
}

void WCDBRustDatabaseClassMethod(blockade, void* self) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    WCDBDatabaseBlockade(selfStruct);
}

void WCDBRustDatabaseClassMethod(unblockade, void* self) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    WCDBDatabaseUnblockade(selfStruct);
}

//
// void WCDBRustDatabaseClassMethod(purge, jlong self)
//{
//     WCDBRustBridgeStruct(CPPDatabase, self);
//     WCDBDatabasePurge(selfStruct);
// }
//
void WCDBRustDatabaseClassMethod(configCipher,
                                 void* self,
                                 uint8_t* cipherKey,
                                 size_t len,
                                 int pageSize,
                                 int cipherVersion) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    WCDBDatabaseConfigCipher(selfStruct, cipherKey, len, pageSize, cipherVersion);
}

bool WCDBRustDatabaseConfigInvocationCallback(void* invocation, CPPHandle handle) {
    bool ret = false;
    if (invocation == NULL) {
        return ret;
    }
    RustSetConfigCallback callback = (RustSetConfigCallback)invocation;
    ret = callback((void*)handle.innerValue);
    return ret;
}

bool WCDBRustDatabaseConfigUnInvocationCallback(void* unInvocation, CPPHandle handle) {
    bool ret = false;
    if (unInvocation == NULL) {
        return ret;
    }
    RustSetConfigCallback callback = (RustSetConfigCallback)unInvocation;
    ret = callback((void*)handle.innerValue);
    return ret;
}

void WCDBRustSetConfigDestructContext(void* context) {
    if (context != NULL) {
        free(context);
        context = NULL;
    }
}

void WCDBRustDatabaseClassMethod(config,
                                 void* self,
                                 const char* name,
                                 RustSetConfigCallback* invocation,
                                 RustSetConfigCallback* unInvocation,
                                 int priority) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    WCDBDatabaseConfig(
        selfStruct, name, invocation != NULL ? WCDBRustDatabaseConfigInvocationCallback : NULL,
        invocation, unInvocation != NULL ? WCDBRustDatabaseConfigUnInvocationCallback : NULL,
        unInvocation, priority, (WCDBContextDestructor)WCDBRustSetConfigDestructContext);
}

//
// void WCDBRustDatabaseClassMethod(enableLiteMode, jlong self, jboolean enable)
//{
//     WCDBRustBridgeStruct(CPPDatabase, self);
//     WCDBDatabaseEnableLiteMode(selfStruct, enable);
// }
//
// void WCDBRustDatabasePerformanceTrace(jobject tracer,
//                                      long tag,
//                                      const char* path,
//                                      unsigned long long handleId,
//                                      const char* sql,
//                                      const CPPPerformanceInfo* info)
//{
//     WCDBRustTryGetEnvOr(return );
//     WCDBRustTryGetDatabaseMethodId("onTracePerformance",
//                                   "(" WCDBRustDatabaseSignature "$PerformanceTracer;J"
//                                   WCDBRustStringSignature "J" WCDBRustStringSignature "J[I)V",
//                                   return );
//     WCDBRustCreateJavaString(path);
//     WCDBRustCreateJavaString(sql);
//     jint size = sizeof(CPPPerformanceInfo) / sizeof(int) - 2;
//     jintArray infoValues = (*env)->NewIntArray(env, size);
//     if (infoValues != NULL) {
//         (*env)->SetIntArrayRegion(env, infoValues, 0, size, (jint*) info);
//     }
//     (*env)->CallStaticVoidMethod(env,
//                                  WCDBRustGetDatabaseClass(),
//                                  g_methodId,
//                                  tracer,
//                                  (jlong) tag,
//                                  jpath,
//                                  (jlong) handleId,
//                                  jsql,
//                                  (jlong) info->costInNanoseconds,
//                                  infoValues);
//     WCDBRustTryDetach;
// }
//
typedef struct WCDBRustGlobalTracePerformanceContext {
    RustGlobalTracePerformanceCallback rust_callback;
} WCDBRustGlobalTracePerformanceContext;

void WCDBRustDatabasePerformanceTrace(WCDBRustGlobalTracePerformanceContext* context,
                                      long tag,
                                      const char* path,
                                      unsigned long long handleId,
                                      const char* sql,
                                      const CPPPerformanceInfo* info) {
    if (context == NULL || context->rust_callback == NULL) {
        return;
    }
    context->rust_callback(tag, path, handleId, sql, info);
}

void WCDBRustDestructContext(void* context) {
    if (context != NULL) {
        free(context);
        context = NULL;
    }
}

void WCDBRustDatabaseClassMethod(globalTracePerformance,
                                 RustGlobalTracePerformanceCallback rust_callback) {
    size_t size = sizeof(WCDBRustGlobalTracePerformanceContext);
    WCDBRustGlobalTracePerformanceContext* context =
        (WCDBRustGlobalTracePerformanceContext*)WCDBRustCreateGlobalRef(size);
    context->rust_callback = rust_callback;
    WCDBDatabaseGlobalTracePerformance((WCDBPerformanceTracer)WCDBRustDatabasePerformanceTrace,
                                       context, (WCDBContextDestructor)WCDBRustDestructContext);
}
//
// void WCDBRustDatabaseClassMethod(tracePerformance, jlong self, jobject tracer)
//{
//    WCDBRustTryGetVM;
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustCreateGlobalRef(tracer);
//    WCDBDatabaseTracePerformance(
//    selfStruct, tracer != NULL ? WCDBRustDatabasePerformanceTrace : NULL, tracer,
//    WCDBRustDestructContext);
//}
//
// void WCDBRustDatabaseSQLTrace(jobject tracer,
//                             long tag,
//                             const char* path,
//                             unsigned long long handleId,
//                             const char* sql,
//                             const char* info)
//{
//    WCDBRustTryGetEnvOr(return );
//    WCDBRustTryGetDatabaseMethodId("onTraceSQL",
//                                  "(" WCDBRustDatabaseSignature "$SQLTracer;J"
//                                  WCDBRustStringSignature "J" WCDBRustStringSignature
//                                  WCDBRustStringSignature ")V", return );
//    WCDBRustCreateJavaString(path);
//    WCDBRustCreateJavaString(sql);
//    WCDBRustCreateJavaString(info);
//    (*env)->CallStaticVoidMethod(
//    env, WCDBRustGetDatabaseClass(), g_methodId, tracer, (jlong) tag, jpath, (jlong) handleId,
//    jsql, jinfo); WCDBRustTryDetach;
//}
//

typedef struct WCDBRustGlobalTraceSQLContext {
    RustGlobalTraceSQLCallback rust_callback;
} WCDBRustGlobalTraceSQLContext;

void WCDBRustDatabaseGlobalSQLTrace(WCDBRustGlobalTraceSQLContext* context,
                                    long tag,
                                    const char* path,
                                    unsigned long long handleId,
                                    const char* sql,
                                    const char* info) {
    if (context == NULL || context->rust_callback == NULL) {
        return;
    }
    context->rust_callback(tag, path, handleId, sql, info);
}

void WCDBRustDatabaseClassMethod(globalTraceSQL, RustGlobalTraceSQLCallback rust_callback) {
    size_t size = sizeof(WCDBRustGlobalTraceSQLContext);
    WCDBRustGlobalTraceSQLContext* context =
        (WCDBRustGlobalTraceSQLContext*)WCDBRustCreateGlobalRef(size);
    context->rust_callback = rust_callback;
    WCDBDatabaseGlobalTraceSQL((WCDBSQLTracer)WCDBRustDatabaseGlobalSQLTrace, context,
                               WCDBRustDestructContext);
}

typedef struct WCDBRustTraceSQLContext {
    RustTraceSQLCallback rust_callback;
} WCDBRustTraceSQLContext;

void WCDBRustDatabaseSQLTrace(WCDBRustTraceSQLContext* context,
                              long tag,
                              const char* path,
                              unsigned long long handleId,
                              const char* sql,
                              const char* info) {
    if (context == NULL || context->rust_callback == NULL) {
        return;
    }
    context->rust_callback(tag, path, handleId, sql, info);
}

void WCDBRustDatabaseClassMethod(traceSQL, void* self, RustTraceSQLCallback rust_callback) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    size_t size = sizeof(WCDBRustTraceSQLContext);
    WCDBRustTraceSQLContext* context = (WCDBRustTraceSQLContext*)WCDBRustCreateGlobalRef(size);
    context->rust_callback = rust_callback;
    WCDBDatabaseTraceSQL(selfStruct, (WCDBSQLTracer)WCDBRustDatabaseSQLTrace, context,
                         WCDBRustDestructContext);
}
//
// void WCDBRustDatabaseClassMethod(setFullSQLTraceEnable, jlong self, jboolean enable)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBDatabaseSetFullSQLTraceEnable(selfStruct, enable);
//}
//
// void WCDBRustDatabaseErrorTrace(jobject tracer, CPPError error)
//{
//    WCDBRustTryGetEnvOr(return );
//    WCDBRustTryGetDatabaseMethodId(
//    "onTraceException", "(" WCDBRustDatabaseSignature "$ExceptionTracer;J)V", return );
//    (*env)->CallStaticVoidMethod(
//    env, WCDBRustGetDatabaseClass(), g_methodId, tracer, (jlong) error.innerValue);
//    WCDBRustTryDetach;
//}
//

typedef struct WCDBRustGlobalTraceExceptionContext {
    RustGlobalTraceTraceExceptionCallback rust_callback;
} WCDBRustGlobalTraceExceptionContext;

void WCDBRustDatabaseErrorTrace(WCDBRustGlobalTraceExceptionContext* context, CPPError error) {
    if (context == NULL || context->rust_callback == NULL) {
        return;
    }
    context->rust_callback(error.innerValue);
}

void WCDBRustDatabaseClassMethod(globalTraceException,
                                 RustGlobalTraceTraceExceptionCallback rust_callback) {
    size_t size = sizeof(RustGlobalTraceTraceExceptionCallback);
    WCDBRustGlobalTraceExceptionContext* context =
        (WCDBRustGlobalTraceExceptionContext*)WCDBRustCreateGlobalRef(size);
    context->rust_callback = rust_callback;
    WCDBDatabaseGlobalTraceError((WCDBErrorTracer)WCDBRustDatabaseErrorTrace, context,
                                 WCDBRustDestructContext);
}

void WCDBRustDatabaseClassMethod(traceException,
                                 void* self,
                                 RustGlobalTraceTraceExceptionCallback rust_callback) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    size_t size = sizeof(RustGlobalTraceTraceExceptionCallback);
    WCDBRustGlobalTraceExceptionContext* context =
        (WCDBRustGlobalTraceExceptionContext*)WCDBRustCreateGlobalRef(size);
    context->rust_callback = rust_callback;
    WCDBDatabaseTraceError(selfStruct, (WCDBErrorTracer)WCDBRustDatabaseErrorTrace, context,
                           WCDBRustDestructContext);
}
//
// void WCDBRustDatabaseOperationTrace(jobject tracer, CPPDatabase database, long operation, const
// void* info)
//{
//    WCDBRustTryGetEnvOr(return );
//    WCDBRustTryGetDatabaseMethodId(
//    "onTraceOperation", "(" WCDBRustDatabaseSignature "$OperationTracer;JIJ)V", return );
//    (*env)->CallStaticVoidMethod(env,
//                                 WCDBRustGetDatabaseClass(),
//                                 g_methodId,
//                                 tracer,
//                                 (jlong) database.innerValue,
//                                 (jint) operation,
//                                 (jlong) info);
//    WCDBRustTryDetach;
//}
//
// void WCDBRustDatabaseClassMethod(globalTraceOperation, jobject tracer)
//{
//    WCDBRustTryGetVM;
//    WCDBRustCreateGlobalRef(tracer);
//    WCDBDatabaseGlobalTraceOperation(
//    (tracer != NULL ? (WCDBOperationTracer) WCDBRustDatabaseOperationTrace : NULL),
//    tracer,
//    WCDBRustDestructContext);
//}
//
// typedef struct JNIEnumerateInfoContext {
//    JNIEnv* env;
//    jobject object;
//} JNIEnumerateInfoContext;
//
// void WCDBRustDatabaseEnumerateInfoCallback(JNIEnumerateInfoContext* context,
//                                          const char* key,
//                                          CPPCommonValue value)
//{
//    JNIEnv* env = context->env;
//    jlong intValue = 0;
//    double doubleValue = 0;
//    const char* stringValue = NULL;
//    switch (value.type) {
//    case WCDBBridgedType_Int:
//        intValue = (jlong) value.intValue;
//        break;
//    case WCDBBridgedType_Double:
//        doubleValue = value.doubleValue;
//        break;
//    case WCDBBridgedType_String:
//        stringValue = (const char*) value.intValue;
//        break;
//    default:
//        break;
//    }
//    WCDBRustTryGetDatabaseMethodId("onEnumerateInfo",
//                                  "(Ljava/util/HashMap;" WCDBRustStringSignature
//                                  "IJD" WCDBRustStringSignature ")V",
//                                  return );
//    WCDBRustCreateJavaString(key);
//    WCDBRustCreateJavaString(stringValue);
//    (*env)->CallStaticVoidMethod(env,
//                                 WCDBRustGetDatabaseClass(),
//                                 g_methodId,
//                                 context->object,
//                                 jkey,
//                                 (int) value.type,
//                                 intValue,
//                                 doubleValue,
//                                 jstringValue);
//}
//
// void WCDBRustDatabaseClassMethod(enumerateInfo, jobject javaInfo, jlong cppInfo)
//{
//    JNIEnumerateInfoContext context;
//    context.object = javaInfo;
//    context.env = env;
//    WCDBEnumerateStringViewMap((const void*) cppInfo,
//                               (void*) &context,
//                               (StringViewMapEnumerator) WCDBRustDatabaseEnumerateInfoCallback);
//}
//
// void WCDBRustDatabaseBusyTrace(jobject tracer, long tag, const char* path, jlong tid, const char*
// sql)
//{
//    WCDBRustTryGetEnvOr(return );
//    WCDBRustTryGetDatabaseMethodId("onBusyTrace",
//                                  "(" WCDBRustDatabaseSignature "$BusyTracer;J"
//                                  WCDBRustStringSignature "J" WCDBRustStringSignature ")V", return
//                                  );
//    WCDBRustCreateJavaString(path);
//    WCDBRustCreateJavaString(sql);
//    (*env)->CallStaticVoidMethod(
//    env, WCDBRustGetDatabaseClass(), g_methodId, tracer, (jlong) tag, jpath, (jlong) tid, jsql);
//    WCDBRustTryDetach;
//}
//
// void WCDBRustDatabaseClassMethod(globalTraceDatabaseBusy, jobject tracer, jdouble timeOut)
//{
//    WCDBRustTryGetVM;
//    WCDBRustCreateGlobalRef(tracer);
//    WCDBCoreGlobalTraceBusy(
//    (tracer != NULL ? (WCDBBusyTracer) WCDBRustDatabaseBusyTrace : NULL), timeOut, tracer,
//    WCDBRustDestructContext);
//}

bool WCDBRustDatabaseClassMethod(removeFiles, void* self) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseRemoveFile(selfStruct);
}

//
// jboolean WCDBRustDatabaseClassMethod(moveFile, jlong self, jstring destination)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustGetString(destination);
//    jboolean ret = WCDBDatabaseMoveFile(selfStruct, destinationString);
//    WCDBRustReleaseString(destination);
//    return ret;
//}
//
// jlong WCDBRustDatabaseClassMethod(getFileSize, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    OptionalUInt64 size = WCDBDatabaseGetFileSize(selfStruct);
//    return size.hasValue ? size.value : -1;
//}
//
// void WCDBRustDatabaseClassMethod(addTokenizer, jlong self, jstring tokenizer)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustGetString(tokenizer);
//    WCDBDatabaseAddTokenizer(selfStruct, tokenizerString);
//    WCDBRustReleaseString(tokenizer);
//}
//
// void WCDBRustDatabaseClassMethod(addAuxiliaryFunction, jlong self, jstring auxiliaryFunction)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustGetString(auxiliaryFunction);
//    WCDBDatabaseAddAuxiliaryFunction(selfStruct, auxiliaryFunctionString);
//    WCDBRustReleaseString(auxiliaryFunction);
//}
//
// void WCDBRustDatabaseCorrupted(jobject notification, CPPDatabase database)
//{
//    WCDBRustTryGetEnvOr(return );
//    WCDBRustTryGetDatabaseMethodId(
//    "onCorrupted", "(" WCDBRustDatabaseSignature "$CorruptionNotification;J)V", return );
//    (*env)->CallStaticVoidMethod(
//    env, WCDBRustGetDatabaseClass(), g_methodId, notification, (jlong) database.innerValue);
//    WCDBRustTryDetach;
//}

void WCDBRustDatabaseCorrupted(void* notification, CPPDatabase database) {
    RustGlobalCorruptionNotificationCallback func =
        (RustGlobalCorruptionNotificationCallback)notification;
    func((void*)database.innerValue);
}

void WCDBRustDatabaseClassMethod(setNotificationWhenCorrupted,
                                 void* self,
                                 RustGlobalCorruptionNotificationCallback* notification) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    WCDBDatabaseSetNotificationWhenCorrupted(
        selfStruct, notification == NULL ? NULL : WCDBRustDatabaseCorrupted, notification,
        (WCDBContextDestructor)WCDBRustDestructContext);
}

bool WCDBRustDatabaseClassMethod(checkIfCorrupted, void* self) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseCheckIfCorrupted(selfStruct);
}

bool WCDBRustDatabaseClassMethod(checkIfIsAlreadyCorrupted, void* self) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseCheckIsAlreadyCorrupted(selfStruct);
}

void WCDBRustDatabaseClassMethod(enableAutoBackup, void* self, bool enable) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    WCDBDatabaseEnableAutoBackup(selfStruct, enable);
}

bool WCDBRustDatabaseClassMethod(backup, void* self) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseBackup(selfStruct);
}

bool WCDBRustDatabaseTableShouldBeBackup(void* context, const char* table) {
    if (context == NULL) {
        return false;
    }
    RustTableShouldBeBackupCallback callback = (RustTableShouldBeBackupCallback)context;
    return callback(table);
}

void WCDBRustDatabaseClassMethod(filterBackup,
                                 void* self,
                                 RustTableShouldBeBackupCallback* tableShouldBeBackup) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    WCDBDatabaseFilterBackup(
        selfStruct, tableShouldBeBackup == NULL ? NULL : WCDBRustDatabaseTableShouldBeBackup,
        tableShouldBeBackup, (WCDBContextDestructor)WCDBRustDestructContext);
}

bool WCDBRustDatabaseClassMethod(deposit, void* self) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseDeposit(selfStruct);
}

bool WCDBRustDatabaseClassMethod(removeDepositedFiles, void* self) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseRemoveDepositedFiles(selfStruct);
}

bool WCDBRustDatabaseClassMethod(containDepositedFiles, void* self) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseContainDepositedFiles(selfStruct);
}

typedef struct WCDBRustGlobalProgressMonitorContext {
    RustProgressMonitorCallback rust_callback;
} WCDBRustGlobalProgressMonitorContext;

bool WCDBRustDatabaseOnProgressUpdate(WCDBRustGlobalProgressMonitorContext* context,
                                      double percentage,
                                      double increment) {
    if (context == NULL || context->rust_callback == NULL) {
        return false;
    }
    return context->rust_callback(percentage, increment);
}

double WCDBRustDatabaseClassMethod(retrieve,
                                   void* self,
                                   RustProgressMonitorCallback* onProgressUpdate) {
    size_t size = sizeof(RustProgressMonitorCallback);
    WCDBRustGlobalProgressMonitorContext* context =
        (WCDBRustGlobalProgressMonitorContext*)WCDBRustCreateGlobalRef(size);
    context->rust_callback = onProgressUpdate;
    WCDBRustBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseRetrieve(
        selfStruct,
        onProgressUpdate != NULL ? (WCDBProgressUpdate)WCDBRustDatabaseOnProgressUpdate : NULL,
        context, (WCDBContextDestructor)WCDBRustDestructContext);
}

bool WCDBRustDatabaseClassMethod(vacuum,
                                 void* self,
                                 RustProgressMonitorCallback* onProgressUpdate) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    size_t size = sizeof(RustProgressMonitorCallback);
    WCDBRustGlobalProgressMonitorContext* context =
        (WCDBRustGlobalProgressMonitorContext*)WCDBRustCreateGlobalRef(size);
    context->rust_callback = onProgressUpdate;
    return WCDBDatabaseVacuum(
        selfStruct,
        onProgressUpdate != NULL ? (WCDBProgressUpdate)WCDBRustDatabaseOnProgressUpdate : NULL,
        context, (WCDBContextDestructor)WCDBRustDestructContext);
}

void WCDBRustDatabaseClassMethod(enableAutoVacuum, void* self, bool incremental) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    WCDBDatabaseEnableAutoVacuum(selfStruct, incremental);
}

bool WCDBRustDatabaseClassMethod(incrementalVacuum, void* self, int pageCount) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseIncrementalVacuum(selfStruct, pageCount);
}
//
// jboolean WCDBRustDatabaseClassMethod(passiveCheckpoint, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabasePassiveCheckpoint(selfStruct);
//}
//
bool WCDBRustDatabaseClassMethod(truncateCheckpoint, void* self) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseTruncateCheckpoint(selfStruct);
}

void WCDBRustDatabaseClassMethod(setAutoCheckpointEnable, void* self, bool enable) {
    WCDBRustBridgeStruct(CPPDatabase, self);
    WCDBCoreSetAutoCheckpointEnable(selfStruct, (bool)enable);
}
//
// void WCDBRustDatabaseFilterMigrate(jobject filter, const char* table, void* info,
// WCDBMigrationInfoSetter setter)
//{
//    WCDBRustTryGetEnvOr(return );
//    WCDBRustTryGetDatabaseMethodId("filterMigrate",
//                                  "(" WCDBRustDatabaseSignature
//                                  "$MigrationFilter;JJ" WCDBRustStringSignature ")V",
//                                  return );
//    WCDBRustCreateJavaString(table);
//    (*env)->CallStaticVoidMethod(
//    env, WCDBRustGetDatabaseClass(), g_methodId, filter, (jlong) setter, (jlong) info, jtable);
//    WCDBRustTryDetach;
//}
//
// void WCDBRustDatabaseClassMethod(
// addMigrationSource, jlong self, jstring sourcePath, jbyteArray cipherKey, jobject filter)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustCreateGlobalRef(filter);
//    WCDBRustGetString(sourcePath);
//    WCDBRustGetByteArrayCritical(cipherKey);
//    WCDBDatabaseAddMigration(selfStruct,
//                             sourcePathString,
//                             cipherKeyArray,
//                             cipherKeyLength,
//                             filter != NULL ? WCDBRustDatabaseFilterMigrate : NULL,
//                             filter,
//                             WCDBRustDestructContext);
//    WCDBRustReleaseByteArrayCritical(cipherKey);
//    WCDBRustReleaseString(sourcePath);
//}
//
// void WCDBRustDatabaseClassMethod(setMigrationInfo, jlong infoSetter, jlong info, jstring
// sourceTable, jlong filterCondition)
//{
//    WCDBRustGetStringCritical(sourceTable);
//    WCDBRustBridgeStruct(CPPExpression, filterCondition);
//    ((WCDBMigrationInfoSetter) infoSetter)((void*) info, sourceTableString,
//    filterConditionStruct); WCDBRustReleaseStringCritical(sourceTable);
//}
//
// jboolean WCDBRustDatabaseClassMethod(stepMigration, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabaseStepMigration(selfStruct);
//}
//
// void WCDBRustDatabaseClassMethod(enableAutoMigration, jlong self, jboolean flag)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBDatabaseEnableAutoMigration(selfStruct, flag);
//}
//
// void WCDBRustDatabaseOnTableMigrate(jobject notification,
//                                   CPPDatabase database,
//                                   const char* table,
//                                   const char* sourceTable)
//{
//    WCDBRustTryGetEnvOr(return );
//    WCDBRustTryGetDatabaseMethodId(
//    "onTableMigrated",
//    "(" WCDBRustDatabaseSignature
//    "$MigrationNotification;J" WCDBRustStringSignature WCDBRustStringSignature ")V",
//    return );
//    WCDBRustCreateJavaString(table);
//    WCDBRustCreateJavaString(sourceTable);
//    (*env)->CallStaticVoidMethod(
//    env, WCDBRustGetDatabaseClass(), g_methodId, notification, (jlong) database.innerValue,
//    jtable, jsourceTable); WCDBRustTryDetach;
//}
//
// void WCDBRustDatabaseClassMethod(setNotificationWhenMigrated, jlong self, jobject onMigrated)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustTryGetVM;
//    WCDBRustCreateGlobalRef(onMigrated);
//    WCDBDatabaseSetNotificationWhenMigrated(
//    selfStruct, onMigrated != NULL ? WCDBRustDatabaseOnTableMigrate : NULL, onMigrated,
//    WCDBRustDestructContext);
//}
//
// jboolean WCDBRustDatabaseClassMethod(isMigrated, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabaseIsMigrated(selfStruct);
//}
//
// typedef struct DataEnumeratorContext {
//    JNIEnv* env;
//    bool isString;
//    jint totalCount;
//    jint index;
//    jobjectArray* objects;
//    jobject preObject;
//    jbyte* preContent;
//} DataEnumeratorContext;
//
// void WCDBRustTryReleaseLastElement(DataEnumeratorContext* context)
//{
//    if (context->preObject == NULL || context->preContent == NULL) {
//        return;
//    }
//    JNIEnv* env = context->env;
//    if (context->isString) {
//        (*env)->ReleaseStringCritical(
//        env, context->preObject, (const jchar*) context->preContent);
//        WCDBClearAllPreAllocatedMemory();
//    } else {
//        (*env)->ReleasePrimitiveArrayCritical(
//        env, context->preObject, context->preContent, 0);
//    }
//    context->preContent = NULL;
//    context->preObject = NULL;
//}
//
// CPPData WCDBRustDataEnumerator(DataEnumeratorContext* context)
//{
//    CPPData ret;
//    if (context->index >= context->totalCount) {
//        ret.buffer = NULL;
//        ret.size = 0;
//        return ret;
//    }
//    WCDBRustTryReleaseLastElement(context);
//    JNIEnv* env = context->env;
//    if (context->isString) {
//        jstring string = (jstring) (*env)->GetObjectArrayElement(
//        env, context->objects, context->index);
//        WCDBRustGetStringCritical(string);
//        ret.buffer = (unsigned char*) stringString;
//        ret.size = stringString != NULL ? strlen(stringString) : 0;
//        context->preObject = string;
//        context->preContent = (jbyte*) string_utf16String;
//    } else {
//        jbyteArray array = (jbyteArray) (*env)->GetObjectArrayElement(
//        env, context->objects, context->index);
//        WCDBRustGetByteArrayCritical(array);
//        ret.buffer = (unsigned char*) arrayArray;
//        ret.size = arrayLength;
//        context->preObject = array;
//        context->preContent = (jbyte*) arrayArray;
//    }
//    context->index++;
//    return ret;
//}
//
// jbyteArray
// WCDBRustDatabaseClassMethod(trainDictWithStrings, jobjectArray stringArray, jbyte dictId)
//{
//    DataEnumeratorContext context;
//    context.env = env;
//    context.isString = true;
//    context.totalCount
//    = stringArray != NULL ? (*env)->GetArrayLength(env, stringArray) : 0;
//    context.objects = stringArray;
//    context.preObject = NULL;
//    context.preContent = NULL;
//    context.index = 0;
//    CPPData dict = WCDBDatabaseTrainDict(
//    dictId, (WCDBDataEnumerator) WCDBRustDataEnumerator, &context);
//    WCDBRustTryReleaseLastElement(&context);
//    jbyteArray ret = NULL;
//    if (dict.size > 0 && dict.buffer != NULL) {
//        ret = (*env)->NewByteArray(env, dict.size);
//        (*env)->SetByteArrayRegion(env, ret, 0, dict.size, (const jbyte*) dict.buffer);
//        free(dict.buffer);
//    }
//    return ret;
//}
//
// jbyteArray WCDBRustDatabaseClassMethod(trainDictWithDatas, jobjectArray dataArray, jbyte dictId)
//{
//    DataEnumeratorContext context;
//    context.env = env;
//    context.isString = false;
//    context.totalCount = dataArray != NULL ? (*env)->GetArrayLength(env, dataArray) : 0;
//    context.objects = dataArray;
//    context.preObject = NULL;
//    context.preContent = NULL;
//    context.index = 0;
//    CPPData dict = WCDBDatabaseTrainDict(
//    dictId, (WCDBDataEnumerator) WCDBRustDataEnumerator, &context);
//    WCDBRustTryReleaseLastElement(&context);
//    jbyteArray ret = NULL;
//    if (dict.size > 0 && dict.buffer != NULL) {
//        ret = (*env)->NewByteArray(env, dict.size);
//        (*env)->SetByteArrayRegion(env, ret, 0, dict.size, (const jbyte*) dict.buffer);
//        free(dict.buffer);
//    }
//    return ret;
//}
//
// jboolean WCDBRustDatabaseClassMethod(registerDict, jbyteArray dict, jbyte dictId)
//{
//    WCDBRustGetByteArray(dict);
//    bool ret = WCDBDatabaseRegisterDict(dictArray, dictLength, dictId);
//    WCDBRustReleaseByteArray(dict);
//    return ret;
//}
//
// void WCDBRustDatabaseClassMethod(addZSTDNormalCompress, jlong info, jlong column)
//{
//    WCDBRustBridgeStruct(CPPColumn, column);
//    WCDBDatabaseSetZSTDNormalCompress((void*) info, columnStruct);
//}
//
// void WCDBRustDatabaseClassMethod(addZSTDDictCompress, jlong info, jlong column, jbyte dictId)
//{
//    WCDBRustBridgeStruct(CPPColumn, column);
//    WCDBDatabaseSetZSTDDictCompress((void*) info, columnStruct, dictId);
//}
//
// void WCDBRustDatabaseClassMethod(addZSTDMultiDictCompress,
//                                jlong info,
//                                jlong column,
//                                jlong matchColumn,
//                                jlongArray values,
//                                jbyteArray dictIds)
//{
//    WCDBRustGetLongArray(values);
//    WCDBRustGetByteArray(dictIds);
//    WCDBRustBridgeStruct(CPPColumn, column);
//    WCDBRustBridgeStruct(CPPColumn, matchColumn);
//    WCDBDatabaseSetZSTDMultiDictCompress(
//    (void*) info, columnStruct, matchColumnStruct, (const long long*) valuesArray, dictIdsArray,
//    dictIdsLength); WCDBRustReleaseLongArray(values); WCDBRustReleaseByteArray(dictIds);
//}
//
// void WCDBRustDatabaseClassMethod(enableReplaceCompression, jlong info)
//{
//    WCDBDatabaseEnableReplaceCompresssion((void*) info);
//}
//
// void WCDBRustDatabaseFilterCompress(jobject filter, const char* table, void* info)
//{
//    WCDBRustTryGetEnvOr(return );
//    WCDBRustTryGetDatabaseMethodId("filterCompress",
//                                  "(" WCDBRustDatabaseSignature
//                                  "$CompressionFilter;J" WCDBRustStringSignature ")V",
//                                  return );
//    WCDBRustCreateJavaString(table);
//    (*env)->CallStaticVoidMethod(
//    env, WCDBRustGetDatabaseClass(), g_methodId, filter, (jlong) info, jtable);
//    WCDBRustTryDetach;
//}
//
// void WCDBRustDatabaseClassMethod(setCompression, jlong self, jobject filter)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustCreateGlobalRef(filter);
//    WCDBDatabaseSetCompression(
//    selfStruct, filter != NULL ? WCDBRustDatabaseFilterCompress : NULL, filter,
//    WCDBRustDestructContext);
//}
//
// void WCDBRustDatabaseClassMethod(disableCompressNewData, jlong self, jboolean disable)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBDatabaseDisableCompressNewData(selfStruct, disable);
//}
//
// jboolean WCDBRustDatabaseClassMethod(stepCompression, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabaseStepCompression(selfStruct);
//}
//
// void WCDBRustDatabaseClassMethod(enableAutoCompression, jlong self, jboolean enable)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBDatabaseEnableAutoCompression(selfStruct, enable);
//}
//
// void WCDBRustDatabaseOnTableCompressed(jobject notification, CPPDatabase database, const char*
// table)
//{
//    WCDBRustTryGetEnvOr(return );
//    WCDBRustTryGetDatabaseMethodId("onTableCompressed",
//                                  "(" WCDBRustDatabaseSignature
//                                  "$CompressionNotification;J" WCDBRustStringSignature ")V",
//                                  return );
//    WCDBRustCreateJavaString(table);
//    (*env)->CallStaticVoidMethod(
//    env, WCDBRustGetDatabaseClass(), g_methodId, notification, (jlong) database.innerValue,
//    jtable); WCDBRustTryDetach;
//}
//
// void WCDBRustDatabaseClassMethod(setNotificationWhenCompressed, jlong self, jobject onCompressed)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustTryGetVM;
//    WCDBRustCreateGlobalRef(onCompressed);
//    WCDBDatabaseSetNotificationWhenCompressed(
//    selfStruct, onCompressed != NULL ? WCDBRustDatabaseOnTableCompressed : NULL, onCompressed,
//    WCDBRustDestructContext);
//}
//
// jboolean WCDBRustDatabaseClassMethod(isCompressed, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabaseIsCompressed(selfStruct);
//}
//
// jdouble WCDBRustDatabaseClassMethod(rollbackCompression, jlong self, jobject onProgressUpdate)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustTryGetVM;
//    WCDBRustCreateGlobalRef(onProgressUpdate);
//    return WCDBDatabaseRollbackCompression(
//    selfStruct,
//    onProgressUpdate != NULL ? (WCDBProgressUpdate) WCDBRustDatabaseOnProgressUpdate : NULL,
//    onProgressUpdate,
//    WCDBRustDestructContext);
//}
//
// jint WCDBRustDatabaseClassMethod(getNumberOfAliveHandle, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabaseGetAliveHandleCount(selfStruct);
//}
