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

#include <WCDB/ErrorProne.hpp>
#include <WCDB/HandleNotification.hpp>
#include <WCDB/HandleStatement.hpp>
#include <WCDB/WINQ.h>
#include <array>
#include <memory>
#include <mutex>
#include <set>
#include <string>

namespace WCDB {

class Handle : public ErrorProne {
#pragma mark - Initialize
public:
    Handle(const std::string &path);

    Handle() = delete;
    Handle(const Handle &) = delete;
    Handle &operator=(const Handle &) = delete;
    virtual ~Handle();

protected:
    friend class HandleRelated;
    void *m_handle;

#pragma mark - Global
public:
    static void enableMultithread();
    static void enableMemoryStatus(bool enable);
    static void setMemoryMapSize(int64_t defaultSizeLimit, int64_t maximumAllowedSizeLimit);

    typedef void (*Log)(void *, int, const char *);
    static void setNotificationForLog(const Log &log);

    typedef int (*VFSOpen)(const char *, int, int);
    static void setNotificationWhenVFSOpened(const VFSOpen &vfsOpen);

#pragma mark - Path
public:
    const std::string path;

    static std::string getSHMSubfix();
    static std::string getWALSubfix();
    static std::string getJournalSubfix();

#pragma mark - Basic
public:
    bool open();
    void close();
    bool isOpened() const;

    virtual bool execute(const Statement &statement);

    long long getLastInsertedRowID();
    const char *getErrorMessage();
    int getExtendedErrorCode();
    int getResultCode();
    int getChanges();
    bool isReadonly();
    bool isInTransaction();

#pragma mark - Statement
public:
    virtual bool prepare(const Statement &statement);
    virtual void finalize();
    virtual bool step(bool &done);
    bool isPrepared() const;

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

#pragma mark - Convenient
public:
    std::pair<bool, bool> tableExists(const TableOrSubquery &table);
    std::pair<bool, std::set<std::string>>
    getUnorderedColumnsWithTable(const std::string &tableName,
                                 const std::string &schemaName = Schema::main());

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
    std::pair<bool, std::set<std::string>>
    getUnorderedValues(const Statement &statement, int index);

private:
    static const std::string &savepointPrefix();
    int m_nestedLevel;

#pragma mark - Cipher
public:
    bool setCipherKey(const UnsafeData &data);

#pragma mark - Notification
public:
    typedef HandleNotification::PerformanceNotification PerformanceNotification;
    void setNotificationWhenPerformanceTraced(const std::string &name,
                                              const PerformanceNotification &onTraced);

    typedef HandleNotification::SQLNotification SQLNotification;
    void setNotificationWhenSQLTraced(const std::string &name, const SQLNotification &onTraced);

    typedef HandleNotification::CommittedNotification CommittedNotification;
    void setNotificationWhenCommitted(int order,
                                      const std::string &name,
                                      const CommittedNotification &onCommitted);

    typedef HandleNotification::WillCheckpointNotification WillCheckpointNotification;
    bool setNotificationWhenWillCheckpoint(int order,
                                           const std::string &name,
                                           const WillCheckpointNotification &willCheckpoint);
    bool unsetNotificationWhenWillCheckpoint(const std::string &name);
    typedef HandleNotification::CheckpointedNotification CheckpointedNotification;
    bool setNotificationWhenCheckpointed(const std::string &name,
                                         const CheckpointedNotification &checkpointed);
    void unsetNotificationWhenCommitted(const std::string &name);

private:
    HandleNotification m_notification;

#pragma mark - Error
protected:
    void setError(int rc, const std::string &sql = "");

    // if code >= 0, then the level of error with the specified code will be marked as ignored
    // if code < 0, then the level of all errors will be marked as ignored
    // when the error is marked as ignored, it will not be set to m_error
    void markErrorAsIgnorable(int codeToBeIgnored);
    void markErrorAsUnignorable();

private:
    void doSetError(Error &error, int rc, const std::string &sql);
    int m_codeToBeIgnored;
};

} //namespace WCDB

#endif /* Handle_hpp */
