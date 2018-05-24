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
Error::Error() : level(Level::Error), m_code(Code::OK)
{
}

Error::Error(Code code) : level(Level::Error), m_code(code)
{
}

Error::Error(Code code, const std::string &message_)
    : level(Level::Error), m_code(code), message(message_)
{
}

void Error::clear()
{
    m_code = Code::OK;
    level = Level::Error;
    message.clear();
    infos.clear();
}

#pragma mark - Code
void Error::setCode(Code code)
{
    m_code = code;
}

void Error::setCode(Code code, const std::string &source)
{
    m_code = code;
    infos.set("Source", source);
}

void Error::setSystemCode(int systemCode, Code codeIfUnresolve)
{
    switch (systemCode) {
        case EIO:
            m_code = Code::IOError;
            break;
        case ENOMEM:
            m_code = Code::NoMemory;
            break;
        case EACCES:
            m_code = Code::Permission;
            break;
        case EBUSY:
            m_code = Code::Busy;
            break;
        case ENOSPC:
            m_code = Code::Full;
            break;
        case EAUTH:
            m_code = Code::Authorization;
            break;
        default:
            m_code = codeIfUnresolve;
            break;
    }
    infos.set("ExtCode", systemCode);
    infos.set("Source", "System");
}

void Error::setSQLiteCode(int code)
{
    m_code = (Code) code;
    infos.set("Source", "SQLite");
}

void Error::setSQLiteCode(int code, int extendedCode)
{
    setSQLiteCode(code);
    infos.set("ExtCode", extendedCode);
}

Error::Code Error::code() const
{
    return m_code;
}

bool Error::isOK() const
{
    return m_code == Code::OK;
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
    if (m_code == Code::OK) {
        return String::empty();
    }
    std::ostringstream stream;
    stream << "[" << Error::LevelName(level) << ": ";
    stream << (int) m_code << ", ";
    if (!message.empty()) {
        stream << message;
    } else {
        stream << Error::CodeName(m_code);
    }
    stream << "]";

    bool comma = false;
    for (const auto &info : infos.getIntInfos()) {
        if (comma) {
            stream << ", ";
        } else {
            comma = true;
        }
        stream << info.first << ": " << info.second;
    }
    for (const auto &info : infos.getStringInfos()) {
        if (comma) {
            stream << ", ";
        } else {
            comma = true;
        }
        stream << ", " << info.first << ": " << info.second;
    }
    return stream.str();
}

} //namespace WCDB
