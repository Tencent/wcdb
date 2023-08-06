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
WCDBLiteralStringDefine(OperationQueueName) static constexpr double OperationQueueTimeIntervalForRetringAfterFailure = 5.0;
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
WCDBLiteralStringDefine(AutoCheckpointConfigName)
#pragma mark - Config - Auto Backup
WCDBLiteralStringDefine(AutoBackupConfigName)
#pragma mark - Config - Auto Migrate
WCDBLiteralStringDefine(AutoMigrateConfigName)
#pragma mark - Config - Auto Merge
WCDBLiteralStringDefine(AutoMergeFTSIndexConfigName)
WCDBLiteralStringDefine(AutoMergeFTSIndexQueueName) static constexpr const
int AutoMergeFTS5IndexMinSegmentCount = 4;
static constexpr const double AutoMergeFTSIndexMaxExpectingDuration = 0.02;
static constexpr const double AutoMergeFTSIndexMaxInitializeDuration = 0.005;
#pragma mark - Config - Basic
WCDBLiteralStringDefine(BasicConfigName) static constexpr const
int BasicConfigBusyRetryMaxAllowedNumberOfTimes = 3;
#pragma mark - Config - Busy Retry
WCDBLiteralStringDefine(BusyRetryConfigName) static constexpr const
double BusyRetryTimeOut = 10.0;
#pragma mark - Config - Cipher
WCDBLiteralStringDefine(CipherConfigName) static constexpr const
int CipherConfigDefaultPageSize = SQLITE_DEFAULT_PAGE_SIZE;
#pragma mark - Config - Global SQL Trace
WCDBLiteralStringDefine(GlobalSQLTraceConfigName)
#pragma mark - Config - Global Performance Trace
WCDBLiteralStringDefine(GlobalPerformanceTraceConfigName)
#pragma mark - Config - Performance Trace
WCDBLiteralStringDefine(PerformanceTraceConfigName)
#pragma mark - Config - SQL Trace
WCDBLiteralStringDefine(SQLTraceConfigName)
#pragma mark - Config - Tokenize
WCDBLiteralStringDefine(TokenizeConfigPrefix)
#pragma mark - Config - AuxiliaryFunction
WCDBLiteralStringDefine(AuxiliaryFunctionConfigPrefix)

#pragma mark - Notifier
WCDBLiteralStringDefine(NotifierPreprocessorName)
WCDBLiteralStringDefine(NotifierLoggerName)

#pragma mark - Handle Pool
static constexpr const int HandlePoolMaxAllowedNumberOfHandles = 32;
enum HandleSlot : unsigned int {
    HandleSlotNormal = 0,
    HandleSlotMigrating,
    HandleSlotMigrate,
    HandleSlotCheckPoint,
    HandleSlotOperation,
    HandleSlotAssemble,
    HandleSlotCount,
};
enum HandleCategory : unsigned int {
    HandleCategoryNormal = 0,
    HandleCategoryMigrating,
    HandleCategoryMigrate,
    HandleCategoryBackupRead,
    HandleCategoryBackupWrite,
    HandleCategoryCheckpoint,
    HandleCategoryIntegrity,
    HandleCategoryAssemble,
    HandleCategoryCipher,
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
};
static constexpr HandleSlot slotOfHandleType(HandleType type)
{
    return HandleSlot(((unsigned int) type) & 0xff);
}
static constexpr HandleCategory categoryOfHandleType(HandleType type)
{
    return HandleCategory(((unsigned int) type >> 8) & 0xff);
}

#pragma mark - Migrate
static constexpr const double MigrateMaxExpectingDuration = 0.01;
static constexpr const double MigrateMaxInitializeDuration = 0.005;

WCDBLiteralStringDefine(ErrorStringKeyType) WCDBLiteralStringDefine(ErrorStringKeySource)

#define WCDB_ERROR_STRING_KEY_PATH "Path"
WCDBLiteralStringDefine(ErrorStringKeyPath)

#define WCDB_ERROR_STRING_KEY_ASSOCIATE_PATH "AssociatePath"
WCDBLiteralStringDefine(ErrorStringKeyAssociatePath)

#define WCDB_ERROR_STRING_KEY_SQL "SQL"
WCDBLiteralStringDefine(ErrorStringKeySQL)

#define WCDB_ERROR_INT_KEY_TAG "Tag"
WCDBLiteralStringDefine(ErrorIntKeyTag)

#define WCDB_ERROR_INT_KEY_EXTCODE "ExtCode"
WCDBLiteralStringDefine(ErrorIntKeyExtCode)

#pragma mark - Error - Source
WCDBLiteralStringDefine(ErrorSourceSQLite) WCDBLiteralStringDefine(ErrorSourceRepair)
WCDBLiteralStringDefine(ErrorSourceSystem) WCDBLiteralStringDefine(ErrorSourceAssertion)
WCDBLiteralStringDefine(ErrorSourceNative) WCDBLiteralStringDefine(ErrorSourceSwift)

#pragma mark - Error - Type
WCDBLiteralStringDefine(ErrorTypeMigrate) WCDBLiteralStringDefine(ErrorTypeCheckpoint)
WCDBLiteralStringDefine(ErrorTypeIntegrity) WCDBLiteralStringDefine(ErrorTypeBackup)
WCDBLiteralStringDefine(ErrorTypeAssemble) WCDBLiteralStringDefine(ErrorTypeMergeIndex)

#pragma mark - Moniter
WCDBLiteralStringDefine(MonitorInfoKeyHandleCount)
WCDBLiteralStringDefine(MonitorInfoKeyHandleOpenTime)
WCDBLiteralStringDefine(MonitorInfoKeySchemaUsage)
WCDBLiteralStringDefine(MonitorInfoKeyTableCount)
WCDBLiteralStringDefine(MonitorInfoKeyIndexCount)
WCDBLiteralStringDefine(MonitorInfoKeyTriggerCount)

#pragma mark - Tag
static constexpr const int TagInvalidValue = 0;

#pragma mark - Constraint
static_assert(OperationQueueTimeIntervalForMigration > MigrateMaxExpectingDuration, "");

} // namespace WCDB
