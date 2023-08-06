//
// Created by qiuwenchen on 2023/8/6.
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

#include "CoreConst.h"

namespace WCDB {

WCDBLiteralStringImpletement(OperationQueueName, "com.Tencent.WCDB.Queue.Operation")

WCDBLiteralStringImpletement(AutoCheckpointConfigName, "com.Tencent.WCDB.Config.AutoCheckpoint")

WCDBLiteralStringImpletement(AutoBackupConfigName, "com.Tencent.WCDB.Config.AutoBackup")

WCDBLiteralStringImpletement(AutoMigrateConfigName, "com.Tencent.WCDB.Config.AutoMigrate")

WCDBLiteralStringImpletement(AutoMergeFTSIndexConfigName, "com.Tencent.WCDB.Config.AutoMergeFTSIndex")

WCDBLiteralStringImpletement(AutoMergeFTSIndexQueueName, "com.Tencent.WCDB.Queue.MergeIndex")

WCDBLiteralStringImpletement(BasicConfigName, "com.Tencent.WCDB.Config.Basic")

WCDBLiteralStringImpletement(BusyRetryConfigName, "com.Tencent.WCDB.Config.BusyRetry")

WCDBLiteralStringImpletement(CipherConfigName, "com.Tencent.WCDB.Config.Cipher")

WCDBLiteralStringImpletement(GlobalSQLTraceConfigName, "com.Tencent.WCDB.Config.GlobalSQLTrace")

WCDBLiteralStringImpletement(GlobalPerformanceTraceConfigName,
                             "com.Tencent.WCDB.Config.GlobalPerformanceTrace")

WCDBLiteralStringImpletement(PerformanceTraceConfigName, "com.Tencent.WCDB.Config.PerformanceTrace")

WCDBLiteralStringImpletement(SQLTraceConfigName, "com.Tencent.WCDB.Config.SQLTrace")

WCDBLiteralStringImpletement(TokenizeConfigPrefix, "com.Tencent.WCDB.Config.Tokenize.")

WCDBLiteralStringImpletement(AuxiliaryFunctionConfigPrefix, "com.Tencent.WCDB.Config.AuxiliaryFunction.")

WCDBLiteralStringImpletement(NotifierPreprocessorName, "com.Tencent.WCDB.Notifier.PreprocessTag")

WCDBLiteralStringImpletement(NotifierLoggerName, "com.Tencent.WCDB.Notifier.Log")

WCDBLiteralStringImpletement(ErrorStringKeyType, "Type")
WCDBLiteralStringImpletement(ErrorStringKeySource, "Source")
WCDBLiteralStringImpletement(ErrorStringKeyPath, WCDB_ERROR_STRING_KEY_PATH)
WCDBLiteralStringImpletement(ErrorStringKeyAssociatePath, WCDB_ERROR_STRING_KEY_ASSOCIATE_PATH)
WCDBLiteralStringImpletement(ErrorStringKeySQL, WCDB_ERROR_STRING_KEY_SQL)
WCDBLiteralStringImpletement(ErrorIntKeyTag, WCDB_ERROR_INT_KEY_TAG)
WCDBLiteralStringImpletement(ErrorIntKeyExtCode, WCDB_ERROR_INT_KEY_EXTCODE)

WCDBLiteralStringImpletement(ErrorSourceSQLite, "SQLite")
WCDBLiteralStringImpletement(ErrorSourceRepair, "Repair")
WCDBLiteralStringImpletement(ErrorSourceSystem, "System")
WCDBLiteralStringImpletement(ErrorSourceAssertion, "Assertion")
WCDBLiteralStringImpletement(ErrorSourceNative, "Native")
WCDBLiteralStringImpletement(ErrorSourceSwift, "Swift")

WCDBLiteralStringImpletement(ErrorTypeMigrate, "Migrate")
WCDBLiteralStringImpletement(ErrorTypeCheckpoint, "Checkpoint")
WCDBLiteralStringImpletement(ErrorTypeIntegrity, "Integrity")
WCDBLiteralStringImpletement(ErrorTypeBackup, "Backup")
WCDBLiteralStringImpletement(ErrorTypeAssemble, "Assemble")
WCDBLiteralStringImpletement(ErrorTypeMergeIndex, "MergeIndex")

WCDBLiteralStringImpletement(MonitorInfoKeyHandleCount, "HandleCount")
WCDBLiteralStringImpletement(MonitorInfoKeyHandleOpenTime, "OpenTime")
WCDBLiteralStringImpletement(MonitorInfoKeySchemaUsage, "SchemaUsage")
WCDBLiteralStringImpletement(MonitorInfoKeyTableCount, "TableCount")
WCDBLiteralStringImpletement(MonitorInfoKeyIndexCount, "IndexCount")
WCDBLiteralStringImpletement(MonitorInfoKeyTriggerCount, "TriggerCount")

};
