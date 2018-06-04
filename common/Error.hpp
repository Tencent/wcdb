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

#ifndef Error_hpp
#define Error_hpp

#include <functional>
#include <map>
#include <sstream>
#include <string>

namespace WCDB {

class Error {
#pragma mark - Level
public:
    enum class Level : int {
        Ignore = 1,
        Debug = 2,
        Warning = 3,
        Error = 4,
        Fatal = 5,
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
            case Level::Error:
                return "ERROR";
            case Level::Fatal:
                return "FATAL";
        }
    }

#pragma mark - Code
public:
    enum class Code : int {
        OK = 0,         /* Successful result */
        Error = 1,      /* SQL error, missing database or other error */
        Internal = 2,   /* Internal logic error */
        Permission = 3, /* Access permission denied */
        Abort = 4,      /* Callback routine requested an abort */
        Busy = 5,       /* The database file is locked or system call busy */
        Locked = 6,     /* A table in the database is locked */
        NoMemory = 7,   /* Out of memory */
        Readonly = 8,   /* Attempt to write a readonly file */
        Interrupt = 9,  /* Operation terminated by sqlite3_interrupt()*/
        IOError = 10,   /* Some kind of disk I/O error occurred */
        Corrupt =
            11, /* The data(database, memory or other files) acquired is malformed */
        NotFound = 12, /* Unknown opcode in sqlite3_file_control() */
        Full = 13,     /* Disk is full */
        CantOpen = 14, /* Unable to open the database file */
        Protocol = 15, /* Database lock protocol error */
        Empty = 16,    /* Database is empty */
        Schema = 17,   /* The database schema changed */
        Exceed = 18,   /* String, BLOB, File, Concurrency exceeds size limit */
        Constraint = 19,         /* Abort due to constraint violation */
        Mismatch = 20,           /* Data type mismatch */
        Misuse = 21,             /* WCDB or SQLite used incorrectly */
        NoLargeFileSupport = 22, /* Uses OS features not supported on host */
        Authorization = 23,      /* Authorization denied */
        Format = 24,             /* Auxiliary database format error */
        Range =
            25, /* 2nd parameter to sqlite3_bind out of range or System call result too large */
        NotADatabase = 26, /* File opened that is not a database file */
        Notice = 27,       /* Notifications */
        Warning = 28,      /* Warnings */
    };
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
            case Code::Constraint:
                return "Constraint";
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
        }
    }

    Code code() const;
    bool isOK() const;

    void setSystemCode(int systemCode, Code codeIfUnresolved);
    void setSQLiteCode(int code);
    void setSQLiteCode(int code, int extendedCode);
    void setCode(Code code);
    void setCode(Code code, const std::string &source);

protected:
    Code m_code;

#pragma mark - Initialize
public:
    Error();
    Error(Code code);
    Error(Code code, const std::string &message);

    void clear();

#pragma mark - Info
public:
    std::string message;

    class Infos {
    public:
        void set(const std::string &key, int64_t value);
        void set(const std::string &key, const std::string &value);

        const std::map<std::string, int64_t> getIntInfos() const;
        const std::map<std::string, std::string> getStringInfos() const;

        void clear();

    protected:
        std::map<std::string, int64_t> m_ints;
        std::map<std::string, std::string> m_strings;
    };
    Infos infos;

#pragma mark - Description
public:
    std::string getDescription() const;
};

} //namespace WCDB

#endif /* Error_hpp */
