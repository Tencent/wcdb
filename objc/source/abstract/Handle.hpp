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

#include <WCDB/HandleError.hpp>
#include <WCDB/HandleStatement.hpp>
#include <WCDB/Tracer.hpp>
#include <WCDB/WINQ.h>
#include <array>
#include <memory>
#include <mutex>
#include <set>
#include <string>

#pragma GCC visibility push(hidden)

namespace WCDB {

class Handle {
#pragma mark - Initialize
public:
    using Tag = HandleError::Tag;

    static std::shared_ptr<Handle> handleWithPath(const std::string &path,
                                                  Tag tag);
    Handle() = delete;
    Handle(const Handle &) = delete;
    Handle &operator=(const Handle &) = delete;

protected:
    Handle(const std::string &path, Tag tag);
    void *m_handle;

#pragma mark - Path
public:
    const std::string path;

    static const std::string &getSHMSubfix();
    static const std::string &getWALSubfix();
    static const std::string &getJournalSubfix();
    static const std::string &getBackupSubfix();
    static const std::array<std::string, 5> &getSubfixs();

#pragma mark - Basic
public:
    void setTag(Tag tag);
    Tag getTag() const;

    bool open();
    void close();
    virtual bool execute(const Statement &statement);

    long long getLastInsertedRowID();
    const char *getErrorMessage();
    int getExtendedErrorCode();
    int getResultCode() const;
    int getChanges();
    bool isReadonly();
    bool isInTransaction();

    typedef std::function<void(Handle *, int, void *)> CommittedCallback;
    void setCommittedHook(const CommittedCallback &onCommitted, void *info);

protected:
    typedef struct {
        CommittedCallback onCommitted;
        void *info;
        Handle *handle;
    } CommittedHookInfo;
    CommittedHookInfo m_committedHookInfo;

#pragma mark - Statement
public:
    virtual bool prepare(const Statement &statement);
    virtual void finalize();
    virtual bool step(bool &done);

    bool step();
    virtual void reset();

    bool isStatementReadonly();
    bool isPrepared();

    using Integer32 = HandleStatement::Integer32;
    using Integer64 = HandleStatement::Integer64;
    using Text = HandleStatement::Text;
    using Float = HandleStatement::Float;
    using BLOB = HandleStatement::BLOB;

    virtual void bindInteger32(const Integer32 &value, int index);
    virtual void bindInteger64(const Integer64 &value, int index);
    virtual void bindDouble(const Float &value, int index);
    void bindText(const Text &value, int index);
    virtual void bindText(const Text &value, int length, int index);

    virtual void bindBLOB(const BLOB &value, int index);
    virtual void bindNull(int index);

    Integer32 getInteger32(int index);
    Integer64 getInteger64(int index);
    Float getDouble(int index);
    Text getText(int index);
    BLOB getBLOB(int index);

    ColumnType getType(int index);
    int getColumnCount();
    const char *getColumnName(int index);
    const char *getColumnTableName(int index);

protected:
    HandleStatement m_handleStatement;
    bool prepare(const Statement &statement, HandleStatement &handleStatement);
    bool step(HandleStatement &handleStatement, bool &done);

#pragma mark - Convenient
public:
    std::pair<bool, bool> isTableExists(const TableOrSubquery &table);
    std::pair<bool, std::set<std::string>> getUnorderedColumnsWithTable(
        const std::string &tableName,
        const std::string &schemaName = Schema::main());
    std::pair<bool, std::set<std::string>> getUnorderedAttachedSchemas();

    typedef std::function<bool(Handle *)> TransactionCallback;

    bool beginTransaction();
    bool commitOrRollbackTransaction();
    void rollbackTransaction();
    bool runTransaction(const TransactionCallback &transaction);

    bool beginNestedTransaction();
    bool commitOrRollbackNestedTransaction();
    void rollbackNestedTransaction();
    bool runNestedTransaction(const TransactionCallback &transaction);

protected:
    void discardableExecute(const Statement &statement);
    std::pair<bool, std::set<std::string>>
    getUnorderedValues(const Statement &statement, int index);
    static const std::string &savepointPrefix();
    int m_nestedLevel;

#pragma mark - Cipher
public:
    bool setCipherKey(const void *data, int size);

#pragma mark - Trace
public:
    typedef std::function<void(
        Tag, const Tracer::Footprints &, const int64_t &)>
        PerformanceTraceCallback;
    void tracePerformance(const PerformanceTraceCallback &trace);

    using SQLTraceCallback = Tracer::SQLTraceCallback;
    void traceSQL(const SQLTraceCallback &trace);

protected:
    Tracer m_tracer;

#pragma mark - Repair Kit
public:
    bool backup(const NoCopyData &data);
    bool recoverFromPath(const std::string &corruptedDBPath,
                         int pageSize,
                         const NoCopyData &backupCipher,
                         const NoCopyData &databaseCipher);

#pragma mark - Error
public:
    const HandleError &getError() const;

protected:
    using Error = HandleError;
    using Operation = HandleError::Operation;
    void setupError();
    HandleError m_error;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* Handle_hpp */
