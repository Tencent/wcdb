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

#ifndef __WCDB_CORE_CONST_H
#define __WCDB_CORE_CONST_H

namespace WCDB {

// Async Queue
static constexpr const double AsyncQueueTimeOutForExiting = 10.0;

// Migration Queue
static constexpr const char* MigrationQueueName = "com.Tencent.WCDB.Queue.Migration";
static constexpr const double MigrationQueueTimeIntervalForMigrating = 1.0;
static constexpr const double MigrationQueueTimeIntervalForRetryingAfterFailure = 3.0;
static constexpr const int MigrationQueueTolerableFailures = 3;

// Observation Queue
static constexpr const char* ObservationQueueName = "com.Tencent.WCDB.Queue.Observation";
static constexpr const double ObservationQueueTimeIntervalForReinvokingCorruptedEvent = 5.0;
static constexpr const double ObservationQueueTimeIntervalForPurgingAgain = 10.0;
static constexpr const double ObservationQueueRateForTooManyFileDescriptors = 0.5;
static constexpr const int ObservationQueueTimesOfIgnoringBackupCorruption = 3;

// Checkpoint Queue
static constexpr const char* CheckpointQueueName = "com.Tencent.WCDB.Queue.Checkpoint";
static constexpr const double CheckpointQueueTimeIntervalForRetryingAfterFailure = 10.0;
static constexpr const int CheckpointQueueFramesThresholdForCritical = 200;
static constexpr const double CheckpointQueueTimeIntervalForCritical = 1.0;
static constexpr const double CheckpointQueueTimeIntervalForNonCritical = 10.0;

// Backup Queue
static constexpr const char* BackupQueueName = "com.Tencent.WCDB.Queue.Backup";
static constexpr double BackupQueueTimeIntervalForRetryingAfterFailure = 15.0;

// Checkpoint Config
static constexpr const char* CheckpointConfigName = "com.Tencent.WCDB.Config.Checkpoint";

// Backup Config
static constexpr const char* BackupConfigName = "com.Tencent.WCDB.Config.Backup";
static constexpr const int BackupConfigFramesIntervalForCritical = 300;
static constexpr const int BackupConfigFramesIntervalForNonCritical = 100;
static constexpr const double BackupQueueTimeIntervalForCritical = 0;
static constexpr const double BackupQueueTimeIntervalForNonCritical = 5.0;

// BackupConfigFramesIntervalForCritical should be greater than CheckpointQueueFramesThresholdForCritical since a checkpoint will trigger critical checkpoint too.
static_assert(BackupConfigFramesIntervalForCritical > CheckpointQueueFramesThresholdForCritical,
              "");

// Basic Config
static constexpr const char* BasicConfigName = "com.Tencent.WCDB.Config.Basic";
static constexpr const int BasicConfigBusyRetryMaxAllowedNumberOfTimes = 3;

// Tokenize Config
static constexpr const char* TokenizeConfigPrefix = "com.Tencent.WCDB.Config.Tokenize.";

// Cipher Config
static constexpr const char* CipherConfigName = "com.Tencent.WCDB.Config.Cipher";
static constexpr const int CipherConfigDefaultPageSize = 4096;

// SQL Trace Config
static constexpr const char* SQLTraceConfigName = "com.Tencent.WCDB.Config.SQLTrace";

// Performance Trace Config
static constexpr const char* PerformanceTraceConfigName
= "com.Tencent.WCDB.Config.PerformanceTrace";

// Global SQL Trace Config
static constexpr const char* GlobalSQLTraceConfigName = "com.Tencent.WCDB.Config.GlobalSQLTrace";

// Global Performance Trace Config
static constexpr const char* GlobalPerformanceTraceConfigName
= "com.Tencent.WCDB.Config.GlobalPerformanceTrace";

// Busy Retry Config
static constexpr const char* BusyRetryConfigName = "com.Tencent.WCDB.Config.BusyRetry";
static constexpr const double BusyRetryTimeOutForMainThread = 2.0;
static constexpr const double BusyRetryTimeOutForSubThread = 10.0;

// Notifier
static constexpr const char* NotifierPreprocessorName = "com.Tencent.WCDB.Notifier.PreprocessTag";
static constexpr const char* NotifierLoggerName = "com.Tencent.WCDB.Notifier.Log";

// Handle Pool
static constexpr const int HandlePoolMaxAllowedNumberOfHandles = 32;
static constexpr const unsigned int HandlePoolNumberOfSlots = 8;

// Checkpoint
static constexpr const int CheckpointFramesThresholdForTruncating
= 10 * 1024 * 1024 / 4096; // 10 MB / Default Page Size = 2560

// Error Key
#define WCDB_ERROR_STRING_KEY_SOURCE "Source";
static constexpr const char* ErrorStringKeySource = WCDB_ERROR_STRING_KEY_SOURCE;

#define WCDB_ERROR_STRING_KEY_PATH "Path"
static constexpr const char* ErrorStringKeyPath = WCDB_ERROR_STRING_KEY_PATH;

#define WCDB_ERROR_STRING_KEY_SQL "SQL"
static constexpr const char* ErrorStringKeySQL = WCDB_ERROR_STRING_KEY_SQL;

#define WCDB_ERROR_INT_KEY_TAG "Tag"
static constexpr const char* ErrorIntKeyTag = WCDB_ERROR_INT_KEY_TAG;

#define WCDB_ERROR_INT_KEY_EXTCODE "ExtCode"
static constexpr const char* ErrorIntKeyExtCode = WCDB_ERROR_INT_KEY_EXTCODE;

static constexpr const char* ErrorSourceSQLite = "SQLite";
static constexpr const char* ErrorSourceRepair = "Repair";
static constexpr const char* ErrorSourceSystem = "System";
static constexpr const char* ErrorSourceAssertion = "Assertion";
static constexpr const char* ErrorSourceTrace = "Trace";
static constexpr const char* ErrorSourceNative = "Native";

// Tag
static constexpr const int TagInvalidValue = 0;

} // namespace WCDB

#endif /* __WCDB_CORE_CONST_H */
