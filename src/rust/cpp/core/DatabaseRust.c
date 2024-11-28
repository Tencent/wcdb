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
//
//jlong WCDBRustDatabaseClassMethod(getError, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return (jlong) WCDBDatabaseGetError(selfStruct).innerValue;
//}
//
//jlong WCDBRustDatabaseClassMethod(getTag, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return (jlong) WCDBDatabaseGetTag(selfStruct);
//}
//
//void WCDBRustDatabaseClassMethod(setTag, jlong self, jlong tag)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBDatabaseSetTag(selfStruct, tag);
//}
//
//jstring WCDBRustDatabaseClassMethod(getPath, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustCreateJStringAndReturn(WCDBDatabaseGetPath(selfStruct));
//}
//
//typedef struct StringEnumeratorContext {
//    JNIEnv* env;
//    jobject array;
//    jclass arrayClass;
//} StringEnumeratorContext;
//
//void WCDBRustStringEnumerator(StringEnumeratorContext* context, const char* string)
//{
//    JNIEnv* env = context->env;
//    WCDBRustGetObjectMethodId(g_addMethod, context->arrayClass, "add", "(Ljava/lang/Object;)Z");
//    if (g_addMethod == NULL) {
//        return;
//    }
//    WCDBRustCreateJavaString(string);
//    (*env)->CallBooleanMethod(env, context->array, g_addMethod, jstring);
//}
//
//jobject WCDBRustDatabaseClassMethod(getPaths, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustFindClass(g_arrayClass, "java/util/ArrayList", return NULL);
//    WCDBRustGetObjectMethodId(g_arrayInit, g_arrayClass, "<init>", "()V");
//    jobject arrayList = (*env)->NewObject(env, g_arrayClass, g_arrayInit);
//    StringEnumeratorContext context;
//    context.env = env;
//    context.array = arrayList;
//    context.arrayClass = g_arrayClass;
//    WCDBDatabaseGetPaths(selfStruct, &context, (WCDBStringEnumerater) WCDBRustStringEnumerator);
//    return arrayList;
//}
//
//jlong WCDBRustDatabaseClassMethod(getHandle, jlong self, jboolean writeHint)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return (jlong) WCDBDatabaseGetHandle(selfStruct, writeHint).innerValue;
//}
//
//jboolean WCDBRustDatabaseClassMethod(canOpen, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabaseCanOpen(selfStruct);
//}
//
//jboolean WCDBRustDatabaseClassMethod(isOpened, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabaseIsOpened(selfStruct);
//}
//
//jboolean WCDBRustDatabaseClassMethod(isBlockaded, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabaseIsBlockaded(selfStruct);
//}
//
//typedef void (*DatabaseCloseCallback)();
//
//typedef struct CloseDatabaseContext {
//    JNIEnv* env;
//    jobject callback;
//} CloseDatabaseContext;
//
//void WCDBRustDatabaseCloseCallback(CloseDatabaseContext* context)
//{
//    JNIEnv* env = context->env;
//    WCDBRustTryGetDatabaseMethodId(
//    "onClose", "(" WCDBRustDatabaseSignature "$CloseCallBack;)V", return );
//    (*env)->CallStaticVoidMethod(
//    env, WCDBRustGetDatabaseClass(), g_methodId, context->callback);
//}

void WCDBRustDatabase_close(void* self, void* context, WCDBDatabaseCloseCallback callback)
{
    WCDBRustBridgeStruct(CPPDatabase, self);
    WCDBDatabaseClose(selfStruct, context, callback);
}

