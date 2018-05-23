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

#include <WCDB/Reporter.hpp>
#include <iostream>

namespace WCDB {

Reporter *Reporter::shared()
{
    static Reporter reporter;
    return &reporter;
}

void Reporter::defaultCallback(const Error &error)
{
    logger(error.level, error.getDescription());
}

void Reporter::logger(const Error::Level &level, const std::string &description)
{
    switch (level) {
        case Error::Level::Ignore:
            break;
        case Error::Level::Debug:
#ifdef DEBUG
            std::cout << description << std::endl;
#endif
            break;
        case Error::Level::Warning:
        case Error::Level::Error:
            std::cout << description << std::endl;
            break;
        case Error::Level::Fatal:
            std::cout << description << std::endl;
            abort();
            break;
    }
}

void Reporter::setCallback(const Callback &callback)
{
    m_callback = callback;
}

Reporter::Reporter() : m_callback(Reporter::defaultCallback)
{
}

void Reporter::report(const Error &error)
{
    if (m_callback) {
        m_callback(error);
    }
}

void Reporter::fatal(const std::string &message, const char *file, int line)
{
    error(Error::Level::Fatal, message, file, line);
}

void Reporter::error(const std::string &message, const char *file, int line)
{
    error(Error::Level::Error, message, file, line);
}

void Reporter::warning(const std::string &message, const char *file, int line)
{
    error(Error::Level::Warning, message, file, line);
}

void Reporter::error(Error::Level level,
                     const std::string &message,
                     const char *file,
                     int line)
{
    Error error;
    error.setCode(Error::Code::Misuse);
    error.level = level;
    error.message = message;
    if (file) {
        error.infos.set("File", file);
    }
    error.infos.set("Line", line);
    Reporter::shared()->report(error);
}

} //namespace WCDB
