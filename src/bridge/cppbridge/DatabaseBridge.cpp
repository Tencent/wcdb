//
// Created by qiuwenchen on 2022/3/31.
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

#include "DatabaseBridge.h"
#include "CipherConfig.hpp"
#include "CompressionCenter.hpp"
#include "CompressionConst.hpp"
#include "Core.hpp"
#include "CustomConfig.hpp"
#include "DBOperationNotifier.hpp"
#include "InnerDatabase.hpp"
#include "ObjectBridge.hpp"

CPPError WCDBDatabaseGetError(CPPDatabase database)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, CPPError());
    const WCDB::Error& error = cppDatabase->getThreadedError();
    return WCDBCreateUnmanagedCPPObject(CPPError, &error);
}

long WCDBDatabaseGetTag(CPPDatabase database)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, 0);
    return cppDatabase->getTag();
}

void WCDBDatabaseSetTag(CPPDatabase database, long tag)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    cppDatabase->setTag(tag);
}

const char* WCDBDatabaseGetPath(CPPDatabase database)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, nullptr);
    return cppDatabase->getPath().data();
}

void WCDBDatabaseGetPaths(CPPDatabase database,
                          void* _Nullable context,
                          WCDBStringEnumerater _Nonnull enumerator)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    std::list<WCDB::StringView> paths = cppDatabase->getPaths();
    for (const auto& path : paths) {
        enumerator(context, path.data());
    }
}

CPPHandle WCDBDatabaseGetHandle(CPPDatabase database, bool writeHint)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, CPPHandle());
    WCDB::RecyclableHandle cppHandle = cppDatabase->getHandle(writeHint);
    if (cppHandle == nullptr) {
        CPPHandle invalidHandle;
        invalidHandle.innerValue = NULL;
        return invalidHandle;
    }
    return WCDBCreateRecylableCPPObject(CPPHandle, cppHandle);
}

bool WCDBDatabaseCanOpen(CPPDatabase database)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, false);
    return cppDatabase->canOpen();
}

bool WCDBDatabaseIsOpened(CPPDatabase database)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, false);
    return cppDatabase->isOpened();
}

bool WCDBDatabaseIsBlockaded(CPPDatabase database)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, false);
    return cppDatabase->isBlockaded();
}

void WCDBDatabaseClose(CPPDatabase database, void* context, WCDBDatabaseCloseCallback callback)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    cppDatabase->close([callback, context]() {
        if (callback != nullptr) {
            callback(context);
        }
    });
}

void WCDBDatabaseBlockade(CPPDatabase database)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    cppDatabase->blockade();
}

void WCDBDatabaseUnblockade(CPPDatabase database)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    cppDatabase->unblockade();
}

void WCDBDatabasePurge(CPPDatabase database)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    cppDatabase->purge();
}

void WCDBDatabaseConfigCipher(CPPDatabase database,
                              const unsigned char* cipherKey,
                              int keyLength,
                              int pageSize,
                              int cipherVersion)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    if (cipherKey) {
        cppDatabase->setConfig(
        WCDB::CipherConfigName,
        std::static_pointer_cast<WCDB::Config>(std::make_shared<WCDB::CipherConfig>(
        WCDB::UnsafeData::immutable(cipherKey, (size_t) keyLength), pageSize, cipherVersion)),
        WCDB::Configs::Priority::Highest);
    } else {
        cppDatabase->removeConfig(WCDB::CipherConfigName);
    }
}