//void WCDBRustDatabaseClassMethod(blockade, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBDatabaseBlockade(selfStruct);
//}
//
//void WCDBRustDatabaseClassMethod(unblockade, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBDatabaseUnblockade(selfStruct);
//}
//
//void WCDBRustDatabaseClassMethod(purge, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBDatabasePurge(selfStruct);
//}
//
//void WCDBRustDatabaseClassMethod(configCipher, jlong self, jbyteArray cipherKey, jint pageSize, jint cipherVersion)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustGetByteArrayCritical(cipherKey);
//    WCDBDatabaseConfigCipher(
//    selfStruct, cipherKeyArray, cipherKeyLength, pageSize, cipherVersion);
//    WCDBRustReleaseByteArrayCritical(cipherKey);
//}
//
//bool WCDBRustDatabaseConfig(jobject config, CPPHandle handle)
//{
//    WCDBRustTryGetEnvOr(return false);
//    WCDBRustTryGetDatabaseMethodId(
//    "onConfig", "(J" WCDBRustDatabaseSignature "$Config;)Z", return false);
//    jboolean ret = (*env)->CallStaticBooleanMethod(
//    env, WCDBRustGetDatabaseClass(), g_methodId, (jlong) handle.innerValue, config);
//    if ((*env)->ExceptionCheck(env)) {
//        ret = false;
//    }
//    WCDBRustTryDetach;
//    return ret;
//}
//
//void WCDBRustDatabaseClassMethod(
//config, jlong self, jstring name, jobject invocation, jobject unInvocation, jint priority)
//{
//    WCDBRustTryGetVM;
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustCreateGlobalRel(invocation);
//    WCDBRustCreateGlobalRel(unInvocation);
//    WCDBRustGetString(name);
//    WCDBDatabaseConfig(selfStruct,
//                       nameString,
//                       invocation != NULL ? WCDBRustDatabaseConfig : NULL,
//                       invocation,
//                       unInvocation != NULL ? WCDBRustDatabaseConfig : NULL,
//                       unInvocation,
//                       priority,
//                       WCDBRustDestructContext);
//    WCDBRustReleaseString(name);
//}
//
//void WCDBRustDatabaseClassMethod(enableLiteMode, jlong self, jboolean enable)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBDatabaseEnableLiteMode(selfStruct, enable);
//}
//
//void WCDBRustDatabasePerformanceTrace(jobject tracer,
//                                     long tag,
//                                     const char* path,
//                                     unsigned long long handleId,
//                                     const char* sql,
//                                     const CPPPerformanceInfo* info)
//{
//    WCDBRustTryGetEnvOr(return );
//    WCDBRustTryGetDatabaseMethodId("onTracePerformance",
//                                  "(" WCDBRustDatabaseSignature "$PerformanceTracer;J" WCDBRustStringSignature
//                                  "J" WCDBRustStringSignature "J[I)V",
//                                  return );
//    WCDBRustCreateJavaString(path);
//    WCDBRustCreateJavaString(sql);
//    jint size = sizeof(CPPPerformanceInfo) / sizeof(int) - 2;
//    jintArray infoValues = (*env)->NewIntArray(env, size);
//    if (infoValues != NULL) {
//        (*env)->SetIntArrayRegion(env, infoValues, 0, size, (jint*) info);
//    }
//    (*env)->CallStaticVoidMethod(env,
//                                 WCDBRustGetDatabaseClass(),
//                                 g_methodId,
//                                 tracer,
//                                 (jlong) tag,
//                                 jpath,
//                                 (jlong) handleId,
//                                 jsql,
//                                 (jlong) info->costInNanoseconds,
//                                 infoValues);
//    WCDBRustTryDetach;
//}
//
//void WCDBRustDatabaseClassMethod(globalTracePerformance, jobject tracer)
//{
//    WCDBRustTryGetVM;
//    WCDBRustCreateGlobalRel(tracer);
//    WCDBDatabaseGlobalTracePerformance(
//    tracer != NULL ? WCDBRustDatabasePerformanceTrace : NULL, tracer, WCDBRustDestructContext);
//}
//
//void WCDBRustDatabaseClassMethod(tracePerformance, jlong self, jobject tracer)
//{
//    WCDBRustTryGetVM;
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustCreateGlobalRel(tracer);
//    WCDBDatabaseTracePerformance(
//    selfStruct, tracer != NULL ? WCDBRustDatabasePerformanceTrace : NULL, tracer, WCDBRustDestructContext);
//}
//
//void WCDBRustDatabaseSQLTrace(jobject tracer,
//                             long tag,
//                             const char* path,
//                             unsigned long long handleId,
//                             const char* sql,
//                             const char* info)
//{
//    WCDBRustTryGetEnvOr(return );
//    WCDBRustTryGetDatabaseMethodId("onTraceSQL",
//                                  "(" WCDBRustDatabaseSignature "$SQLTracer;J" WCDBRustStringSignature
//                                  "J" WCDBRustStringSignature WCDBRustStringSignature ")V",
//                                  return );
//    WCDBRustCreateJavaString(path);
//    WCDBRustCreateJavaString(sql);
//    WCDBRustCreateJavaString(info);
//    (*env)->CallStaticVoidMethod(
//    env, WCDBRustGetDatabaseClass(), g_methodId, tracer, (jlong) tag, jpath, (jlong) handleId, jsql, jinfo);
//    WCDBRustTryDetach;
//}
//
//void WCDBRustDatabaseClassMethod(globalTraceSQL, jobject tracer)
//{
//    WCDBRustTryGetVM;
//    WCDBRustCreateGlobalRel(tracer);
//    WCDBDatabaseGlobalTraceSQL(
//    tracer != NULL ? WCDBRustDatabaseSQLTrace : NULL, tracer, WCDBRustDestructContext);
//}
//
//void WCDBRustDatabaseClassMethod(traceSQL, jlong self, jobject tracer)
//{
//    WCDBRustTryGetVM;
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustCreateGlobalRel(tracer);
//    WCDBDatabaseTraceSQL(
//    selfStruct, tracer != NULL ? WCDBRustDatabaseSQLTrace : NULL, tracer, WCDBRustDestructContext);
//}
//
//void WCDBRustDatabaseClassMethod(setFullSQLTraceEnable, jlong self, jboolean enable)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBDatabaseSetFullSQLTraceEnable(selfStruct, enable);
//}
//
//void WCDBRustDatabaseErrorTrace(jobject tracer, CPPError error)
//{
//    WCDBRustTryGetEnvOr(return );
//    WCDBRustTryGetDatabaseMethodId(
//    "onTraceException", "(" WCDBRustDatabaseSignature "$ExceptionTracer;J)V", return );
//    (*env)->CallStaticVoidMethod(
//    env, WCDBRustGetDatabaseClass(), g_methodId, tracer, (jlong) error.innerValue);
//    WCDBRustTryDetach;
//}
//
//void WCDBRustDatabaseClassMethod(globalTraceError, jobject tracer)
//{
//    WCDBRustTryGetVM;
//    WCDBRustCreateGlobalRel(tracer);
//    WCDBDatabaseGlobalTraceError(
//    tracer != NULL ? WCDBRustDatabaseErrorTrace : NULL, tracer, WCDBRustDestructContext);
//}
//
//void WCDBRustDatabaseClassMethod(traceError, jlong self, jobject tracer)
//{
//    WCDBRustTryGetVM;
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustCreateGlobalRel(tracer);
//    WCDBDatabaseTraceError(
//    selfStruct, tracer != NULL ? WCDBRustDatabaseErrorTrace : NULL, tracer, WCDBRustDestructContext);
//}
//
//void WCDBRustDatabaseOperationTrace(jobject tracer, CPPDatabase database, long operation, const void* info)
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
//void WCDBRustDatabaseClassMethod(globalTraceOperation, jobject tracer)
//{
//    WCDBRustTryGetVM;
//    WCDBRustCreateGlobalRel(tracer);
//    WCDBDatabaseGlobalTraceOperation(
//    (tracer != NULL ? (WCDBOperationTracer) WCDBRustDatabaseOperationTrace : NULL),
//    tracer,
//    WCDBRustDestructContext);
//}
//
//typedef struct JNIEnumerateInfoContext {
//    JNIEnv* env;
//    jobject object;
//} JNIEnumerateInfoContext;
//
//void WCDBRustDatabaseEnumerateInfoCallback(JNIEnumerateInfoContext* context,
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
//void WCDBRustDatabaseClassMethod(enumerateInfo, jobject javaInfo, jlong cppInfo)
//{
//    JNIEnumerateInfoContext context;
//    context.object = javaInfo;
//    context.env = env;
//    WCDBEnumerateStringViewMap((const void*) cppInfo,
//                               (void*) &context,
//                               (StringViewMapEnumerator) WCDBRustDatabaseEnumerateInfoCallback);
//}
//
//void WCDBRustDatabaseBusyTrace(jobject tracer, long tag, const char* path, jlong tid, const char* sql)
//{
//    WCDBRustTryGetEnvOr(return );
//    WCDBRustTryGetDatabaseMethodId("onBusyTrace",
//                                  "(" WCDBRustDatabaseSignature "$BusyTracer;J" WCDBRustStringSignature
//                                  "J" WCDBRustStringSignature ")V",
//                                  return );
//    WCDBRustCreateJavaString(path);
//    WCDBRustCreateJavaString(sql);
//    (*env)->CallStaticVoidMethod(
//    env, WCDBRustGetDatabaseClass(), g_methodId, tracer, (jlong) tag, jpath, (jlong) tid, jsql);
//    WCDBRustTryDetach;
//}
//
//void WCDBRustDatabaseClassMethod(globalTraceDatabaseBusy, jobject tracer, jdouble timeOut)
//{
//    WCDBRustTryGetVM;
//    WCDBRustCreateGlobalRel(tracer);
//    WCDBCoreGlobalTraceBusy(
//    (tracer != NULL ? (WCDBBusyTracer) WCDBRustDatabaseBusyTrace : NULL), timeOut, tracer, WCDBRustDestructContext);
//}
//
//jboolean WCDBRustDatabaseClassMethod(removeFiles, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabaseRemoveFile(selfStruct);
//}
//
//jboolean WCDBRustDatabaseClassMethod(moveFile, jlong self, jstring destination)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustGetString(destination);
//    jboolean ret = WCDBDatabaseMoveFile(selfStruct, destinationString);
//    WCDBRustReleaseString(destination);
//    return ret;
//}
//
//jlong WCDBRustDatabaseClassMethod(getFileSize, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    OptionalUInt64 size = WCDBDatabaseGetFileSize(selfStruct);
//    return size.hasValue ? size.value : -1;
//}
//
//void WCDBRustDatabaseClassMethod(addTokenizer, jlong self, jstring tokenizer)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustGetString(tokenizer);
//    WCDBDatabaseAddTokenizer(selfStruct, tokenizerString);
//    WCDBRustReleaseString(tokenizer);
//}
//
//void WCDBRustDatabaseClassMethod(addAuxiliaryFunction, jlong self, jstring auxiliaryFunction)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustGetString(auxiliaryFunction);
//    WCDBDatabaseAddAuxiliaryFunction(selfStruct, auxiliaryFunctionString);
//    WCDBRustReleaseString(auxiliaryFunction);
//}
//
//void WCDBRustDatabaseCorrupted(jobject notification, CPPDatabase database)
//{
//    WCDBRustTryGetEnvOr(return );
//    WCDBRustTryGetDatabaseMethodId(
//    "onCorrupted", "(" WCDBRustDatabaseSignature "$CorruptionNotification;J)V", return );
//    (*env)->CallStaticVoidMethod(
//    env, WCDBRustGetDatabaseClass(), g_methodId, notification, (jlong) database.innerValue);
//    WCDBRustTryDetach;
//}
//
//void WCDBRustDatabaseClassMethod(setNotificationWhenCorrupted, jlong self, jobject notification)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustTryGetVM;
//    WCDBRustCreateGlobalRel(notification);
//    WCDBDatabaseSetNotificationWhenCorrupted(
//    selfStruct, notification != NULL ? WCDBRustDatabaseCorrupted : NULL, notification, WCDBRustDestructContext);
//}
//
//jboolean WCDBRustDatabaseClassMethod(checkIfCorrupted, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabaseCheckIfCorrupted(selfStruct);
//}
//
//jboolean WCDBRustDatabaseClassMethod(checkIfIsAlreadyCorrupted, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabaseCheckIsAlreadyCorrupted(selfStruct);
//}
//
//void WCDBRustDatabaseClassMethod(enableAutoBackup, jlong self, jboolean enable)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBDatabaseEnableAutoBackup(selfStruct, enable);
//}
//
//jboolean WCDBRustDatabaseClassMethod(backup, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabaseBackup(selfStruct);
//}
//
//bool WCDBRustDatabaseTableShouldBeBackup(jobject filter, const char* table)
//{
//    WCDBRustTryGetEnvOr(return false);
//    WCDBRustTryGetDatabaseMethodId("checkTableShouldBeBackup",
//                                  "(" WCDBRustDatabaseSignature
//                                  "$BackupFilter;" WCDBRustStringSignature ")Z",
//                                  return false);
//    WCDBRustCreateJavaString(table);
//    bool ret = (*env)->CallStaticBooleanMethod(
//    env, WCDBRustGetDatabaseClass(), g_methodId, filter, jtable);
//    if ((*env)->ExceptionCheck(env)) {
//        ret = false;
//    }
//    WCDBRustTryDetach;
//    return ret;
//}
//
//void WCDBRustDatabaseClassMethod(filterBackup, jlong self, jobject tableShouldBeBackup)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustTryGetVM;
//    WCDBRustCreateGlobalRel(tableShouldBeBackup);
//    WCDBDatabaseFilterBackup(
//    selfStruct,
//    tableShouldBeBackup != NULL ? WCDBRustDatabaseTableShouldBeBackup : NULL,
//    tableShouldBeBackup,
//    WCDBRustDestructContext);
//}
//
//jboolean WCDBRustDatabaseClassMethod(deposit, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabaseDeposit(selfStruct);
//}
//
//jboolean WCDBRustDatabaseClassMethod(removeDepositedFiles, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabaseRemoveDepositedFiles(selfStruct);
//}
//
//jboolean WCDBRustDatabaseClassMethod(containDepositedFiles, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabaseContainDepositedFiles(selfStruct);
//}
//
//bool WCDBRustDatabaseOnProgressUpdate(jobject monitor, double percentage, double increment)
//{
//    WCDBRustTryGetEnvOr(return false);
//    WCDBRustTryGetDatabaseMethodId(
//    "onProgressUpdate", "(" WCDBRustDatabaseSignature "$ProgressMonitor;DD)Z", return false);
//    bool ret = (*env)->CallStaticBooleanMethod(
//    env, WCDBRustGetDatabaseClass(), g_methodId, monitor, (jdouble) percentage, (jdouble) increment);
//    WCDBRustTryDetach;
//    return ret;
//}
//
//jdouble WCDBRustDatabaseClassMethod(retrieve, jlong self, jobject onProgressUpdate)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustTryGetVM;
//    WCDBRustCreateGlobalRel(onProgressUpdate);
//    return WCDBDatabaseRetrieve(
//    selfStruct,
//    onProgressUpdate != NULL ? (WCDBProgressUpdate) WCDBRustDatabaseOnProgressUpdate : NULL,
//    onProgressUpdate,
//    WCDBRustDestructContext);
//}
//
//jdouble WCDBRustDatabaseClassMethod(vacuum, jlong self, jobject onProgressUpdate)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustTryGetVM;
//    WCDBRustCreateGlobalRel(onProgressUpdate);
//    return WCDBDatabaseVacuum(
//    selfStruct,
//    onProgressUpdate != NULL ? (WCDBProgressUpdate) WCDBRustDatabaseOnProgressUpdate : NULL,
//    onProgressUpdate,
//    WCDBRustDestructContext);
//}
//
//void WCDBRustDatabaseClassMethod(enableAutoVacuum, jlong self, jboolean incremental)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBDatabaseEnableAutoVacuum(selfStruct, incremental);
//}
//
//jboolean WCDBRustDatabaseClassMethod(incrementalVacuum, jlong self, jint pageCount)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabaseIncrementalVacuum(selfStruct, pageCount);
//}
//
//jboolean WCDBRustDatabaseClassMethod(passiveCheckpoint, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabasePassiveCheckpoint(selfStruct);
//}
//
//jboolean WCDBRustDatabaseClassMethod(truncateCheckpoint, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabaseTruncateCheckpoint(selfStruct);
//}
//
//void WCDBRustDatabaseClassMethod(setAutoCheckpointEnable, jlong self, jboolean enable)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBCoreSetAutoCheckpointEnable(selfStruct, (bool) enable);
//}
//
//void WCDBRustDatabaseFilterMigrate(jobject filter, const char* table, void* info, WCDBMigrationInfoSetter setter)
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
//void WCDBRustDatabaseClassMethod(
//addMigrationSource, jlong self, jstring sourcePath, jbyteArray cipherKey, jobject filter)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustCreateGlobalRel(filter);
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
//void WCDBRustDatabaseClassMethod(setMigrationInfo, jlong infoSetter, jlong info, jstring sourceTable, jlong filterCondition)
//{
//    WCDBRustGetStringCritical(sourceTable);
//    WCDBRustBridgeStruct(CPPExpression, filterCondition);
//    ((WCDBMigrationInfoSetter) infoSetter)((void*) info, sourceTableString, filterConditionStruct);
//    WCDBRustReleaseStringCritical(sourceTable);
//}
//
//jboolean WCDBRustDatabaseClassMethod(stepMigration, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabaseStepMigration(selfStruct);
//}
//
//void WCDBRustDatabaseClassMethod(enableAutoMigration, jlong self, jboolean flag)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBDatabaseEnableAutoMigration(selfStruct, flag);
//}
//
//void WCDBRustDatabaseOnTableMigrate(jobject notification,
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
//    env, WCDBRustGetDatabaseClass(), g_methodId, notification, (jlong) database.innerValue, jtable, jsourceTable);
//    WCDBRustTryDetach;
//}
//
//void WCDBRustDatabaseClassMethod(setNotificationWhenMigrated, jlong self, jobject onMigrated)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustTryGetVM;
//    WCDBRustCreateGlobalRel(onMigrated);
//    WCDBDatabaseSetNotificationWhenMigrated(
//    selfStruct, onMigrated != NULL ? WCDBRustDatabaseOnTableMigrate : NULL, onMigrated, WCDBRustDestructContext);
//}
//
//jboolean WCDBRustDatabaseClassMethod(isMigrated, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabaseIsMigrated(selfStruct);
//}
//
//typedef struct DataEnumeratorContext {
//    JNIEnv* env;
//    bool isString;
//    jint totalCount;
//    jint index;
//    jobjectArray* objects;
//    jobject preObject;
//    jbyte* preContent;
//} DataEnumeratorContext;
//
//void WCDBRustTryReleaseLastElement(DataEnumeratorContext* context)
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
//CPPData WCDBRustDataEnumerator(DataEnumeratorContext* context)
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
//jbyteArray
//WCDBRustDatabaseClassMethod(trainDictWithStrings, jobjectArray stringArray, jbyte dictId)
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
//jbyteArray WCDBRustDatabaseClassMethod(trainDictWithDatas, jobjectArray dataArray, jbyte dictId)
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
//jboolean WCDBRustDatabaseClassMethod(registerDict, jbyteArray dict, jbyte dictId)
//{
//    WCDBRustGetByteArray(dict);
//    bool ret = WCDBDatabaseRegisterDict(dictArray, dictLength, dictId);
//    WCDBRustReleaseByteArray(dict);
//    return ret;
//}
//
//void WCDBRustDatabaseClassMethod(addZSTDNormalCompress, jlong info, jlong column)
//{
//    WCDBRustBridgeStruct(CPPColumn, column);
//    WCDBDatabaseSetZSTDNormalCompress((void*) info, columnStruct);
//}
//
//void WCDBRustDatabaseClassMethod(addZSTDDictCompress, jlong info, jlong column, jbyte dictId)
//{
//    WCDBRustBridgeStruct(CPPColumn, column);
//    WCDBDatabaseSetZSTDDictCompress((void*) info, columnStruct, dictId);
//}
//
//void WCDBRustDatabaseClassMethod(addZSTDMultiDictCompress,
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
//    (void*) info, columnStruct, matchColumnStruct, (const long long*) valuesArray, dictIdsArray, dictIdsLength);
//    WCDBRustReleaseLongArray(values);
//    WCDBRustReleaseByteArray(dictIds);
//}
//
//void WCDBRustDatabaseClassMethod(enableReplaceCompression, jlong info)
//{
//    WCDBDatabaseEnableReplaceCompresssion((void*) info);
//}
//
//void WCDBRustDatabaseFilterCompress(jobject filter, const char* table, void* info)
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
//void WCDBRustDatabaseClassMethod(setCompression, jlong self, jobject filter)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustCreateGlobalRel(filter);
//    WCDBDatabaseSetCompression(
//    selfStruct, filter != NULL ? WCDBRustDatabaseFilterCompress : NULL, filter, WCDBRustDestructContext);
//}
//
//void WCDBRustDatabaseClassMethod(disableCompressNewData, jlong self, jboolean disable)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBDatabaseDisableCompressNewData(selfStruct, disable);
//}
//
//jboolean WCDBRustDatabaseClassMethod(stepCompression, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabaseStepCompression(selfStruct);
//}
//
//void WCDBRustDatabaseClassMethod(enableAutoCompression, jlong self, jboolean enable)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBDatabaseEnableAutoCompression(selfStruct, enable);
//}
//
//void WCDBRustDatabaseOnTableCompressed(jobject notification, CPPDatabase database, const char* table)
//{
//    WCDBRustTryGetEnvOr(return );
//    WCDBRustTryGetDatabaseMethodId("onTableCompressed",
//                                  "(" WCDBRustDatabaseSignature
//                                  "$CompressionNotification;J" WCDBRustStringSignature ")V",
//                                  return );
//    WCDBRustCreateJavaString(table);
//    (*env)->CallStaticVoidMethod(
//    env, WCDBRustGetDatabaseClass(), g_methodId, notification, (jlong) database.innerValue, jtable);
//    WCDBRustTryDetach;
//}
//
//void WCDBRustDatabaseClassMethod(setNotificationWhenCompressed, jlong self, jobject onCompressed)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustTryGetVM;
//    WCDBRustCreateGlobalRel(onCompressed);
//    WCDBDatabaseSetNotificationWhenCompressed(
//    selfStruct, onCompressed != NULL ? WCDBRustDatabaseOnTableCompressed : NULL, onCompressed, WCDBRustDestructContext);
//}
//
//jboolean WCDBRustDatabaseClassMethod(isCompressed, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabaseIsCompressed(selfStruct);
//}
//
//jdouble WCDBRustDatabaseClassMethod(rollbackCompression, jlong self, jobject onProgressUpdate)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    WCDBRustTryGetVM;
//    WCDBRustCreateGlobalRel(onProgressUpdate);
//    return WCDBDatabaseRollbackCompression(
//    selfStruct,
//    onProgressUpdate != NULL ? (WCDBProgressUpdate) WCDBRustDatabaseOnProgressUpdate : NULL,
//    onProgressUpdate,
//    WCDBRustDestructContext);
//}
//
//jint WCDBRustDatabaseClassMethod(getNumberOfAliveHandle, jlong self)
//{
//    WCDBRustBridgeStruct(CPPDatabase, self);
//    return WCDBDatabaseGetAliveHandleCount(selfStruct);
//}
