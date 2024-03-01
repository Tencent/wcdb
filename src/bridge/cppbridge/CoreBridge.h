//
// Created by qiuwenchen on 2022/4/4.
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
#include "DatabaseBridge.h"
#include "ObjectBridge.h"

WCDB_EXTERN_C_BEGIN

CPPDatabase WCDBCoreCreateDatabase(const char* _Nonnull path);
void WCDBCoreSetDefaultCipherConfig(int version);
void WCDBCorePurgeAllDatabase(void);
bool WCDBCoreSetDefaultTemporaryDirectory(const char* _Nullable dir);
void WCDBCoreSetAutoCheckpointEnable(CPPDatabase database, bool enable);
void WCDBCoreSetAutoCheckpointMinFrames(int frames);
void WCDBCoreReleaseSQLiteMemory(int bytes);
void WCDBCoreSetSoftHeapLimit(long long limit);
CPPError WCDBCoreGetThreadedError();

typedef void (*WCDBBusyTracer)(void* _Nullable context,
                               long tag,
                               const char* _Nonnull path,
                               unsigned long long tid,
                               const char* _Nonnull sql);
void WCDBCoreGlobalTraceBusy(WCDBBusyTracer _Nullable tracer,
                             double timeOut,
                             void* _Nullable context,
                             WCDBContextDestructor _Nullable destructor);

WCDB_EXTERN_C_END
