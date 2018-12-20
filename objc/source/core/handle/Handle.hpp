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

#ifndef _WCDB_HANDLE_HPP
#define _WCDB_HANDLE_HPP

#include <WCDB/ErrorProne.hpp>
#include <WCDB/HandleNotification.hpp>
#include <WCDB/HandleStatement.hpp>
#include <WCDB/String.hpp>
#include <WCDB/WINQ.h>
#include <array>
#include <memory>
#include <mutex>
#include <set>

namespace WCDB {

class Handle : public ErrorProne {
#pragma mark - Initialize
public:
    Handle();

    Handle(const Handle &) = delete;
    Handle &operator=(const Handle &) = delete;
    virtual ~Handle() = 0;

    // Developers can call sqlite interfaces those WCDB does not provided currently by using this raw handle.
    // Note that this is not tested, which means that it may result in an unpredictable behavior.
    // Usage:
    //  e.g. 1. sqlite3** rawHandle = (sqlite3**)getRawHandle()
    //  e.g. 2. sqlite3_open(rawHandle, ...)
    //  e.g. 3. sqlite3_exec(*rawHandle, ...)
    void **getRawHandle();

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
    void setPath(const String &path);
    const String &getPath() const;

    static String getSHMSubfix();
    static String getWALSubfix();
    static String getJournalSubfix();

private:
    String m_path;

#pragma mark - Basic
public:
    bool open();
    void close();
    bool isOpened() const;

    long long getLastInsertedRowID();
    const char *getErrorMessage();
    int getExtendedErrorCode();
    int getResultCode();
    int getChanges();
    bool isReadonly();
    bool isInTransaction();

    void interrupt(); // It's thread safe.

protected:
    bool execute(const String &sql);

#pragma mark - Statement
public:
    virtual bool execute(const Statement &statement);

    virtual bool prepare(const Statement &statement);
    virtual bool isPrepared();
    virtual void finalize();

    virtual bool step(bool &done);
    bool step();
    virtual void reset();

    using Integer32 = HandleStatement::Integer32;
    using Integer64 = HandleStatement::Integer64;
    using Text = HandleStatement::Text;
    using Float = HandleStatement::Float;
    using BLOB = HandleStatement::BLOB;

    virtual void bindInteger32(const Integer32 &value, int index);
    virtual void bindInteger64(const Integer64 &value, int index);
    virtual void bindDouble(const Float &value, int index);
    virtual void bindText(const Text &value, int index);
    virtual void bindBLOB(const BLOB &value, int index);
    virtual void bindNull(int index);

    virtual Integer32 getInteger32(int index);
    virtual Integer64 getInteger64(int index);
    virtual Float getDouble(int index);
    virtual Text getText(int index);
    virtual BLOB getBLOB(int index);

    virtual ColumnType getType(int index);
    virtual const UnsafeString getOriginColumnName(int index);
    virtual const UnsafeString getColumnName(int index);
    virtual const UnsafeString getColumnTableName(int index);

    virtual bool isStatementReadonly();
    virtual int getColumnCount();

protected:
    HandleStatement *getStatement();
    void returnStatement(HandleStatement *handleStatement);
    HandleStatement *m_mainStatement;

private:
    std::list<HandleStatement> m_handleStatements;

#pragma mark - Convenient
public:
    std::pair<bool, bool> tableExists(const String &table);
    std::pair<bool, std::set<String>>
    getColumns(const Schema &schema, const String &table);

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
    std::pair<bool, std::set<String>> getValues(const Statement &statement, int index);

private:
    static const String &savepointPrefix();
    int m_nestedLevel;

#pragma mark - Cipher
public:
    bool setCipherKey(const UnsafeData &data);

#pragma mark - Notification
public:
    typedef HandleNotification::PerformanceNotification PerformanceNotification;
    void setNotificationWhenPerformanceTraced(const String &name,
                                              const PerformanceNotification &onTraced);

    typedef HandleNotification::SQLNotification SQLNotification;
    void setNotificationWhenSQLTraced(const String &name, const SQLNotification &onTraced);

    typedef HandleNotification::CommittedNotification CommittedNotification;
    void setNotificationWhenCommitted(int order,
                                      const String &name,
                                      const CommittedNotification &onCommitted);

    typedef HandleNotification::WillCheckpointNotification WillCheckpointNotification;
    bool setNotificationWhenWillCheckpoint(int order,
                                           const String &name,
                                           const WillCheckpointNotification &willCheckpoint);
    bool unsetNotificationWhenWillCheckpoint(const String &name);
    typedef HandleNotification::CheckpointedNotification CheckpointedNotification;
    bool setNotificationWhenCheckpointed(const String &name,
                                         const CheckpointedNotification &checkpointed);
    void unsetNotificationWhenCommitted(const String &name);

private:
    HandleNotification m_notification;

#pragma mark - Error
protected:
    // if code >= 0, then the level of error with the specified code will be marked as ignored
    // if code < 0, then the level of all errors will be marked as ignored
    // when the error is marked as ignored, it will not be set to m_error and error() will return true
    bool error(int rc, const String &sql = "");
    void markErrorAsIgnorable(int codeToBeIgnored);
    void markErrorAsUnignorable();

private:
    void setupAndNotifyError(Error &error, int rc, const String &sql);
    int m_codeToBeIgnored;
};

} //namespace WCDB

#endif /* _WCDB_HANDLE_HPP */