void WCDBDatabaseConfig(CPPDatabase database,
                        const char* _Nullable name,
                        WCDBConfigCallback _Nonnull invocation,
                        void* _Nonnull invocationContext,
                        WCDBConfigCallback _Nullable unInvocation,
                        void* _Nullable unInvocationContext,
                        int priority,
                        WCDBContextDestructor _Nonnull destructor)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    if (invocation == NULL) {
        cppDatabase->removeConfig(name);
        return;
    }
    WCDB::RecyclableContext recyclableInvocationContext(invocationContext, destructor);

    WCDB::CustomConfig::Invocation cppInvocation
    = [invocation, recyclableInvocationContext](WCDB::InnerHandle* handle) -> bool {
        return invocation(recyclableInvocationContext.get(),
                          WCDBCreateUnmanagedCPPObject(CPPHandle, handle));
    };

    WCDB::CustomConfig::Invocation cppUninvocation = nullptr;
    if (unInvocation != nullptr) {
        assert(unInvocationContext != nullptr);
        WCDB::RecyclableContext recyclableUnInvocationContext(unInvocationContext, destructor);
        cppUninvocation
        = [unInvocation, recyclableUnInvocationContext](WCDB::InnerHandle* handle) -> bool {
            return unInvocation(recyclableUnInvocationContext.get(),
                                WCDBCreateUnmanagedCPPObject(CPPHandle, handle));
        };
    }
    cppDatabase->setConfig(
    WCDB::UnsafeStringView(name),
    std::static_pointer_cast<WCDB::Config>(
    std::make_shared<WCDB::CustomConfig>(cppInvocation, cppUninvocation)),
    priority);
}

static_assert(sizeof(CPPPerformanceInfo) == sizeof(WCDB::InnerHandle::PerformanceInfo), "");
static_assert(offsetof(CPPPerformanceInfo, tablePageReadCount)
              == offsetof(WCDB::InnerHandle::PerformanceInfo, tablePageReadCount),
              "");
static_assert(offsetof(CPPPerformanceInfo, tablePageWriteCount)
              == offsetof(WCDB::InnerHandle::PerformanceInfo, tablePageWriteCount),
              "");
static_assert(offsetof(CPPPerformanceInfo, indexPageReadCount)
              == offsetof(WCDB::InnerHandle::PerformanceInfo, indexPageReadCount),
              "");
static_assert(offsetof(CPPPerformanceInfo, indexPageWriteCount)
              == offsetof(WCDB::InnerHandle::PerformanceInfo, indexPageWriteCount),
              "");
static_assert(offsetof(CPPPerformanceInfo, overflowPageReadCount)
              == offsetof(WCDB::InnerHandle::PerformanceInfo, overflowPageReadCount),
              "");
static_assert(offsetof(CPPPerformanceInfo, overflowPageWriteCount)
              == offsetof(WCDB::InnerHandle::PerformanceInfo, overflowPageWriteCount),
              "");
static_assert(offsetof(CPPPerformanceInfo, costInNanoseconds)
              == offsetof(WCDB::InnerHandle::PerformanceInfo, costInNanoseconds),
              "");

void WCDBDatabaseGlobalTracePerformance(WCDBPerformanceTracer _Nullable tracer,
                                        void* _Nullable context,
                                        WCDBContextDestructor _Nullable destructor)
{
    WCDB::InnerHandle::PerformanceNotification callback = nullptr;
    if (tracer != nullptr) {
        WCDB::RecyclableContext recyclableContext(context, destructor);
        callback
        = [recyclableContext, tracer](const WCDB::Tag& tag,
                                      const WCDB::UnsafeStringView& path,
                                      const void* handle,
                                      const WCDB::UnsafeStringView& sql,
                                      const WCDB::InnerHandle::PerformanceInfo& info) {
              tracer(recyclableContext.get(),
                     tag,
                     path.data(),
                     (unsigned long long) handle,
                     sql.data(),
                     (const CPPPerformanceInfo*) &info);
          };
    }
    WCDB::Core::shared().setNotificationWhenPerformanceGlobalTraced(callback);
}

