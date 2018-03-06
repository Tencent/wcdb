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

#ifndef Handle_hpp
#define Handle_hpp

#include <WCDB/Error.hpp>
#include <WCDB/String.hpp>
#include <WCDB/WINQ.h>
#include <array>
#include <map>
#include <memory>
#include <mutex>
#include <string>

typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;

namespace WCDB {

class Handle;

//{{sql->count}, cost}
typedef std::function<void(const std::map<const std::string, unsigned int> &,
                           const int64_t &)>
    PerformanceTraceCallback;

typedef std::function<void(const std::string &)> SQLTraceCallback;

typedef std::function<void(Handle *, int, void *)> CommittedCallback;

class Handle {
public:
    static std::shared_ptr<Handle> handleWithPath(const std::string &path);
    ~Handle();

    const std::string path;
    std::string getSHMPath() const;
    std::string getWALPath() const;
    std::string getJournalPath() const;
    std::string getBackupPath() const;
    std::list<std::string> getPaths() const;

    static const std::string &getSHMSubfix();
    static const std::string &getWALSubfix();
    static const std::string &getJournalSubfix();
    static const std::string &getBackupSubfix();
    static const std::array<std::string, 5> &getSubfixs();

    void setTag(Tag tag);
    Tag getTag() const;

    bool open();
    void close();

    bool execute(const Statement &statement);

    bool prepare(const Statement &statement);
    bool step(bool &done);
    bool step();
    void finalize();
    bool isStatementReadonly();
    bool isPrepared();

    void reset();

    //bind, index begin with 1
    void bindInteger32(
        const ColumnTypeInfo<ColumnType::Integer32>::UnderlyingType &value,
        int index);
    void bindInteger64(
        const ColumnTypeInfo<ColumnType::Integer64>::UnderlyingType &value,
        int index);
    void
    bindDouble(const ColumnTypeInfo<ColumnType::Float>::UnderlyingType &value,
               int index);
    void bindText(const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &value,
                  int index);
    void bindBLOB(const ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType &value,
                  int index);
    void bindNull(int index);

    ColumnTypeInfo<ColumnType::Integer32>::UnderlyingType
    getInteger32(int index);
    ColumnTypeInfo<ColumnType::Integer64>::UnderlyingType
    getInteger64(int index);
    ColumnTypeInfo<ColumnType::Float>::UnderlyingType getDouble(int index);
    ColumnTypeInfo<ColumnType::Text>::UnderlyingType getText(int index);
    ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType getBLOB(int index);

    ColumnType getType(int index);

    int getColumnCount();
    const char *getColumnName(int index);
    const char *getColumnTableName(int index);

    bool setCipherKey(const void *data, int size);

    long long getLastInsertedRowID();

    const char *getErrorMessage();
    int getExtendedErrorCode();
    int getResultCode() const;

    int getChanges();

    bool isReadonly();

    bool isInTransaction();

    void setPerformanceTrace(const PerformanceTraceCallback &trace);
    void setSQLTrace(const SQLTraceCallback &trace);

    bool backup(const void *key = nullptr, unsigned int length = 0);
    bool recoverFromPath(const std::string &corruptedDBPath,
                         int pageSize,
                         const void *backupKey,
                         unsigned int backupKeyLength,
                         const void *databaseKey,
                         unsigned int databaseKeyLength);

    void registerCommittedHook(const CommittedCallback &onCommitted,
                               void *info);

    const Error &getError();
    void resetError();
    void skipError(bool skip);

protected:
    Handle(const std::string &path);
    Handle(const Handle &) = delete;
    Handle &operator=(const Handle &) = delete;

    sqlite3 *m_handle;
    sqlite3_stmt *m_stmt;

    Error m_error; // error will be set only for those bool returned interface
    bool m_skipError;
    void setupError(Error &error);
    void setupAndReportError(Error &error);
    void setupAndReportErrorWithSQL(Error &error, const std::string &sql);
    void setupAndReportErrorWithPath(Error &error, const std::string &path);

    void reportPerformance();
    void addPerformanceTrace(const std::string &sql, const int64_t &cost);
    bool shouldPerformanceAggregation() const;
    void reportSQL(const std::string &sql);

    typedef struct {
        CommittedCallback onCommitted;
        void *info;
        Handle *handle;
    } CommittedHookInfo;
    CommittedHookInfo m_committedHookInfo;

    void setupTrace();
    PerformanceTraceCallback m_performanceTrace;
    SQLTraceCallback m_sqlTrace;
    std::map<const std::string, unsigned int> m_footprint;
    int64_t m_cost;
    bool m_aggregation;
};

} //namespace WCDB

#endif /* Handle_hpp */
