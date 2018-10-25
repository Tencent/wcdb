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

#ifndef Console_hpp
#define Console_hpp

#include <WCDB/Notifier.hpp>
#include <WCDB/String.hpp>
#include <atomic>

namespace WCDB {

class Error;

class Console {
public:
    static Console* shared();

    void setDebuggable(bool debuggable);
    bool isDebuggable();
    typedef Notifier::Callback Callback;
    void setLogger(const Callback& callback);

    static bool debuggable();

    static void log(const Error& error);
    static void fatal(const String& message, const char* file, int line);
    static void breakpoint(const Error& error);

    Console(const Console&) = delete;
    Console& operator=(const Console&) = delete;

protected:
    Console();

private:
    std::atomic<bool> m_debugable;
    static void print(const Error& error);
};

} // namespace WCDB

#endif /* Console_hpp */