void WCDBDatabaseTracePerformance(CPPDatabase database,
                                  WCDBPerformanceTracer _Nullable tracer,
                                  void* _Nullable context,
                                  WCDBContextDestructor _Nullable destructor)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    if (tracer != nullptr) {
        WCDB::RecyclableContext recyclableContext(context, destructor);
        WCDB::InnerHandle::PerformanceNotification callback
        = [recyclableContext, tracer](const WCDB::Tag& tag,
                                      const WCDB::UnsafeStringView& path,
                                      const void* handle,
                                      const WCDB::UnsafeStringView& sql,
                                      const WCDB::InnerHandle::PerformanceInfo& info) {
              tracer(recyclableContext.get(),
                     tag,
                     path.data(),
                     (unsigned long long) handle,
                     sql.data(),
                     (const CPPPerformanceInfo*) &info);
          };
        cppDatabase->setConfig(WCDB::PerformanceTraceConfigName,
                               std::static_pointer_cast<WCDB::Config>(
                               std::make_shared<WCDB::PerformanceTraceConfig>(callback)),
                               WCDB::Configs::Priority::Highest);
    } else {
        cppDatabase->removeConfig(WCDB::PerformanceTraceConfigName);
    }
}

void WCDBDatabaseGlobalTraceSQL(WCDBSQLTracer _Nullable tracer,
                                void* _Nullable context,
                                WCDBContextDestructor _Nullable destructor)
{
    WCDB::InnerHandle::SQLNotification callback = nullptr;
    if (tracer != nullptr) {
        WCDB::RecyclableContext recyclableContext(context, destructor);
        callback = [recyclableContext, tracer](const WCDB::Tag& tag,
                                               const WCDB::UnsafeStringView& path,
                                               const void* handle,
                                               const WCDB::UnsafeStringView& sql,
                                               const WCDB::UnsafeStringView& info) {
            tracer(recyclableContext.get(),
                   tag,
                   path.data(),
                   (uint64_t) handle,
                   sql.data(),
                   info.data());
        };
    }
    WCDB::Core::shared().setNotificationForSQLGLobalTraced(callback);
}

void WCDBDatabaseTraceSQL(CPPDatabase database,
                          WCDBSQLTracer _Nullable tracer,
                          void* _Nullable context,
                          WCDBContextDestructor _Nullable destructor)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    if (tracer != nullptr) {
        WCDB::RecyclableContext recyclableContext(context, destructor);
        WCDB::InnerHandle::SQLNotification callback
        = [recyclableContext, tracer](const WCDB::Tag& tag,
                                      const WCDB::UnsafeStringView& path,
                                      const void* handle,
                                      const WCDB::UnsafeStringView& sql,
                                      const WCDB::UnsafeStringView& info) {
              tracer(recyclableContext.get(),
                     tag,
                     path.data(),
                     (uint64_t) handle,
                     sql.data(),
                     info.data());
          };
        cppDatabase->setConfig(WCDB::SQLTraceConfigName,
                               std::static_pointer_cast<WCDB::Config>(
                               std::make_shared<WCDB::SQLTraceConfig>(callback)),
                               WCDB::Configs::Priority::Highest);
    } else {
        cppDatabase->removeConfig(WCDB::SQLTraceConfigName);
    }
}

void WCDBDatabaseSetFullSQLTraceEnable(CPPDatabase database, bool enable)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    cppDatabase->setFullSQLTraceEnable(enable);
}

void WCDBDatabaseGlobalTraceError(WCDBErrorTracer _Nullable tracer,
                                  void* _Nullable context,
                                  WCDBContextDestructor _Nullable destructor)
{
    if (tracer != nullptr) {
        WCDB::RecyclableContext recyclableContext(context, destructor);
        WCDB::Core::shared().setNotificationWhenErrorTraced(
        [recyclableContext, tracer](const WCDB::Error& error) {
            tracer(recyclableContext.get(), WCDBCreateUnmanagedCPPObject(CPPError, &error));
        });
    } else {
        WCDB::Core::shared().setNotificationWhenErrorTraced(nullptr);
    }
}

