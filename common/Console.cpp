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

#include <WCDB/Console.hpp>
#include <WCDB/CoreConst.h>
#include <WCDB/Error.hpp>
#include <WCDB/Notifier.hpp>
#include <WCDB/Version.h>
#include <iostream>

namespace WCDB {

std::atomic<bool>& Console::debuggableValue()
{
#if WCDB_DEBUG
    std::atomic<bool>* s_debuggable = new std::atomic<bool>(true);
#else  // WCDB_DEBUG
    std::atomic<bool>* s_debuggable = new std::atomic<bool>(false);
#endif // WCDB_DEBUG
    return *s_debuggable;
}

void Console::debug()
{
    debuggableValue().store(true);
}

void Console::release()
{
    debuggableValue().store(false);
}

bool Console::debuggable()
{
    return debuggableValue().load();
}

#if WCDB_DEBUG

void Console::fatal(const UnsafeStringView& message, const char* file, int line, const char* function)
{
    Error error(Error::Code::Misuse, Error::Level::Fatal, message);
    error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceAssertion);
    error.infos.insert_or_assign("File", file);
    error.infos.insert_or_assign("Line", line);
    error.infos.insert_or_assign("Func", function);
    error.infos.insert_or_assign("Version", WCDB_VERSION_STRING);
    error.infos.insert_or_assign("Timestamp", WCDB_TIMESTAMP_STRING);
    error.infos.insert_or_assign("Build", WCDB_BUILD_STRING);
    Notifier::shared().notify(error);
}

#else // WCDB_DEBUG

void Console::fatal(const UnsafeStringView& message)
{
    Error error(Error::Code::Misuse, Error::Level::Fatal, message);
    error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceAssertion);
    error.infos.insert_or_assign("Version", WCDB_VERSION_STRING);
    error.infos.insert_or_assign("Timestamp", WCDB_TIMESTAMP_STRING);
    error.infos.insert_or_assign("Build", WCDB_BUILD_STRING);
    Notifier::shared().notify(error);
}

#endif // WCDB_DEBUG

} // namespace WCDB
