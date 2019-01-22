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

#ifndef _WCDB_CONSOLE_HPP
#define _WCDB_CONSOLE_HPP

#include <WCDB/Macro.hpp>
#include <WCDB/String.hpp>
#include <atomic>
#include <functional>

namespace WCDB {

class Error;

class Console final {
public:
    static Console* shared();

    void setDebuggable(bool debuggable);
    bool isDebuggable();

    typedef std::function<void(const Error&)> Logger;
    void setLogger(const Logger& logger);
    static void logger(const Error& error);

    typedef std::function<void(const String&)> Printer;
    void setPrinter(const Printer& printer);
    static void printer(const String& message);

    static bool debuggable();
#ifdef DEBUG
    static void fatal(const String& message, const char* file, int line);
#else  // DEBUG
    static void fatal(const String& message);
#endif // DEBUG

    Console(const Console&) = delete;
    Console& operator=(const Console&) = delete;

protected:
    Console();

    static void breakpoint() WCDB_USED WCDB_NO_INLINE;
    void print(const String& message);

private:
    Printer m_printer;
    std::atomic<bool> m_debuggable;
};

} // namespace WCDB

#endif /* _WCDB_CONSOLE_HPP */
