//
// Created by sanhuazhang on 2019/05/02
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

#include "Macro.h"
#include "StringView.hpp"

namespace WCDB {

#pragma mark - Async Queue
static constexpr const double AsyncQueueTimeOutForExiting = 10.0;

#pragma mark - Operation Queue
WCDBLiteralStringDefine(OperationQueueName, "WCDB.Operation");
static constexpr double OperationQueueTimeIntervalForRetringAfterFailure = 5.0;
#pragma mark - Operation Queue - Migration
static constexpr const double OperationQueueTimeIntervalForMigration = 2.0;
static constexpr const int OperationQueueTolerableFailuresForMigration = 5;
#pragma mark - Operation Queue - Purge
static constexpr const double OperationQueueTimeIntervalForPurgingAgain = 30.0;
static constexpr const double OperationQueueRateForTooManyFileDescriptors = 0.7;
#pragma mark - Operation Queue - Checkpoint
static constexpr const double OperationQueueTimeIntervalForCheckpoint = 10.0;
#pragma mark - Operation Queue - Backup
#ifndef WCDB_QUICK_TESTS
static double OperationQueueTimeIntervalForBackup = 600.0;
#else
static double OperationQueueTimeIntervalForBackup = 10.0;
#endif
#pragma mark - Operation Queue - Merge FTS Index
static constexpr const double OperationQueueTimeIntervalForMergeFTSIndex
= 1.871; //Use prime numbers to reduce the probability of collision with external logic

#pragma mark - Config - Auto Checkpoint
WCDBLiteralStringDefine(AutoCheckpointConfigName, "com.Tencent.WCDB.Config.AutoCheckpoint");
#pragma mark - Config - Auto Backup
WCDBLiteralStringDefine(AutoBackupConfigName, "com.Tencent.WCDB.Config.AutoBackup");
#pragma mark - Config - Auto Migrate
WCDBLiteralStringDefine(AutoMigrateConfigName, "com.Tencent.WCDB.Config.AutoMigrate");
#pragma mark - Config - Auto Merge
WCDBLiteralStringDefine(AutoMergeFTSIndexConfigName, "com.Tencent.WCDB.Config.AutoMergeFTSIndex");
WCDBLiteralStringDefine(AutoMergeFTSIndexQueueName, "WCDB.MergeIndex");
static constexpr const int AutoMergeFTS5IndexMinSegmentCount = 4;
static constexpr const double AutoMergeFTSIndexMaxExpectingDuration = 0.02;
static constexpr const double AutoMergeFTSIndexMaxInitializeDuration = 0.005;
#pragma mark - Config - Basic
WCDBLiteralStringDefine(BasicConfigName, "com.Tencent.WCDB.Config.Basic");
static constexpr const int BasicConfigBusyRetryMaxAllowedNumberOfTimes = 3;
#pragma mark - Config - Busy Retry
WCDBLiteralStringDefine(BusyRetryConfigName, "com.Tencent.WCDB.Config.BusyRetry");
static constexpr const double BusyRetryTimeOut = 10.0;
#pragma mark - Config - Cipher
WCDBLiteralStringDefine(CipherConfigName, "com.Tencent.WCDB.Config.Cipher");
static constexpr const int CipherConfigDefaultPageSize = SQLITE_DEFAULT_PAGE_SIZE;
#pragma mark - Config - Global SQL Trace
WCDBLiteralStringDefine(GlobalSQLTraceConfigName, "com.Tencent.WCDB.Config.GlobalSQLTrace");
#pragma mark - Config - Global Performance Trace
WCDBLiteralStringDefine(GlobalPerformanceTraceConfigName,
                        "com.Tencent.WCDB.Config.GlobalPerformanceTrace");
#pragma mark - Config - Performance Trace
WCDBLiteralStringDefine(PerformanceTraceConfigName, "com.Tencent.WCDB.Config.PerformanceTrace");
#pragma mark - Config - SQL Trace
WCDBLiteralStringDefine(SQLTraceConfigName, "com.Tencent.WCDB.Config.SQLTrace");
#pragma mark - Config - Tokenize
WCDBLiteralStringDefine(TokenizeConfigPrefix, "com.Tencent.WCDB.Config.Tokenize.");
#pragma mark - Config - AuxiliaryFunction
WCDBLiteralStringDefine(AuxiliaryFunctionConfigPrefix,
                        "com.Tencent.WCDB.Config.AuxiliaryFunction.");

#pragma mark - Notifier
WCDBLiteralStringDefine(NotifierPreprocessorName, "com.Tencent.WCDB.Notifier.PreprocessTag");
WCDBLiteralStringDefine(NotifierLoggerName, "com.Tencent.WCDB.Notifier.Log");

#pragma mark - Handle Pool
static constexpr const int HandlePoolMaxAllowedNumberOfHandles = 32;
static constexpr const int HandlePoolMaxAllowedNumberOfWriters = 4;

enum HandleSlot : unsigned char {
    HandleSlotNormal = 0,
    HandleSlotMigrating,
    HandleSlotMigrate,
    HandleSlotCheckPoint,
    HandleSlotOperation,
    HandleSlotAssemble,
    HandleSlotCount,
};
enum HandleCategory : unsigned char {
    HandleCategoryNormal = 0,
    HandleCategoryMigrating,
    HandleCategoryMigrate,
    HandleCategoryBackupRead,
    HandleCategoryBackupWrite,
    HandleCategoryCheckpoint,
    HandleCategoryIntegrity,
    HandleCategoryAssemble,
    HandleCategoryCipher,
    HandleCategoryMergeIndex,
    HandleCategoryCount,
};

enum class HandleType : unsigned int {
    Normal = (HandleCategoryNormal << 8) | HandleSlotNormal,
    Migrating = (HandleCategoryMigrating << 8) | HandleSlotMigrating,
    Migrate = (HandleCategoryMigrate << 8) | HandleSlotMigrate,
    BackupRead = (HandleCategoryBackupRead << 8) | HandleSlotOperation,
    BackupWrite = (HandleCategoryBackupWrite << 8) | HandleSlotOperation,
    BackupCipher = (HandleCategoryCipher << 8) | HandleSlotOperation,
    Checkpoint = (HandleCategoryCheckpoint << 8) | HandleSlotCheckPoint,
    Integrity = (HandleCategoryIntegrity << 8) | HandleSlotOperation,
    Assemble = (HandleCategoryAssemble << 8) | HandleSlotAssemble,
    AssembleCipher = (HandleCategoryCipher << 8) | HandleSlotAssemble,
    AssembleBackupRead = (HandleCategoryBackupRead << 8) | HandleSlotAssemble,
    AssembleBackupWrite = (HandleCategoryBackupWrite << 8) | HandleSlotAssemble,
    MergeIndex = (HandleCategoryMergeIndex << 8) | HandleSlotOperation,
};
static constexpr HandleSlot slotOfHandleType(HandleType type)
{
    return HandleSlot(((unsigned int) type) & 0xff);
}
static constexpr HandleCategory categoryOfHandleType(HandleType type)
{
    return HandleCategory(((unsigned int) type >> 8) & 0xff);
}
static constexpr bool handleShouldWaitWhenFull(HandleType type)
{
    HandleSlot slot = slotOfHandleType(type);
    return slot == HandleSlotNormal || slot == HandleSlotMigrating;
}

#pragma mark - Migrate
static constexpr const double MigrateMaxExpectingDuration = 0.01;
static constexpr const double MigrateMaxInitializeDuration = 0.005;

WCDBLiteralStringDefine(ErrorStringKeyType, "Type");
WCDBLiteralStringDefine(ErrorStringKeySource, "Source")

#define WCDB_ERROR_STRING_KEY_PATH "Path"
WCDBLiteralStringDefine(ErrorStringKeyPath, WCDB_ERROR_STRING_KEY_PATH);

#define WCDB_ERROR_STRING_KEY_ASSOCIATE_PATH "AssociatePath"
WCDBLiteralStringDefine(ErrorStringKeyAssociatePath, WCDB_ERROR_STRING_KEY_ASSOCIATE_PATH);

#define WCDB_ERROR_STRING_KEY_SQL "SQL"
WCDBLiteralStringDefine(ErrorStringKeySQL, WCDB_ERROR_STRING_KEY_SQL);

#define WCDB_ERROR_INT_KEY_TAG "Tag"
WCDBLiteralStringDefine(ErrorIntKeyTag, WCDB_ERROR_INT_KEY_TAG);

#define WCDB_ERROR_INT_KEY_EXTCODE "ExtCode"
WCDBLiteralStringDefine(ErrorIntKeyExtCode, WCDB_ERROR_INT_KEY_EXTCODE);

#pragma mark - Error - Source
WCDBLiteralStringDefine(ErrorSourceSQLite, "SQLite");
WCDBLiteralStringDefine(ErrorSourceRepair, "Repair");
WCDBLiteralStringDefine(ErrorSourceSystem, "System");
WCDBLiteralStringDefine(ErrorSourceAssertion, "Assertion");
WCDBLiteralStringDefine(ErrorSourceNative, "Native");
WCDBLiteralStringDefine(ErrorSourceSwift, "Swift");

#pragma mark - Error - Type
WCDBLiteralStringDefine(ErrorTypeMigrate, "Migrate");
WCDBLiteralStringDefine(ErrorTypeCheckpoint, "Checkpoint");
WCDBLiteralStringDefine(ErrorTypeIntegrity, "Integrity");
WCDBLiteralStringDefine(ErrorTypeBackup, "Backup")
WCDBLiteralStringDefine(ErrorTypeAssemble, "Assemble");
WCDBLiteralStringDefine(ErrorTypeMergeIndex, "MergeIndex")

#pragma mark - Moniter
WCDBLiteralStringDefine(MonitorInfoKeyHandleCount, "HandleCount");
WCDBLiteralStringDefine(MonitorInfoKeyHandleOpenTime, "OpenTime");
WCDBLiteralStringDefine(MonitorInfoKeySchemaUsage, "SchemaUsage");
WCDBLiteralStringDefine(MonitorInfoKeyTableCount, "TableCount");
WCDBLiteralStringDefine(MonitorInfoKeyIndexCount, "IndexCount");
WCDBLiteralStringDefine(MonitorInfoKeyTriggerCount, "TriggerCount");

#pragma mark - Tag
static constexpr const int TagInvalidValue = 0;

#pragma mark - Constraint
static_assert(OperationQueueTimeIntervalForMigration > MigrateMaxExpectingDuration, "");

} // namespace WCDB
