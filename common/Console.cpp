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

void Console::debug()
{
    s_debuggable.store(true);
}

void Console::release()
{
    s_debuggable.store(false);
}

bool Console::debuggable()
{
    return s_debuggable.load();
}

#ifdef DEBUG

std::atomic<bool> Console::s_debuggable(true);

void Console::fatal(const String& message, const char* file, int line)
{
    Error error(Error::Code::Misuse, Error::Level::Fatal, message);
    error.infos.set(ErrorStringKeySource, ErrorSourceAssertion);
    if (file != nullptr) {
        error.infos.set("File", file);
    }
    error.infos.set("Line", line);
    error.infos.set("Version", WCDB_VERSION);
    error.infos.set("BuildTime", WCDB_BUILD_TIME);
    error.infos.set("BuildTimestamp", WCDB_BUILD_TIMESTAMP);
    error.infos.set("CommitHash", WCDB_COMMIT_HASH);
    Notifier::shared()->notify(error);
}

#else // DEBUG

std::atomic<bool> Console::s_debuggable(false);

void Console::fatal(const String& message)
{
    Error error(Error::Code::Misuse, Error::Level::Fatal, message);
    error.infos.set(ErrorStringKeySource, ErrorSourceAssertion);
    error.infos.set("Version", WCDB_VERSION);
    error.infos.set("BuildTime", WCDB_BUILD_TIME);
    error.infos.set("BuildTimestamp", WCDB_BUILD_TIMESTAMP);
    error.infos.set("CommitHash", WCDB_COMMIT_HASH);
    Notifier::shared()->notify(error);
}

#endif // DEBUG

} // namespace WCDB
