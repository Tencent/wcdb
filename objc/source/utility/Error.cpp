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
#include <WCDB/OptionalImpl.hpp>
#include <stdlib.h>
#include <string>

namespace WCDB {

Error::Value::Value(int value)
    : m_type(Error::Value::Type::Int), m_intValue(value), m_stringValue("")
{
}

Error::Value::Value(const char *value)
    : m_type(Error::Value::Type::String)
    , m_intValue(0)
    , m_stringValue(value ? value : "")
{
}

Error::Value::Value(const std::string &value)
    : m_type(Error::Value::Type::String), m_intValue(0), m_stringValue(value)
{
}

int Error::Value::getIntValue() const
{
    if (m_type == Error::Value::Type::Int) {
        return m_intValue;
    }
    return std::stoi(m_stringValue);
}

std::string Error::Value::getStringValue() const
{
    if (m_type == Error::Value::Type::String) {
        return m_stringValue;
    }
    return std::to_string(m_intValue);
}

Error::Value::Type Error::Value::getType() const
{
    return m_type;
}

Error::ReportMethod Error::s_builtinErrorReport([](const Error &error) {
    switch (error.getType()) {
        case Error::Type::SQLiteGlobal:
#if DEBUG
            printf("[WCDB][DEBUG]%s\n", error.description().c_str());
#endif
            break;
        case Error::Type::Warning:
            printf("[WCDB][WARNING]%s\n", error.description().c_str());
            break;
        default:
            printf("[WCDB][ERROR]%s\n", error.description().c_str());
#if DEBUG
            if (error.getType() == Error::Type::Abort) {
                abort();
            }
#endif
            break;
    }
});

std::shared_ptr<Error::ReportMethod>
    Error::s_reportMethod(new ReportMethod(Error::s_builtinErrorReport));

ThreadLocal<bool> Error::s_slient(false);

Error::Error() : m_type(Error::Type::NotSet), m_code(0)
{
}

void Error::setError(Type type, int code)
{
    m_type = type;
    m_code = code;
    m_infos.clear();
}

void Error::setOperation(Error::Operation operation)
{
    m_infos.insert({Error::Key::Operation, (int) operation});
}

void Error::setTag(Tag tag)
{
    m_infos.insert({Error::Key::Tag, tag});
}

void Error::setExtendedCode(int extendedCode)
{
    m_infos.insert({Error::Key::ExtendedCode, extendedCode});
}

void Error::setMessage(const std::string &message)
{
    m_infos.insert({Error::Key::Message, message});
}

void Error::setSQL(const std::string &sql)
{
    m_infos.insert({Error::Key::SQL, sql});
}

void Error::setPath(const std::string &path)
{
    m_infos.insert({Error::Key::Path, path});
}

Error::Type Error::getType() const
{
    return m_type;
}

int Error::getCode() const
{
    return m_code;
}

Optional<Tag> Error::getTag() const
{
    auto iter = m_infos.find(Error::Key::Tag);
    if (iter != m_infos.end()) {
        return (Tag) iter->second.getIntValue();
    }
    return {};
}

Optional<int> Error::getOperationValue() const
{
    auto iter = m_infos.find(Error::Key::Operation);
    if (iter != m_infos.end()) {
        return iter->second.getIntValue();
    }
    return {};
}

Optional<int> Error::getExtendedCode() const
{
    auto iter = m_infos.find(Error::Key::ExtendedCode);
    if (iter != m_infos.end()) {
        return iter->second.getIntValue();
    }
    return {};
}

Optional<std::string> Error::getMessage() const
{
    auto iter = m_infos.find(Error::Key::Message);
    if (iter != m_infos.end()) {
        return iter->second.getStringValue();
    }
    return {};
}

Optional<std::string> Error::getSQL() const
{
    auto iter = m_infos.find(Error::Key::SQL);
    if (iter != m_infos.end()) {
        return iter->second.getStringValue();
    }
    return {};
}

Optional<std::string> Error::getPath() const
{
    auto iter = m_infos.find(Error::Key::Path);
    if (iter != m_infos.end()) {
        return iter->second.getStringValue();
    }
    return {};
}

const Error::Infos &Error::getInfos() const
{
    return m_infos;
}

bool Error::isOK() const
{
    return m_code == 0;
}

void Error::reset()
{
    m_code = 0;
}

void Error::clear()
{
    m_code = 0;
    m_type = Error::Type::NotSet;
    m_infos.clear();
}

void Error::report() const
{
    if (!*s_slient.get()) {
        std::shared_ptr<Error::ReportMethod> report = s_reportMethod;
        if (report != nullptr) {
            (*report.get())(*this);
        }
    }
}

std::string Error::description() const
{
    std::string desc;
    desc.append("Code:" + std::to_string(m_code) + ", ");
    desc.append("Type:" + std::string(GetTypeName(m_type)) + ", ");
    bool first = true;
    for (const auto &iter : m_infos) {
        if (!first) {
            desc.append(", ");
        } else {
            first = false;
        }
        desc.append(GetKeyName(iter.first));
        desc.append(":");
        desc.append(iter.second.getStringValue());
    }
    return desc;
}

void Error::SetReportMethod(const ReportMethod &reportMethod)
{
    s_reportMethod.reset(new ReportMethod(reportMethod));
}

void Error::ResetReportMethod()
{
    s_reportMethod.reset(new ReportMethod(s_builtinErrorReport));
}

void Error::Abort(const std::string &message)
{
    Error error;
    error.setError(Error::Type::Abort, -1);
    error.setMessage(message);
    error.report();
}

void Error::Warning(const std::string &message)
{
    Error error;
    error.setError(Error::Type::Warning, -1);
    error.setMessage(message);
    error.report();
}

void Error::setThreadedSlient(bool slient)
{
    *s_slient.get() = slient;
}

} //namespace WCDB
