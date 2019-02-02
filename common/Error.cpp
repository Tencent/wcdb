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
#include <WCDB/SQLite.h>
#include <WCDB/String.hpp>

namespace WCDB {

static_assert((int) Error::Code::OK == SQLITE_OK, "");
static_assert((int) Error::Code::Error == SQLITE_ERROR, "");
static_assert((int) Error::Code::Internal == SQLITE_INTERNAL, "");
static_assert((int) Error::Code::Permission == SQLITE_PERM, "");
static_assert((int) Error::Code::Abort == SQLITE_ABORT, "");
static_assert((int) Error::Code::Busy == SQLITE_BUSY, "");
static_assert((int) Error::Code::Locked == SQLITE_LOCKED, "");
static_assert((int) Error::Code::NoMemory == SQLITE_NOMEM, "");
static_assert((int) Error::Code::Readonly == SQLITE_READONLY, "");
static_assert((int) Error::Code::Interrupt == SQLITE_INTERRUPT, "");
static_assert((int) Error::Code::IOError == SQLITE_IOERR, "");
static_assert((int) Error::Code::Corrupt == SQLITE_CORRUPT, "");
static_assert((int) Error::Code::NotFound == SQLITE_NOTFOUND, "");
static_assert((int) Error::Code::Full == SQLITE_FULL, "");
static_assert((int) Error::Code::CantOpen == SQLITE_CANTOPEN, "");
static_assert((int) Error::Code::Protocol == SQLITE_PROTOCOL, "");
static_assert((int) Error::Code::Empty == SQLITE_EMPTY, "");
static_assert((int) Error::Code::Schema == SQLITE_SCHEMA, "");
static_assert((int) Error::Code::Exceed == SQLITE_TOOBIG, "");
static_assert((int) Error::Code::Conflict == SQLITE_CONSTRAINT, "");
static_assert((int) Error::Code::Mismatch == SQLITE_MISMATCH, "");
static_assert((int) Error::Code::Misuse == SQLITE_MISUSE, "");
static_assert((int) Error::Code::NoLargeFileSupport == SQLITE_NOLFS, "");
static_assert((int) Error::Code::Authorization == SQLITE_AUTH, "");
static_assert((int) Error::Code::Format == SQLITE_FORMAT, "");
static_assert((int) Error::Code::Range == SQLITE_RANGE, "");
static_assert((int) Error::Code::NotADatabase == SQLITE_NOTADB, "");
static_assert((int) Error::Code::Notice == SQLITE_NOTICE, "");
static_assert((int) Error::Code::Warning == SQLITE_WARNING, "");
static_assert((int) Error::Code::Row == SQLITE_ROW, "");
static_assert((int) Error::Code::Done == SQLITE_DONE, "");

#pragma mark - Initialize
Error::Error() : level(Level::Error), m_code(Code::OK)
{
}

Error::Error(Code code) : level(Level::Error), m_code(code)
{
}

Error::Error(Code code, const String &message_)
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
Error::Code Error::rc(int rc)
{
    return (Error::Code) rc;
}

void Error::setCode(Code code)
{
    m_code = code;
}

void Error::setCode(Code code, const String &source)
{
    setCode(code);
    infos.set("Source", source);
}

void Error::setSystemCode(int systemCode, Code codeIfUnresolved)
{
    Code code;
    switch (systemCode) {
    case EIO:
        code = Code::IOError;
        break;
    case ENOMEM:
        code = Code::NoMemory;
        break;
    case EACCES:
        code = Code::Permission;
        break;
    case EBUSY:
        code = Code::Busy;
        break;
    case ENOSPC:
        code = Code::Full;
        break;
    case EAUTH:
        code = Code::Authorization;
        break;
    case ENOENT:
        code = Code::NotFound;
        break;
    default:
        code = codeIfUnresolved;
        break;
    }
    const char *errorMessage = strerror(systemCode);
    if (errorMessage != nullptr) {
        message = errorMessage;
    }
    setCode(code, "System");
    infos.set("ExtCode", systemCode);
}

void Error::setSQLiteCode(int fullCode)
{
    int code = fullCode & 0xff;
    setCode((Code) code, "SQLite");
    if (fullCode != code) {
        infos.set("ExtCode", fullCode);
    }
}

void Error::setSQLiteCode(int code, int extendedCode)
{
    WCTInnerAssert(code == (extendedCode & 0xff));
    setCode((Code)(code & 0xff), "SQLite");
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

bool Error::isCorruption() const
{
    return m_code == Error::Code::Corrupt || m_code == Error::Code::NotADatabase;
}

#pragma mark - Info
void Error::Infos::set(const String &key, const String &value)
{
    m_strings[key] = value;
}

void Error::Infos::unset(const String &key)
{
    m_strings.erase(key);
    m_integers.erase(key);
    m_doubles.erase(key);
}

const std::map<String, int64_t> &Error::Infos::getIntegers() const
{
    return m_integers;
}

const std::map<String, String> &Error::Infos::getStrings() const
{
    return m_strings;
}

const std::map<String, double> &Error::Infos::getDoubles() const
{
    return m_doubles;
}

void Error::Infos::clear()
{
    m_integers.clear();
    m_doubles.clear();
    m_strings.clear();
}

bool Error::Infos::empty() const
{
    return m_integers.empty() && m_doubles.empty() && m_strings.empty();
}

} //namespace WCDB
