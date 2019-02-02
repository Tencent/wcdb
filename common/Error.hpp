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

#ifndef _WCDB_ERROR_HPP
#define _WCDB_ERROR_HPP

#include <WCDB/String.hpp>
#include <functional>
#include <map>
#include <sstream>

namespace WCDB {

class Error final {
#pragma mark - Level
public:
    enum class Level : int {
        Ignore = 1,
        Debug = 2,
        Warning = 3,
        Notice = 4,
        Error = 5,
        Fatal = 6,
    };
    Level level;
    static constexpr const char *levelName(const Level &level)
    {
        switch (level) {
        case Level::Ignore:
            return "IGNORE";
        case Level::Debug:
            return "DEBUG";
        case Level::Warning:
            return "WARNING";
        case Level::Notice:
            return "NOTICE";
        case Level::Error:
            return "ERROR";
        case Level::Fatal:
            return "FATAL";
        }
    }

#pragma mark - Code
public:
    enum class Code : int {
        OK = 0,
        Error = 1,
        Internal = 2,
        Permission = 3,
        Abort = 4,
        Busy = 5,
        Locked = 6,
        NoMemory = 7,
        Readonly = 8,
        Interrupt = 9,
        IOError = 10,
        Corrupt = 11,
        NotFound = 12,
        Full = 13,
        CantOpen = 14,
        Protocol = 15,
        Empty = 16,
        Schema = 17,
        Exceed = 18,
        Conflict = 19,
        Mismatch = 20,
        Misuse = 21,
        NoLargeFileSupport = 22,
        Authorization = 23,
        Format = 24,
        Range = 25,
        NotADatabase = 26,
        Notice = 27,
        Warning = 28,
        Row = 100,
        Done = 101,
    };
    static inline Error::Code rc(int rc);
    static constexpr const char *codeName(const Code &code)
    {
        switch (code) {
        case Code::OK:
            return "OK";
        case Code::Error:
            return "Error";
        case Code::Internal:
            return "Internal";
        case Code::Permission:
            return "Permission";
        case Code::Abort:
            return "Abort";
        case Code::Busy:
            return "Busy";
        case Code::Locked:
            return "Locked";
        case Code::NoMemory:
            return "NoMemory";
        case Code::Readonly:
            return "Readonly";
        case Code::Interrupt:
            return "Interrupt";
        case Code::IOError:
            return "IOError";
        case Code::Corrupt:
            return "Corrupt";
        case Code::NotFound:
            return "NotFound";
        case Code::Full:
            return "Full";
        case Code::CantOpen:
            return "CantOpen";
        case Code::Protocol:
            return "Protocol";
        case Code::Empty:
            return "Empty";
        case Code::Schema:
            return "Schema";
        case Code::Exceed:
            return "Exceed";
        case Code::Conflict:
            return "Conflict";
        case Code::Mismatch:
            return "Mismatch";
        case Code::Misuse:
            return "Misuse";
        case Code::NoLargeFileSupport:
            return "NoLargeFileSupport";
        case Code::Authorization:
            return "Authorization";
        case Code::Format:
            return "Format";
        case Code::Range:
            return "Range";
        case Code::NotADatabase:
            return "NotADatabase";
        case Code::Notice:
            return "Notice";
        case Code::Warning:
            return "Warning";
        case Code::Row:
            return "Row";
        case Code::Done:
            return "Done";
        }
    }

    Code code() const;
    bool isOK() const;
    bool isCorruption() const;

    void setSystemCode(int systemCode, Code codeIfUnresolved);
    void setSQLiteCode(int code);
    void setSQLiteCode(int code, int extendedCode);
    void setCode(Code code);
    void setCode(Code code, const String &source);

protected:
    Code m_code;

#pragma mark - Initialize
public:
    Error();
    Error(Code code);
    Error(Code code, const String &message);

    void clear();

#pragma mark - Info
public:
    String message;

    class Infos final {
    public:
#if __cplusplus > 201402L
#warning TODO \
std::any is available since C++17.
#endif
        template<typename T>
        typename std::enable_if<std::is_integral<T>::value, void>::type
        set(const String &key, const T &value)
        {
            m_integers[key] = (int64_t) value;
        }
        template<typename T>
        typename std::enable_if<std::is_floating_point<T>::value, void>::type
        set(const String &key, const T &value)
        {
            m_doubles[key] = (double) value;
        }
        void set(const String &key, const String &value);
        void unset(const String &key);

        const std::map<String, int64_t> &getIntegers() const;
        const std::map<String, String> &getStrings() const;
        const std::map<String, double> &getDoubles() const;

        void clear();
        bool empty() const;

    protected:
        std::map<String, int64_t> m_integers;
        std::map<String, double> m_doubles;
        std::map<String, String> m_strings;
    };
    Infos infos;
};

} //namespace WCDB

#endif /* _WCDB_ERROR_HPP */
