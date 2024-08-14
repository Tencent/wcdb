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

#include "Exiting.hpp"
#include "CommonCore.hpp"
#include "Macro.h"
#include <atomic>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
// VersionHelpers.h must be included after windows.h.
#include <VersionHelpers.h>
#endif

namespace WCDB {

static void exiting();

static std::atomic<bool>& exitingValue()
{
    static std::atomic<bool>* s_exitingValue = new std::atomic<bool>(false);
    WCDB_ONCE(atexit(exiting));
    return *s_exitingValue;
}

static void exiting()
{
    exitingValue().store(true);

#ifdef _WIN32
    /*
     On Windows 7, the thread where the OperationQueue is located may have been released.
     So it can't receive notifications from the condition variable.
     */
    if (IsWindows7OrGreater() && !IsWindows8OrGreater()) {
        return;
    }
#endif

    // The queue needs to be terminated to exit the program normally in Windows.
    CommonCore::shared().stopQueue();
}

bool isExiting()
{
    return exitingValue().load();
}

} // namespace WCDB