void WCDBDatabaseTraceError(CPPDatabase database,
                            WCDBErrorTracer _Nullable tracer,
                            void* _Nullable context,
                            WCDBContextDestructor _Nullable destructor)
{
    const char* path = WCDBDatabaseGetPath(database);
    if (tracer != nullptr) {
        WCDB::RecyclableContext recyclableContext(context, destructor);
        WCDB::Core::shared().setNotificationWhenErrorTraced(
        path, [recyclableContext, tracer](const WCDB::Error& error) {
            tracer(recyclableContext.get(), WCDBCreateUnmanagedCPPObject(CPPError, &error));
        });
    } else {
        WCDB::Core::shared().setNotificationWhenErrorTraced(path, nullptr);
    }
}

#ifndef __ANDROID__
const char* _Nonnull WCDBDatabaseOperationTracerInfoKeyHandleCount
= WCDB::MonitorInfoKeyHandleCount.data();
const char* _Nonnull WCDBDatabaseOperationTracerInfoKeyHandleOpenTime
= WCDB::MonitorInfoKeyHandleOpenTime.data();
const char* _Nonnull WCDBDatabaseOperationTracerInfoKeyHandleOpenCPUTime
= WCDB::MonitorInfoKeyHandleOpenCPUTime.data();
const char* _Nonnull WCDBDatabaseOperationTracerInfoKeySchemaUsage
= WCDB::MonitorInfoKeySchemaUsage.data();
const char* _Nonnull WCDBDatabaseOperationTracerInfoKeyTableCount
= WCDB::MonitorInfoKeyTableCount.data();
const char* _Nonnull WCDBDatabaseOperationTracerInfoKeyIndexCount
= WCDB::MonitorInfoKeyIndexCount.data();
const char* _Nonnull WCDBDatabaseOperationTracerInfoKeyTriggerCount
= WCDB::MonitorInfoKeyTriggerCount.data();
#endif

void WCDBDatabaseGlobalTraceOperation(WCDBOperationTracer _Nullable tracer,
                                      void* _Nullable context,
                                      WCDBContextDestructor _Nullable destructor)
{
    if (tracer != nullptr) {
        WCDB::RecyclableContext recyclableContext(context, destructor);
        WCDB::DBOperationNotifier::shared().setNotification(
        [=](WCDB::InnerDatabase* database,
            WCDB::DBOperationNotifier::Operation operation,
            const WCDB::StringViewMap<WCDB::Value>& info) {
            CPPDatabase cppDatabase = WCDBCreateUnmanagedCPPObject(CPPDatabase, database);
            tracer(recyclableContext.get(), cppDatabase, operation, &info);
        });
    } else {
        WCDB::DBOperationNotifier::shared().setNotification(nullptr);
    }
}

bool WCDBDatabaseRemoveFile(CPPDatabase database)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, false);
    return cppDatabase->removeFiles();
}

bool WCDBDatabaseMoveFile(CPPDatabase database, const char* _Nonnull destination)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, false);
    return cppDatabase->moveFiles(WCDB::UnsafeStringView(destination));
}

OptionalUInt64 WCDBDatabaseGetFileSize(CPPDatabase database)
{
    OptionalUInt64 ret = { false, 0 };
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, ret);
    auto size = cppDatabase->getFilesSize();
    if (size.succeed()) {
        ret.hasValue = true;
        ret.value = size.value();
    }
    return ret;
}

void WCDBDatabaseSetNotificationWhenCorrupted(CPPDatabase database,
                                              WCDBCorruptioNotification _Nullable notification,
                                              void* _Nullable context,
                                              WCDBContextDestructor _Nullable destructor)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    WCDB::Core::CorruptedNotification cppNotification = nullptr;
    if (notification != nullptr) {
        WCDB::RecyclableContext recyclableContext(context, destructor);
        cppNotification = [notification, recyclableContext](WCDB::InnerDatabase* cppDatabase) {
            notification(recyclableContext.get(),
                         WCDBCreateUnmanagedCPPObject(CPPDatabase, cppDatabase));
        };
    }
    WCDB::Core::shared().setNotificationWhenDatabaseCorrupted(
    cppDatabase->getPath(), cppNotification);
}

bool WCDBDatabaseCheckIfCorrupted(CPPDatabase database)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, false);
    cppDatabase->checkIntegrity(false);
    return WCDBDatabaseCheckIsAlreadyCorrupted(database);
}

