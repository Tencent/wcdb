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

#include "ColumnMeta.hpp"
#include "DecorativeHandleStatement.hpp"
#include "ErrorProne.hpp"
#include "HandleNotification.hpp"
#include "StringView.hpp"
#include "TableAttribute.hpp"
#include "Tag.hpp"
#include "WCDBOptional.hpp"
#include "WINQ.h"
#include <set>
#include <tuple>
#include <vector>

namespace WCDB {

class ScalarFunctionConfig;

class AbstractHandle : public ErrorProne {
#pragma mark - Initialize
public:
    AbstractHandle();

    AbstractHandle(const AbstractHandle &) = delete;
    AbstractHandle &operator=(const AbstractHandle &) = delete;
    virtual ~AbstractHandle() override = 0;

private:
    friend class ScalarFunctionConfig;
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

    void enableWriteMainDB(bool enable);
    bool canWriteMainDB();

    long long getLastInsertedRowID();
    //    const char *getErrorMessage();
    //    Error::ExtCode getExtendedErrorCode();
    //    Error::Code getResultCode();
    int getChanges();
    int getTotalChange();
    bool isReadonly();
    bool isInTransaction();

    void setTag(Tag tag);
    Tag getTag();

    bool executeStatement(const Statement &statement);
    bool executeSQL(const UnsafeStringView &sql);

protected:
    int m_customOpenFlag;
    Tag m_tag;

#pragma mark - Statement
public:
    virtual DecorativeHandleStatement *
    getStatement(const UnsafeStringView &skipDecorator = UnsafeStringView());
    virtual void returnStatement(HandleStatement *handleStatement);
    virtual void resetAllStatements();
    virtual void finalizeStatements();
    HandleStatement *getOrCreatePreparedStatement(const Statement &statement);
    HandleStatement *getOrCreatePreparedStatement(const UnsafeStringView &sql);

private:
    HandleStatement *getOrCreateStatement(const UnsafeStringView &sql);
    std::list<DecorativeHandleStatement> m_handleStatements;
    StringViewMap<DecorativeHandleStatement *> m_preparedStatements;

#pragma mark - Meta
public:
    Optional<bool> ft3TokenizerExists(const UnsafeStringView &tokenizer);

    Optional<bool> tableExists(const UnsafeStringView &table);
    Optional<bool> tableExists(const Schema &schema, const UnsafeStringView &table);

    virtual bool
    addColumn(const Schema &schema, const UnsafeStringView &table, const ColumnDef &column);

    virtual Optional<StringViewSet>
    getColumns(const Schema &schema, const UnsafeStringView &table);

    Optional<std::vector<ColumnMeta>>
    getTableMeta(const Schema &schema, const UnsafeStringView &table);
    Optional<StringViewSet> getValues(const Statement &statement, int index);
    Optional<TableAttribute>
    getTableAttribute(const Schema &schema, const UnsafeStringView &tableName);
    bool configAutoIncrement(const UnsafeStringView &tableName);

    bool getSchemaInfo(int &memoryUsed, int &tableCount, int &indexCount, int &triggerCount);

#pragma mark - Transaction
public:
    virtual bool beginTransaction();
    bool commitOrRollbackTransaction();
    virtual void rollbackTransaction();

    void markErrorNotAllowedWithinTransaction();
    bool isErrorAllowedWithinTransaction() const;
    bool checkMainThreadBusyRetry() const;

protected:
    void cacheCurrentTransactionError();
    void resumeCacheTransactionError();
    virtual bool commitTransaction();

private:
    int m_transactionLevel;
    enum class TransactionError {
        Allowed,
        NotAllowed,
        Fatal,
    } m_transactionError;
    TransactionError m_cacheTransactionError;
    static StringView getSavepointName(int transactionLevel);

#pragma mark - Wal
public:
    void enableExtendedResultCodes(bool enable);

