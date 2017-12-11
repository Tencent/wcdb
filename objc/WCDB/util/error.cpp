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

#include <WCDB/error.hpp>
#include <stdlib.h>
#include <string>

namespace WCDB {

ErrorValue::ErrorValue(int value)
    : m_type(ErrorValue::Type::Int), m_intValue(value), m_stringValue("")
{
}

ErrorValue::ErrorValue(const char *value)
    : m_type(ErrorValue::Type::String)
    , m_intValue(0)
    , m_stringValue(value ? value : "")
{
}

ErrorValue::ErrorValue(const std::string &value)
    : m_type(ErrorValue::Type::String), m_intValue(0), m_stringValue(value)
{
}

int ErrorValue::getIntValue() const
{
    if (m_type == ErrorValue::Type::Int) {
        return m_intValue;
    }
    return std::stoi(m_stringValue);
}

std::string ErrorValue::getStringValue() const
{
    if (m_type == ErrorValue::Type::String) {
        return m_stringValue;
    }
    return std::to_string(m_intValue);
}

ErrorValue::Type ErrorValue::getType() const
{
    return m_type;
}

std::shared_ptr<Error::ReportMethod> Error::s_reportMethod;

ThreadLocal<bool> Error::s_slient(false);

Error::Error() : m_type(Error::Type::NotSet), m_code(0)
{
}

Error::Error(Error::Type type, int code, const Error::Infos &infos)
    : m_type(type), m_code(code), m_infos(infos)
{
}

Error::Type Error::getType() const
{
    return m_type;
}

int Error::getCode() const
{
    return m_code;
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
    m_type = Error::Type::NotSet;
    m_infos.clear();
}

void Error::report() const
{
    if (!*s_slient.get()) {
        if (!s_reportMethod) {
            s_reportMethod.reset(
                (new Error::ReportMethod([](const Error &error) {
                    switch (error.getType()) {
                        case Error::Type::SQLiteGlobal:
#if DEBUG
                            printf("[WCDB][DEBUG]%s\n",
                                   error.description().c_str());
#endif
                            break;
                        case Error::Type::Warning:
                            printf("[WCDB][WARNING]%s\n",
                                   error.description().c_str());
                            break;
                        default:
                            printf("[WCDB][ERROR]%s\n",
                                   error.description().c_str());
#if DEBUG
                            if (error.getType() == Error::Type::Abort) {
                                abort();
                            }
#endif
                            break;
                    }
                })));
        }
        (*s_reportMethod)(*this);
    }
}

const char *Error::GetKeyName(Error::Key key)
{
    const char *name = "";
    switch (key) {
        case Error::Key::Operation:
            name = "Op";
            break;
        case Error::Key::ExtendedCode:
            name = "ExtCode";
            break;
        case Error::Key::Message:
            name = "Msg";
            break;
        case Error::Key::SQL:
            name = "SQL";
            break;
        case Error::Key::Tag:
            name = "Tag";
            break;
        case Error::Key::Path:
            name = "Path";
            break;
        default:
            break;
    }
    return name;
}

const char *Error::GetTypeName(Error::Type type)
{
    const char *name = "";
    switch (type) {
        case Error::Type::SQLite:
            name = "SQLite";
            break;
        case Error::Type::SystemCall:
            name = "SystemCall";
            break;
        case Error::Type::Core:
            name = "Core";
            break;
        case Error::Type::Interface:
            name = "Interface";
            break;
        case Error::Type::Abort:
            name = "Abort";
            break;
        case Error::Type::Warning:
            name = "Warning";
            break;
        case Error::Type::SQLiteGlobal:
            name = "SQLiteGlobal";
            break;
        case Error::Type::Repair:
            name = "Repair";
            break;
        default:
            break;
    }
    return name;
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

void Error::Report(Error::Type type,
                   int code,
                   const std::map<Key, ErrorValue> &infos,
                   Error *outError)
{
    Error error(type, code, infos);
    error.report();

    if (outError) {
        *outError = error;
    }
}

void Error::ReportSQLite(Tag tag,
                         const std::string &path,
                         Error::HandleOperation operation,
                         int rc,
                         const char *message,
                         Error *outError)
{
    Error::Report(Error::Type::SQLite, rc,
                  {
                      {Error::Key::Tag, ErrorValue(tag)},
                      {Error::Key::Operation, ErrorValue((int) operation)},
                      {Error::Key::Message, ErrorValue(message)},
                      {Error::Key::Path, path},
                  },
                  outError);
}

void Error::ReportSQLite(Tag tag,
                         const std::string &path,
                         HandleOperation operation,
                         int rc,
                         int extendedError,
                         const char *message,
                         Error *outError)
{
    Error::Report(Error::Type::SQLite, rc,
                  {
                      {Error::Key::Tag, ErrorValue(tag)},
                      {Error::Key::Operation, ErrorValue((int) operation)},
                      {Error::Key::Message, ErrorValue(message)},
                      {Error::Key::ExtendedCode, extendedError},
                      {Error::Key::Path, path},
                  },
                  outError);
}

void Error::ReportSQLite(Tag tag,
                         const std::string &path,
                         HandleOperation operation,
                         int rc,
                         int extendedError,
                         const char *message,
                         const std::string &sql,
                         Error *outError)
{
    Error::Report(Error::Type::SQLite, rc,
                  {
                      {Error::Key::Tag, ErrorValue(tag)},
                      {Error::Key::Operation, ErrorValue((int) operation)},
                      {Error::Key::Message, ErrorValue(message)},
                      {Error::Key::ExtendedCode, extendedError},
                      {Error::Key::SQL, sql},
                      {Error::Key::Path, path},
                  },
                  outError);
}

void Error::ReportCore(Tag tag,
                       const std::string &path,
                       CoreOperation operation,
                       CoreCode code,
                       const char *message,
                       Error *outError)
{
    Error::Report(Error::Type::Core, (int) code,
                  {
                      {Error::Key::Tag, ErrorValue(tag)},
                      {Error::Key::Operation, ErrorValue((int) operation)},
                      {Error::Key::Message, ErrorValue(message)},
                      {Error::Key::Path, path},
                  },
                  outError);
}

void Error::ReportInterface(Tag tag,
                            const std::string &path,
                            InterfaceOperation operation,
                            InterfaceCode code,
                            const char *message,
                            Error *outError)
{
    Error::Report(Error::Type::Interface, (int) code,
                  {
                      {Error::Key::Tag, ErrorValue(tag)},
                      {Error::Key::Operation, ErrorValue((int) operation)},
                      {Error::Key::Message, ErrorValue(message)},
                      {Error::Key::Path, path},
                  },
                  outError);
}

void Error::ReportSystemCall(SystemCallOperation operation,
                             const std::string &path,
                             int rc,
                             const char *message,
                             Error *outError)
{
    Error::Report(Error::Type::SystemCall, rc,
                  {
                      {Error::Key::Operation, ErrorValue((int) operation)},
                      {Error::Key::Message, ErrorValue(message)},
                      {Error::Key::Path, ErrorValue(path)},
                  },
                  outError);
}

void Error::ReportSQLiteGlobal(int rc, const char *message, Error *outError)
{
    Error::Report(Error::Type::SQLiteGlobal, rc,
                  {
                      {Error::Key::Message, ErrorValue(message)},
                  },
                  outError);
}

void Error::ReportRepair(const std::string &path,
                         RepairOperation operation,
                         int code,
                         Error *outError)
{
    Error::Report(Error::Type::Repair, code,
                  {
                      {Error::Key::Path, path},
                      {Error::Key::Operation, ErrorValue((int) operation)},
                  },
                  outError);
}

void Error::Abort(const char *message, Error *outError)
{
    Error::Report(Error::Type::Abort, (int) Error::GlobalCode::Abort,
                  {
                      {Error::Key::Message, ErrorValue(message)},
                  },
                  outError);
}

void Error::Warning(const char *message, Error *outError)
{
    Error::Report(Error::Type::Warning, (int) Error::GlobalCode::Warning,
                  {
                      {Error::Key::Message, ErrorValue(message)},
                  },
                  outError);
}

void Error::setThreadedSlient(bool slient)
{
    *s_slient.get() = slient;
}

} //namespace WCDB