bool WCDBDatabaseCheckIsAlreadyCorrupted(CPPDatabase database)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, false);
    return WCDB::Core::shared().isFileObservedCorrupted(cppDatabase->getPath());
}

void WCDBDatabaseEnableAutoBackup(CPPDatabase database, bool enable)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    WCDB::Core::shared().enableAutoBackup(cppDatabase, enable);
}

bool WCDBDatabaseBackup(CPPDatabase database)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, false);
    return cppDatabase->backup(false);
}

void WCDBDatabaseFilterBackup(CPPDatabase database,
                              WCDBBackupFilter _Nullable filter,
                              void* _Nullable context,
                              WCDBContextDestructor _Nullable destructor)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    WCDB::InnerDatabase::BackupFilter cppFilter = nullptr;
    if (filter != nullptr) {
        WCDB::RecyclableContext recyclableContext(context, destructor);
        cppFilter
        = [filter, recyclableContext](const WCDB::UnsafeStringView& tableName) -> bool {
            return filter(recyclableContext.get(), tableName.data());
        };
    }
    cppDatabase->filterBackup(cppFilter);
}

bool WCDBDatabaseDeposit(CPPDatabase database)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, false);
    return cppDatabase->deposit();
}

bool WCDBDatabaseRemoveDepositedFiles(CPPDatabase database)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, false);
    return cppDatabase->removeDeposited();
}

bool WCDBDatabaseContainDepositedFiles(CPPDatabase database)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, false);
    return cppDatabase->containsDeposited();
}

double WCDBDatabaseRetrieve(CPPDatabase database,
                            WCDBProgressUpdate _Nullable monitor,
                            void* _Nullable context,
                            WCDBContextDestructor _Nullable destructor)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, false);
    WCDB::InnerDatabase::ProgressCallback callback = nullptr;
    if (monitor != nullptr) {
        WCDB::RecyclableContext recyclableContext(context, destructor);
        callback = [monitor, recyclableContext](double percentage, double increment) {
            monitor(recyclableContext.get(), percentage, increment);
            return true;
        };
    }
    return cppDatabase->retrieve(callback);
}

bool WCDBDatabaseVacuum(CPPDatabase database,
                        WCDBProgressUpdate _Nullable monitor,
                        void* _Nullable context,
                        WCDBContextDestructor _Nullable destructor)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, false);
    WCDB::InnerDatabase::ProgressCallback callback = nullptr;
    if (monitor != nullptr) {
        WCDB::RecyclableContext recyclableContext(context, destructor);
        callback = [monitor, recyclableContext](double percentage, double increment) {
            monitor(recyclableContext.get(), percentage, increment);
            return true;
        };
    }
    return cppDatabase->vacuum(callback);
}

bool WCDBDatabasePassiveCheckpoint(CPPDatabase database)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, false);
    return cppDatabase->checkpoint(false, WCDB::InnerDatabase::CheckPointMode::Passive);
}

bool WCDBDatabaseTruncateCheckpoint(CPPDatabase database)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, false);
    return cppDatabase->checkpoint(false, WCDB::InnerDatabase::CheckPointMode::Truncate);
}

void WCDBMigrationInfoSetterImp(WCDB::MigrationUserInfo* info,
                                const char* sourceTable,
                                CPPExpression expression)
{
    info->setSource(sourceTable);
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    info->setFilter(*cppExpression);
}

void WCDBDatabaseAddMigration(CPPDatabase database,
                              const char* _Nullable sourcePath,
                              const unsigned char* _Nullable sourceCipher,
                              int cipherLength,
                              WCDBMigrationFilter _Nullable filter,
                              void* _Nullable context,
                              WCDBContextDestructor _Nullable destructor)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    WCDB::InnerDatabase::MigrationTableFilter cppFilter = nullptr;
    if (filter != nullptr) {
        WCDB::RecyclableContext recyclableContext(context, destructor);
        cppFilter = [recyclableContext, filter](WCDB::MigrationUserInfo& info) {
            filter(recyclableContext.get(),
                   info.getTable().data(),
                   &info,
                   (WCDBMigrationInfoSetter) WCDBMigrationInfoSetterImp);
        };
    }
    cppDatabase->addMigration(
    sourcePath, WCDB::UnsafeData::immutable(sourceCipher, (size_t) cipherLength), cppFilter);
}

