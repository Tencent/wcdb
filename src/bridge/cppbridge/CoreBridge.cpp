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

#include "CoreBridge.h"
#include "CommonCore.hpp"
#include "ObjectBridge.hpp"
#include "Path.hpp"
#include "ThreadedErrors.hpp"

CPPDatabase WCDBCoreCreateDatabase(const char* _Nonnull path)
{
    WCDB::RecyclableDatabase database
    = WCDB::CommonCore::shared().getOrCreateDatabase(WCDB::Path::normalize(path));
    return WCDBCreateRecylableCPPObject(CPPDatabase, database);
}

void WCDBCoreSetDefaultCipherConfig(int version)
{
    WCDB::CommonCore::shared().setDefaultCipherConfiguration(version);
}

void WCDBCorePurgeAllDatabase(void)
{
    WCDB::CommonCore::shared().purgeDatabasePool();
}

bool WCDBCoreSetDefaultTemporaryDirectory(const char* _Nullable dir)
{
    return WCDB::CommonCore::shared().setDefaultTemporaryDirectory(dir);
}

void WCDBCoreSetAutoCheckpointEnable(CPPDatabase database, bool enable)
{
    WCDBGetObjectOrReturn(database, WCDB::InnerDatabase, cppDatabase);
    WCDB::CommonCore::shared().enableAutoCheckpoint(cppDatabase, enable);
}

void WCDBCoreSetAutoCheckpointMinFrames(int frames)
{
    WCDB::CommonCore::shared().setCheckPointMinFrames(frames);
}

void WCDBCoreReleaseSQLiteMemory(int bytes)
{
    WCDB::CommonCore::shared().releaseSQLiteMemory(bytes);
}

void WCDBCoreSetSoftHeapLimit(long long limit)
{
    WCDB::CommonCore::shared().setSoftHeapLimit(limit);
}

CPPError WCDBCoreGetThreadedError()
{
    const WCDB::Error& error = WCDB::ThreadedErrors::shared().getThreadedError();
    return WCDBCreateUnmanagedCPPObject(CPPError, &error);
}

void WCDBCoreGlobalTraceBusy(WCDBBusyTracer _Nullable tracer,
                             double timeOut,
                             void* _Nullable context,
                             WCDBContextDestructor _Nullable destructor)
{
    WCDB::CommonCore::BusyMonitor callback = nullptr;
    if (tracer != nullptr) {
        WCDB::RecyclableContext recyclableContent(context, destructor);
        callback = [tracer, recyclableContent](const WCDB::Tag& tag,
                                               const WCDB::UnsafeStringView& path,
                                               uint64_t tid,
                                               const WCDB::UnsafeStringView& sql) {
            tracer(recyclableContent.get(), tag, path.data(), tid, sql.data());
        };
    }
    WCDB::CommonCore::shared().setBusyMonitor(callback, timeOut);
}
