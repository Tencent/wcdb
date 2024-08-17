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

WCDBLiteralStringImplement(OperationQueueName);

WCDBLiteralStringImplement(AutoCheckpointConfigName);

WCDBLiteralStringImplement(AutoBackupConfigName);

WCDBLiteralStringImplement(AutoMigrateConfigName);

WCDBLiteralStringImplement(AutoCompressConfigName);

WCDBLiteralStringImplement(AutoMergeFTSIndexConfigName);

WCDBLiteralStringImplement(AutoMergeFTSIndexQueueName);

WCDBLiteralStringImplement(BasicConfigName);

WCDBLiteralStringImplement(BusyRetryConfigName);

WCDBLiteralStringImplement(CipherConfigName);

WCDBLiteralStringImplement(GlobalSQLTraceConfigName);

WCDBLiteralStringImplement(GlobalPerformanceTraceConfigName);

WCDBLiteralStringImplement(PerformanceTraceConfigName);

WCDBLiteralStringImplement(SQLTraceConfigName);

WCDBLiteralStringImplement(TokenizeConfigPrefix);

WCDBLiteralStringImplement(AuxiliaryFunctionConfigPrefix);

WCDBLiteralStringImplement(ScalarFunctionConfigPrefix);

WCDBLiteralStringImplement(AutoVacuumConfigName);

WCDBLiteralStringImplement(NotifierPreprocessorName);

WCDBLiteralStringImplement(NotifierLoggerName);

WCDBLiteralStringImplement(ErrorStringKeyType);
WCDBLiteralStringImplement(ErrorStringKeySource);
WCDBLiteralStringImplement(ErrorStringKeyPath);
WCDBLiteralStringImplement(ErrorStringKeyAssociatePath);
WCDBLiteralStringImplement(ErrorStringKeySQL);
WCDBLiteralStringImplement(ErrorIntKeyTag);
WCDBLiteralStringImplement(ErrorIntKeyExtCode);

WCDBLiteralStringImplement(ErrorSourceSQLite);
WCDBLiteralStringImplement(ErrorSourceRepair);
WCDBLiteralStringImplement(ErrorSourceSystem);
WCDBLiteralStringImplement(ErrorSourceAssertion);
WCDBLiteralStringImplement(ErrorSourceNative);
WCDBLiteralStringImplement(ErrorSourceSwift);
WCDBLiteralStringImplement(ErrorSourceZstd);

WCDBLiteralStringImplement(ErrorTypeMigrate);
WCDBLiteralStringImplement(ErrorTypeCompress);
WCDBLiteralStringImplement(ErrorTypeCheckpoint);
WCDBLiteralStringImplement(ErrorTypeIntegrity);
WCDBLiteralStringImplement(ErrorTypeBackup);
WCDBLiteralStringImplement(ErrorTypeMergeIndex);

WCDBLiteralStringImplement(MonitorInfoKeyHandleCount);
WCDBLiteralStringImplement(MonitorInfoKeyHandleOpenTime);
WCDBLiteralStringImplement(MonitorInfoKeyHandleOpenCPUTime);
WCDBLiteralStringImplement(MonitorInfoKeySchemaUsage);
WCDBLiteralStringImplement(MonitorInfoKeyTableCount);
WCDBLiteralStringImplement(MonitorInfoKeyIndexCount);
WCDBLiteralStringImplement(MonitorInfoKeyTriggerCount);

WCDBLiteralStringImplement(DecoratorAllType);
WCDBLiteralStringImplement(DecoratorMigratingHandleStatement);
WCDBLiteralStringImplement(DecoratorMigratingHandle);
WCDBLiteralStringImplement(DecoratorCompressingHandleStatement);
WCDBLiteralStringImplement(DecoratorCompressingHandle);

WCDBLiteralStringImplement(OperatorMigrate);
WCDBLiteralStringImplement(OperatorCompress);
WCDBLiteralStringImplement(OperatorBackup);
WCDBLiteralStringImplement(OperatorCheckIntegrity);

}; // namespace WCDB