bool WCDBDatabaseStepMigration(CPPDatabase database)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, false);
    return (cppDatabase->stepMigration(false)).succeed();
}

void WCDBDatabaseEnableAutoMigration(CPPDatabase database, bool flag)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    WCDB::Core::shared().enableAutoMigrate(cppDatabase, flag);
}

void WCDBDatabaseSetNotificationWhenMigrated(CPPDatabase database,
                                             WCDBMigrationNotification _Nullable notification,
                                             void* _Nullable context,
                                             WCDBContextDestructor _Nullable destructor)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    WCDB::InnerDatabase::MigratedCallback callback = nullptr;
    if (notification != nullptr) {
        WCDB::RecyclableContext recyclableContext(context, destructor);
        callback = [notification, recyclableContext](
                   WCDB::InnerDatabase* cppDatabase, const WCDB::MigrationBaseInfo* baseInfo) {
            CPPDatabase database = WCDBCreateUnmanagedCPPObject(CPPDatabase, cppDatabase);
            if (baseInfo != nullptr) {
                notification(recyclableContext.get(),
                             database,
                             baseInfo->getTable().data(),
                             baseInfo->getSourceTable().data());
            } else {
                notification(recyclableContext.get(), database, nullptr, nullptr);
            }
        };
    }
    cppDatabase->setNotificationWhenMigrated(callback);
}

bool WCDBDatabaseIsMigrated(CPPDatabase database)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, false);
    return cppDatabase->isMigrated();
}

CPPData WCDBDatabaseTrainDict(unsigned char dictId,
                              WCDBDataEnumerator _Nonnull dataEnumerator,
                              void* _Nonnull enumerateContext)
{
    auto dict = WCDB::CompressionCenter::shared().trainDict(
    dictId, [&]() -> WCDB::Optional<WCDB::UnsafeData> {
        CPPData data = dataEnumerator(enumerateContext);
        if (data.buffer != nullptr) {
            return WCDB::UnsafeData((unsigned char*) data.buffer, data.size);
        } else {
            return WCDB::NullOpt;
        }
    });
    if (dict.hasValue()) {
        size_t size = dict.value().size();
        unsigned char* buffer = (unsigned char*) malloc(size);
        if (buffer != nullptr) {
            memcpy(buffer, dict.value().buffer(), size);
            return { buffer, size };
        }
    }
    return { nullptr, 0 };
}

bool WCDBDatabaseRegisterDict(const unsigned char* _Nullable dict, long dictSize, unsigned char dictId)
{
    return WCDB::CompressionCenter::shared().registerDict(
    dictId, WCDB::UnsafeData((unsigned char*) dict, dictSize));
}

void WCDBDatabaseSetZSTDNormalCompress(void* _Nonnull context, CPPColumn column)
{
    WCDBGetObjectOrReturn(column, WCDB::Column, cppColumn);
    WCDB::CompressionColumnInfo columnInfo(*cppColumn, WCDB::CompressionType::Normal);
    WCDB::CompressionTableUserInfo* userInfo = (WCDB::CompressionTableUserInfo*) context;
    userInfo->addCompressingColumn(columnInfo);
}

void WCDBDatabaseSetZSTDDictCompress(void* _Nonnull context, CPPColumn column, unsigned char dictid)
{
    WCDBGetObjectOrReturn(column, WCDB::Column, cppColumn);
    WCDB::CompressionColumnInfo columnInfo(*cppColumn, WCDB::CompressionType::Dict);
    columnInfo.setCommonDict(dictid);
    WCDB::CompressionTableUserInfo* userInfo = (WCDB::CompressionTableUserInfo*) context;
    userInfo->addCompressingColumn(columnInfo);
}

