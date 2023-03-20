//
// Created by sanhuazhang on 2018/05/19
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

#include "Console.hpp"

// Assert is for WCDB debugging only, which will be removed when released. It ensures that the correctness of WCDB's code.
// Remedial assertion is used to ensure that the app does not crash due to incorrect usage by developers.

#if WCDB_DEBUG

#define __WCTAssert(cond, message, action)                                      \
    if (!(cond)) {                                                              \
        WCDB::Console::fatal(message, __FILE__, __LINE__, __PRETTY_FUNCTION__); \
        action                                                                  \
    }

#define WCTAssert(cond)                                                        \
    __WCTAssert(                                                               \
    cond, "If you think it's a bug caused by WCDB, please report to us.", abort();)

#else // WCDB_DEBUG

#define __WCTAssert(cond, message, action)                                     \
    if (!(cond)) {                                                             \
        WCDB::Console::fatal(message);                                         \
        action                                                                 \
    }

#define WCTAssert(cond)

#endif // WCDB_DEBUG

#define WCTRemedialAssert(cond, message, remedial)                             \
    __WCTAssert(cond, message, remedial)