    enum class CheckpointMode {
        Passive = 0,
        Full,
        Restart,
        Truncate,
    };
    bool checkpoint(CheckpointMode mode = CheckpointMode::Passive);
    void disableCheckpointWhenClosing(bool disable);
    void setWALFilePersist(int persist);
    bool setCheckPointLock(bool enable);

#pragma mark - Notification
public:
    typedef HandleNotification::PerformanceInfo PerformanceInfo;
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

    typedef HandleNotification::CheckPointNotification CheckPointNotification;
    void setNotificationWhenCheckpointed(const UnsafeStringView &name,
                                         Optional<CheckPointNotification> notification);
    void unsetNotificationWhenCommitted(const UnsafeStringView &name);

    typedef HandleNotification::BusyNotification BusyNotification;
    void setNotificationWhenBusy(const BusyNotification &busyNotification);

    typedef HandleNotification::TableModifiedNotification TableModifiedNotification;
    void setNotificationWhenTableModified(const UnsafeStringView &name,
                                          const TableModifiedNotification &tableModifiedNotification);

    void postTableNotification(const UnsafeStringView &newTable,
                               const UnsafeStringView &modifiedTable);
    bool needMonitorTable();
    void setTableMonitorEnable(bool enable);

    void setFullSQLTraceEnable(bool enable);
    bool isFullSQLEnable();
    void postSQLNotification(const UnsafeStringView &sql, const UnsafeStringView &info);

    void setBusyTraceEnable(bool enable);
    bool isBusyTraceEnable() const;
    void setCurrentSQL(const UnsafeStringView &sql);
    void resetCurrentSQL(const UnsafeStringView &sql);
    StringView getCurrentSQL() const;
    void setActiveThreadId(uint64_t tid);
    bool isUsingInThread(uint64_t tid) const;

private:
    HandleNotification m_notification;
    bool m_tableMonitorForbidden;
    bool m_fullSQLTrace;
    mutable std::mutex m_lock;
    bool m_busyTrace;
    StringView m_currentSQL;
    uint64_t m_tid;

#pragma mark - Error
public:
    void notifyError(Error::Code rc,
                     const UnsafeStringView &sql,
                     const UnsafeStringView &msg = UnsafeStringView());
    void notifyError(int rc,
                     const UnsafeStringView &sql,
                     const UnsafeStringView &msg = UnsafeStringView());
    // call it as push/pop in stack structure.
    void markErrorAsIgnorable(Error::Code ignorableCode);
    void markErrorAsUnignorable(int count = 1);
    void setThreadedErrorProne(ThreadedErrorProne *threadedErrorProne);

private:
    // The level of error will be "Ignore" if it's marked as ignorable.
    // But the return value will be still false.
    bool APIExit(int rc);
    bool APIExit(int rc, const UnsafeStringView &sql);
    bool APIExit(int rc, const char *sql);

    ThreadedErrorProne *m_threadErrorProne;
    std::vector<int> m_ignorableCodes;

#pragma mark - Suspend
public:
    void suspend(bool suspend);                     // thread-safe
    void markAsCanBeSuspended(bool canBeSuspended); // thread-safe, default to false
    bool isSuspended() const;

protected:
    virtual void doSuspend(bool suspend);

private:
    std::atomic<bool> m_canBeSuspended;

#pragma mark - Cancellation Signal
public:
    typedef std::shared_ptr<volatile bool> CancellationSignal;
    void attachCancellationSignal(CancellationSignal signal);
    void detachCancellationSignal();

private:
    static int progressHandlerCallback(void *ctx);
    CancellationSignal m_cancelSignal;

#pragma mark - Cipher
public:
    size_t getCipherPageSize();
    void *getCipherContext();
    virtual bool setCipherKey(const UnsafeData &data);
    Data getRawCipherKey(const Schema &schema = Schema::main());
    bool hasCipher() const;
    bool setCipherPageSize(int pageSize);
    StringView getCipherSalt();
    bool setCipherSalt(const UnsafeStringView &salt);

#pragma mark - Extra
public:
    void tryPreloadAllPages();
};

} //namespace WCDB