void WCDBDatabaseSetZSTDMultiDictCompress(void* _Nonnull context,
                                          CPPColumn column,
                                          CPPColumn matchColumn,
                                          const long long* _Nullable values,
                                          const unsigned char* _Nullable dictIds,
                                          int dictCount)
{
    WCDBGetObjectOrReturn(column, WCDB::Column, cppColumn);
    WCDBGetObjectOrReturn(matchColumn, WCDB::Column, cppMatchColumn);
    WCDB::CompressionColumnInfo columnInfo(*cppColumn, *cppMatchColumn);
    for (int i = 0; i < dictCount; i++) {
        if (values[i] == INT64_MAX) {
            columnInfo.setCommonDict(dictIds[i]);
        } else {
            columnInfo.addMatchDict(values[i], dictIds[i]);
        }
    }
    WCDB::CompressionTableUserInfo* userInfo = (WCDB::CompressionTableUserInfo*) context;
    userInfo->addCompressingColumn(columnInfo);
}

void WCDBDatabaseSetCompression(CPPDatabase database,
                                WCDBCompressionFilter _Nullable filter,
                                void* _Nullable context,
                                WCDBContextDestructor _Nullable destructor)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    WCDB::InnerDatabase::CompressionTableFilter cppFilter = nullptr;
    if (filter != nullptr) {
        WCDB::RecyclableContext recyclableContext(context, destructor);
        cppFilter = [recyclableContext, filter](WCDB::CompressionTableUserInfo& userInfo) {
            filter(recyclableContext.get(), userInfo.getTable().data(), &userInfo);
        };
        WCDB::StringView configName
        = WCDB::StringView::formatted("%s%s",
                                      WCDB::ScalarFunctionConfigPrefix.data(),
                                      WCDB::DecompressFunctionName.data());
        cppDatabase->setConfig(
        configName,
        WCDB::Core::shared().scalarFunctionConfig(WCDB::DecompressFunctionName),
        WCDB::Configs::Priority::Higher);
    }
    cppDatabase->addCompression(cppFilter);
}

void WCDBDatabaseDisableCompressNewData(CPPDatabase database, bool disable)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    cppDatabase->setCanCompressNewData(!disable);
}

bool WCDBDatabaseStepCompression(CPPDatabase database)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, false);
    return cppDatabase->stepCompression(false).succeed();
}

void WCDBDatabaseEnableAutoCompression(CPPDatabase database, bool enable)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    WCDB::Core::shared().enableAutoCompress(cppDatabase, enable);
}

bool WCDBDatabaseIsCompressed(CPPDatabase database)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, false);
    return cppDatabase->isCompressed();
}

void WCDBDatabaseSetNotificationWhenCompressed(CPPDatabase database,
                                               WCDBCompressdNotification _Nullable notification,
                                               void* _Nullable context,
                                               WCDBContextDestructor _Nullable destructor)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    WCDB::InnerDatabase::CompressedCallback callback = nullptr;
    if (notification != nullptr) {
        WCDB::RecyclableContext recyclableContext(context, destructor);
        callback = [notification, recyclableContext](
                   WCDB::InnerDatabase* cppDatabase,
                   const WCDB::CompressionTableBaseInfo* info) {
            CPPDatabase database = WCDBCreateUnmanagedCPPObject(CPPDatabase, cppDatabase);
            if (info != nullptr) {
                notification(recyclableContext.get(), database, info->getTable().data());
            } else {
                notification(recyclableContext.get(), database, nullptr);
            }
        };
    }
    cppDatabase->setNotificationWhenCompressed(callback);
}

short WCDBDatabaseGetAliveHandleCount(CPPDatabase database)
{
    WCDBGetObjectOrReturnValue(database, WCDB::InnerDatabase, cppDatabase, 0);
    return (short) cppDatabase->numberOfAliveHandles();
}
