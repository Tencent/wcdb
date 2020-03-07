//
// Created by sanhuazhang on 2019/05/19
//

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

#pragma once

#include <WCDB/ColumnMeta.hpp>
#include <WCDB/ErrorProne.hpp>
#include <WCDB/HandleNotification.hpp>
#include <WCDB/HandleStatement.hpp>
#include <WCDB/StringView.hpp>
#include <WCDB/WINQ.h>
#include <set>
#include <vector>

namespace WCDB {

class AbstractHandle : public ErrorProne {
#pragma mark - Initialize
public:
    AbstractHandle();

    AbstractHandle(const AbstractHandle &) = delete;
    AbstractHandle &operator=(const AbstractHandle &) = delete;
    virtual ~AbstractHandle() override = 0;

private:
    friend class HandleRelated;
    sqlite3 *getRawHandle();
    sqlite3 *m_handle;

#pragma mark - Path
public:
    void setPath(const UnsafeStringView &path);
    void clearPath();
    const StringView &getPath() const;

    static StringView shmPathOfDatabase(const UnsafeStringView &database);
    static StringView walPathOfDatabase(const UnsafeStringView &database);
    static StringView journalPathOfDatabase(const UnsafeStringView &database);

private:
    StringView m_path;

#pragma mark - Basic
public:
    virtual bool open();
    virtual void close();
    bool isOpened() const;

    long long getLastInsertedRowID();
    //    const char *getErrorMessage();
    //    Error::ExtCode getExtendedErrorCode();
    //    Error::Code getResultCode();
    int getChanges();
    bool isReadonly();
    bool isInTransaction();

protected:
    bool executeSQL(const UnsafeStringView &sql);
    bool executeStatement(const Statement &statement);

#pragma mark - Statement
public:
    virtual HandleStatement *getStatement();
    virtual void returnStatement(HandleStatement *handleStatement);

protected:
    virtual void finalizeStatements();
    
private:
    std::list<HandleStatement> m_handleStatements;

#pragma mark - Meta
public:
    std::optional<bool> ft3TokenizerExists(const UnsafeStringView &tokenizer);

    std::optional<bool> tableExists(const UnsafeStringView &table);
    std::optional<bool> tableExists(const Schema &schema, const UnsafeStringView &table);

    std::optional<std::set<StringView>>
    getColumns(const Schema &schema, const UnsafeStringView &table);
    std::optional<std::set<StringView>> getColumns(const UnsafeStringView &table);

    std::optional<std::vector<ColumnMeta>>
    getTableMeta(const Schema &schema, const UnsafeStringView &table);

protected:
    std::optional<std::set<StringView>> getValues(const Statement &statement, int index);

#pragma mark - Transaction
public:
    bool beginTransaction();
    bool commitOrRollbackTransaction();
    void rollbackTransaction();

    bool beginNestedTransaction();
    bool commitOrRollbackNestedTransaction();
    void rollbackNestedTransaction();

    void markErrorNotAllowedWithinTransaction();
    bool isErrorAllowedWithinTransaction() const;
    bool checkMainThreadBusyRetry() const;

private:
    int m_transactionLevel;
    enum class TransactionError {
        Allowed,
        NotAllowed,
        Fatal,
    } m_transactionError;
    static StringView getSavepointName(int transactionLevel);

#pragma mark - Interface
public:
    void setCipherKey(const UnsafeData &data);

    void enableExtendedResultCodes(bool enable);

    enum class CheckpointMode {
        Passive = 0,
        Full,
        Restart,
        Truncate,
    };
    bool checkpoint(CheckpointMode mode);
    void disableCheckpointWhenClosing(bool disable);

#pragma mark - Notification
public:
    typedef HandleNotification::PerformanceNotification PerformanceNotification;
    void setNotificationWhenPerformanceTraced(const UnsafeStringView &name,
                                              const PerformanceNotification &onTraced);

    typedef HandleNotification::SQLNotification SQLNotification;
    void setNotificationWhenSQLTraced(const UnsafeStringView &name,
                                      const SQLNotification &onTraced);

    typedef HandleNotification::CommittedNotification CommittedNotification;
    void setNotificationWhenCommitted(int order,
                                      const UnsafeStringView &name,
                                      const CommittedNotification &onCommitted);

    typedef HandleNotification::CheckpointedNotification CheckpointedNotification;
    void setNotificationWhenCheckpointed(const UnsafeStringView &name,
                                         const CheckpointedNotification &checkpointed);
    void unsetNotificationWhenCommitted(const UnsafeStringView &name);

    typedef HandleNotification::BusyNotification BusyNotification;
    void setNotificationWhenBusy(const BusyNotification &busyNotification);

private:
    HandleNotification m_notification;

#pragma mark - Error
public:
    // call it as push/pop in stack structure.
    void markErrorAsIgnorable(Error::Code ignorableCode);
    void markErrorAsUnignorable(int count = 1);

private:
    // The level of error will be "Ignore" if it's marked as ignorable.
    // But the return value will be still false.
    bool APIExit(int rc);
    bool APIExit(int rc, const UnsafeStringView &sql);
    bool APIExit(int rc, const char *sql);

    void notifyError(int rc, const char *sql);

    std::vector<int> m_ignorableCodes;

#pragma mark - Suspend
public:
    void suspend(bool suspend);                     // thread-safe
    void markAsCanBeSuspended(bool canBeSuspended); // thread-safe, default to false
protected:
    virtual void doSuspend(bool suspend);

private:
    std::atomic<bool> m_canBeSuspended;
};

} //namespace WCDB
