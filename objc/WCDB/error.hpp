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

#ifndef error_hpp
#define error_hpp

#include <WCDB/thread_local.hpp>
#include <WCDB/utility.hpp>
#include <climits>
#include <functional>
#include <map>
#include <string>

namespace WCDB {

typedef int Tag;
const Tag InvalidTag = 0;

class ErrorValue {
public:
    enum class Type : int {
        Int,
        String,
    };
    ErrorValue(int value);
    ErrorValue(const char *value);
    ErrorValue(const std::string &value);

    int getIntValue() const;
    std::string getStringValue() const;
    ErrorValue::Type getType() const;

protected:
    ErrorValue::Type m_type;
    int m_intValue;
    std::string m_stringValue;
};

class Error {
public:
    //key
    enum class Key : int {
        Tag = 1,
        Operation = 2,
        ExtendedCode = 3,
        Message = 4,
        SQL = 5,
        Path = 6,
    };
    //type
    enum class Type : int {
        NotSet = 0,
        SQLite = 1,
        SystemCall = 2,
        Core = 3,
        Interface = 4,
        Abort = 5,
        Warning = 6,
        SQLiteGlobal = 7,
        Repair = 8,
    };
    //operation
    enum class HandleOperation : int {
        Open = 1,
        Close = 2,
        Prepare = 3,
        Exec = 4,
        Step = 5,
        Finalize = 6,
        SetCipherKey = 7,
        IsTableExists = 8,
    };
    enum class InterfaceOperation : int {
        StatementHandle = 1,
        Insert = 2,
        Update = 3,
        Select = 4,
        Table = 5,
        ChainCall = 6,
        Delete = 7,
    };
    enum class CoreOperation : int {
        Prepare = 1,
        Exec = 2,
        Begin = 3,
        Commit = 4,
        Rollback = 5,
        GetThreadedHandle = 6,
        FlowOut = 7,
        Tokenize = 8,
    };
    enum class SystemCallOperation : int {
        Lstat = 1,
        Access = 2,
        Remove = 3,
        Link = 4,
        Unlink = 5,
        Mkdir = 6,
    };
    enum class RepairOperation : int {
        SaveMaster,
        LoadMaster,
        Repair,
    };
    //code
    enum class CoreCode : int {
        Misuse = 1,
        Exceed = 2,
    };
    enum class InterfaceCode : int {
        ORM = 1,
        Inconsistent = 2,
        //        NilObject = 3,
        Misuse = 4,
    };
    enum class GlobalCode : int {
        Warning = 1,
        Abort = 2,
    };

    typedef std::map<Key, ErrorValue> Infos;
    typedef std::function<void(const Error &)> ReportMethod;

    Error();
    Error(Error::Type type, int code, const Error::Infos &infos);

    Error::Type getType() const;
    int getCode() const;
    const Error::Infos &getInfos() const;
    bool isOK() const; //getCode()==0
    void reset();
    std::string description() const;

    void report() const;

    static const char *GetKeyName(Key key);
    static const char *GetTypeName(Type type);

    static void SetReportMethod(const ReportMethod &reportMethod);

    static void Report(Error::Type type,
                       int code,
                       const std::map<Key, ErrorValue> &infos,
                       Error *outError);

    //Convenience
    static void ReportSQLite(Tag tag,
                             const std::string &path,
                             HandleOperation operation,
                             int code,
                             const char *message,
                             Error *outError);
    static void ReportSQLite(Tag tag,
                             const std::string &path,
                             HandleOperation operation,
                             int code,
                             int extendedError,
                             const char *message,
                             Error *outError);
    static void ReportSQLite(Tag tag,
                             const std::string &path,
                             HandleOperation operation,
                             int code,
                             int extendedError,
                             const char *message,
                             const std::string &sql,
                             Error *outError);
    static void ReportCore(Tag tag,
                           const std::string &path,
                           CoreOperation operation,
                           CoreCode code,
                           const char *message,
                           Error *outError);
    static void ReportInterface(Tag tag,
                                const std::string &path,
                                InterfaceOperation operation,
                                InterfaceCode code,
                                const char *message,
                                Error *outError);
    static void ReportSystemCall(SystemCallOperation operation,
                                 const std::string &path,
                                 int code,
                                 const char *message,
                                 Error *outError);
    static void
    ReportSQLiteGlobal(int rc, const char *message, Error *outError);
    static void ReportRepair(const std::string &path,
                             RepairOperation operation,
                             int code,
                             Error *outError);
    static void Abort(const char *message, Error *outError = nullptr);
    static void Warning(const char *message, Error *outError = nullptr);

    static void setThreadedSlient(bool slient);

protected:
    int m_code;
    Error::Type m_type;
    Error::Infos m_infos;

    static std::shared_ptr<Error::ReportMethod> s_reportMethod;
    static ThreadLocal<bool> s_slient;
};

} //namespace WCDB

#endif /* error_hpp */
