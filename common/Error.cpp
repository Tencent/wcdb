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

#include <WCDB/Assertion.hpp>
#include <WCDB/Error.hpp>
#include <WCDB/Macro.hpp>
#include <WCDB/String.hpp>

namespace WCDB {

#pragma mark - Initialize
Error::Error() : level(Level::Error), code(0)
{
}

Error::Error(const std::string &message_, Level level_)
    : code(Error::error), level(level_), message(message_), type("Core")
{
}

void Error::clear()
{
    type.clear();
    code = 0;
    level = Level::Error;
    message.clear();
    infos.clear();
}

#pragma mark - Code
bool Error::isOK() const
{
    return code == 0;
}

#pragma mark - Info
void Error::Infos::set(const std::string &key, int64_t value)
{
    m_ints[key] = value;
}

void Error::Infos::set(const std::string &key, const std::string &value)
{
    m_strings[key] = value;
}

const std::map<std::string, int64_t> Error::Infos::getIntInfos() const
{
    return m_ints;
}

const std::map<std::string, std::string> Error::Infos::getStringInfos() const
{
    return m_strings;
}

void Error::Infos::clear()
{
    m_ints.clear();
    m_strings.clear();
}

std::string Error::getDescription() const
{
    if (code == 0) {
        return String::empty();
    }
    std::ostringstream stream;
    stream << "[" << Error::LevelName(level) << "]";

    WCTInnerAssert(!type.empty());
    stream << type;

    if (code != Error::error) {
        stream << ", Code: " << code;
    }

    if (!message.empty()) {
        stream << ", Msg: " << message;
    }

    for (const auto &info : infos.getIntInfos()) {
        stream << ", " << info.first << ": " << info.second;
    }
    for (const auto &info : infos.getStringInfos()) {
        stream << ", " << info.first << ": " << info.second;
    }
    return stream.str();
}

} //namespace WCDB
