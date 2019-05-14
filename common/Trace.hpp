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

#ifndef __WCDB_TRACE_HPP
#define __WCDB_TRACE_HPP

#include <WCDB/Console.hpp>
#include <WCDB/Debug.hpp>
#include <WCDB/Time.hpp>

#if WCDB_TRACE

#define WCTTrace(message)                                                      \
    WCDB::Console::trace(message, __FILE__, __LINE__, __PRETTY_FUNCTION__);

#else // WCDB_TRACE

#define WCTTrace(message)

#endif // WCDB_TRACE

#endif /* __WCDB_TRACE_HPP */
