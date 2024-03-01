//
// Created by sanhuazhang on 2018/09/28
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

#include "Console.hpp"
#include "CoreConst.h"
#include "Notifier.hpp"
#include "WCDBError.hpp"
#include "WCDBVersion.h"

#if !defined(__ANDROID__) && !defined(_WIN32)
#include <execinfo.h>
#endif

#include <stdlib.h>

namespace WCDB {

#if WCDB_DEBUG

void Console::fatal(const UnsafeStringView& message, const char* file, int line, const char* function)
{
    Error error(Error::Code::Misuse, Error::Level::Fatal, message);
    error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceAssertion);
    error.infos.insert_or_assign("File", file);
    error.infos.insert_or_assign("Line", line);
    error.infos.insert_or_assign("Func", function);
    error.infos.insert_or_assign("Version", WCDB_VERSION_STRING);
    error.infos.insert_or_assign("Build", WCDB_BUILD_STRING);
    auto callstacks = Console::callstacks();
    if (callstacks.succeed()) {
        error.infos.insert_or_assign("Callstacks", callstacks.value());
    }
    Notifier::shared().notify(error);
}

#else // WCDB_DEBUG

void Console::fatal(const UnsafeStringView& message)
{
    Error error(Error::Code::Misuse, Error::Level::Fatal, message);
    error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceAssertion);
    error.infos.insert_or_assign("Version", WCDB_VERSION_STRING);
    error.infos.insert_or_assign("Build", WCDB_BUILD_STRING);
    auto callstacks = Console::callstacks();
    if (callstacks.succeed()) {
        error.infos.insert_or_assign("Callstacks", callstacks.value());
    }
    Notifier::shared().notify(error);
}

#endif // WCDB_DEBUG

Optional<StringView> Console::callstacks()
{
#if !defined(__ANDROID__) && !defined(_WIN32)
    constexpr const int size = 100;
    void* buffer[size];
    int depth = backtrace(buffer, size);
    char** symbols = backtrace_symbols(buffer, depth);
    if (symbols == nullptr) {
        return NullOpt;
    }

    std::ostringstream stream;
    std::string string;
    for (int i = 0; i < depth; ++i) {
        if (i != 0) {
            stream << std::endl;
        }
        stream << symbols[i];
    }
    free(symbols);
    return StringView(stream.str());
#else
    return NullOpt;
#endif
}

} // namespace WCDB
