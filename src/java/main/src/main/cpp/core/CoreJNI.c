// Created by chenqiuwen on 2023/5/3.
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

#include "CoreJNI.h"
#include "CoreBridge.h"

jlong WCDBJNICoreClassMethod(createDatabase, jstring path, jboolean readonly, jboolean inMemory)
{
    WCDBJNIGetString(path);
    jlong ret = (jlong) WCDBCoreCreateDatabase(pathString, readonly, inMemory).innerValue;
    WCDBJNIReleaseString(path);
    return ret;
}

void WCDBJNICoreClassMethod(setDefaultCipherConfig, jint version)
{
    WCDBCoreSetDefaultCipherConfig(version);
}

void WCDBJNICoreClassMethodWithNoArg(purgeAllDatabase)
{
    WCDBCorePurgeAllDatabase();
}

void WCDBJNICoreClassMethod(releaseSQLiteMemory, jint bytes)
{
    WCDBCoreReleaseSQLiteMemory(bytes);
}

void WCDBJNICoreClassMethod(setSoftHeapLimit, jlong limit)
{
    WCDBCoreSetSoftHeapLimit((int64_t) limit);
}

void WCDBJNICoreClassMethod(setAutoCheckpointMinFrames, jint frames)
{
    WCDBCoreSetAutoCheckpointMinFrames(frames);
}

jlong WCDBJNICoreClassMethodWithNoArg(getThreadedError)
{
    return (jlong) WCDBCoreGetThreadedError().innerValue;
}
