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

#include <WCDB/Error.hpp>
#include <WCDB/Macro.hpp>
#include <iostream>

namespace WCDB {

Error::Error() : level(Level::Error), code(0)
{
}

int Error::getType() const
{
    return Error::type;
}

bool Error::isOK() const
{
    return code == 0;
}

std::string Error::getDescription() const
{
    std::string description("[");
    description.append(Error::LevelName(level));
    description.append("]");
    if (level != Level::Warning && level != Level::Fatal) {
        addToDescription(description, "Code", code);
    }
    if (!message.empty()) {
        addToDescription(description, "Msg", message);
    }
    return description;
}

void Error::addToDescription(std::string &description,
                             const char *key,
                             int64_t value) const
{
    addToDescription(description, key, std::to_string(value));
}

void Error::addToDescription(std::string &description,
                             const char *key,
                             const std::string &value) const
{
    if (!description.empty()) {
        description.append(", ");
    }
    description.append(key);
    description.append(": ");
    description.append(value);
}

void Error::report()
{
    Report::shared()->report(*this);
}

void Error::error(const std::string &message)
{
    log(message, Error::Level::Error);
}

void Error::warning(const std::string &message)
{
    log(message, Error::Level::Warning);
}

void Error::fatal(const std::string &message)
{
    log(message, Error::Level::Fatal);
}

void Error::log(const std::string &message, Level level)
{
    Error error;
    error.level = level;
    error.code = (int) Code::Error;
    error.message = message;
    error.report();
}

Error::Report *Error::Report::shared()
{
    static Report report;
    return &report;
}

void Error::Report::defaultCallback(const Error &error)
{
    switch (error.level) {
        case Level::Ignore:
            break;
        case Level::Debug:
#ifdef DEBUG
            std::cout << error.getDescription() << std::endl;
#endif
            break;
        case Level::Warning:
        case Level::Error:
            std::cout << error.getDescription() << std::endl;
            break;
        case Level::Fatal:
            std::cout << error.getDescription() << std::endl;
            abort();
            break;
    }
}

void Error::Report::setCallback(const Callback &callback)
{
    m_callback = callback;
}

Error::Report::Report() : m_callback(Report::defaultCallback)
{
}

void Error::Report::report(const Error &error)
{
    if (m_callback) {
        m_callback(error);
    }
}

} //namespace WCDB
