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

#include <WCDB/Optional.hpp>
#include <WCDB/String.hpp>
#include <WCDB/ThreadLocal.hpp>
#include <climits>
#include <functional>
#include <map>
#include <string>

namespace WCDB {

enum Tag {
    Invalid = 0,
};
typedef enum Tag Tag;

class Error {
public:
    class Value {
    public:
        enum class Type : int {
            Int,
            String,
        };
        Value(int value);
        Value(const char *value);
        Value(const std::string &value);

        int getIntValue() const;
        std::string getStringValue() const;
        Value::Type getType() const;

    protected:
        Value::Type m_type;
        int m_intValue;
        std::string m_stringValue;
    };

public:
    enum class Key : int {
        Tag = 1,
        Operation = 2,
        ExtendedCode = 3,
        Message = 4,
        SQL = 5,
        Path = 6,
    };
    static constexpr const char *GetKeyName(Key key)
    {
        switch (key) {
            case Error::Key::Operation:
                return "Op";
            case Error::Key::ExtendedCode:
                return "ExtCode";
            case Error::Key::Message:
                return "Msg";
            case Error::Key::SQL:
                return "SQL";
            case Error::Key::Tag:
                return "Tag";
            case Error::Key::Path:
                return "Path";
        }
    }

    enum class Type : int {
        NotSet = 0,
        Warning = 1,
        Abort = 2,
        SQLiteGlobal = 3,
        SQLite = 4,
        SystemCall = 5,
        WCDB = 6,
        RepairKit = 7,
    };
    static constexpr const char *GetTypeName(Type type)
    {
        switch (type) {
            case Error::Type::SQLite:
                return "SQLite";
            case Error::Type::SystemCall:
                return "SystemCall";
            case Error::Type::Abort:
                return "Abort";
            case Error::Type::Warning:
                return "Warning";
            case Error::Type::SQLiteGlobal:
                return "SQLiteGlobal";
            case Error::Type::RepairKit:
                return "RepairKit";
            case Error::Type::WCDB:
                return "WCDB";
            default:
                return "";
        }
    }

    enum class Operation : int {
        SystemCallLstat = 1,
        SystemCallAccess = 2,
        SystemCallRemove = 3,
        SystemCallLink = 4,
        SystemCallUnlink = 5,
        SystemCallMkdir = 6,
        RepairKitBackup = 1 | (1 << 8),
        RepairKitRepair = 2 | (1 << 8),
    };

    typedef std::map<Error::Key, Value> Infos;
    typedef std::function<void(const Error &)> ReportMethod;

    Error();

    void setError(Type type, int code);
    void setOperation(Operation operation);
    void setTag(Tag tag);
    void setExtendedCode(int extendedCode);
    void setMessage(const std::string &message);
    void setSQL(const std::string &sql);
    void setPath(const std::string &path);

    Error::Type getType() const;
    int getCode() const;
    Optional<Tag> getTag() const;
    Optional<int> getOperationValue() const;
    Optional<int> getExtendedCode() const;
    Optional<std::string> getMessage() const;
    Optional<std::string> getSQL() const;
    Optional<std::string> getPath() const;
    const Error::Infos &getInfos() const;

    bool isOK() const; //getCode()==0
    void reset();
    void clear();
    std::string description() const;

    static void SetReportMethod(const ReportMethod &reportMethod);
    static void ResetReportMethod();

    void report() const;
    static void Abort(const std::string &message);
    static void Warning(const std::string &message);

    static void setThreadedSlient(bool slient);

protected:
    int m_code;
    Error::Type m_type;
    Error::Infos m_infos;

    static std::shared_ptr<ReportMethod> s_reportMethod;
    static ReportMethod s_builtinErrorReport;
    static ThreadLocal<bool> s_slient;
};

} //namespace WCDB

#endif /* Error_hpp */
