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

#include "AbstractHandle.hpp"
#include "Assertion.hpp"
#include "CoreConst.h"
#include "Notifier.hpp"
#include "Path.hpp"
#include "SQLite.h"
#include "StringView.hpp"

namespace WCDB {

#pragma mark - Initialize
AbstractHandle::AbstractHandle()
: m_handle(nullptr)
, m_customOpenFlag(0)
, m_transactionLevel(0)
, m_transactionError(TransactionError::Allowed)
, m_cacheTransactionError(TransactionError::Allowed)
, m_notification(this)
, m_tableMonitorForbidden(false)
, m_canBeSuspended(false)
{
}

AbstractHandle::~AbstractHandle()
{
    WCTAssert(m_transactionError == TransactionError::Allowed);
    close();
}

sqlite3 *AbstractHandle::getRawHandle()
{
    WCTAssert(isOpened());
    return m_handle;
}

#pragma mark - Path
void AbstractHandle::setPath(const UnsafeStringView &path)
{
    if (m_path != path) {
        close();
        m_path = path;
        m_error.infos.insert_or_assign(ErrorStringKeyPath, m_path);
    }
}

void AbstractHandle::clearPath()
{
    close();
    m_path.clear();
}

const StringView &AbstractHandle::getPath() const
{
    return m_path;
}

StringView AbstractHandle::shmPathOfDatabase(const UnsafeStringView &database)
{
    return Path::addExtention(database, "-shm");
}

StringView AbstractHandle::walPathOfDatabase(const UnsafeStringView &database)
{
    return Path::addExtention(database, "-wal");
}

StringView AbstractHandle::journalPathOfDatabase(const UnsafeStringView &database)
{
    return Path::addExtention(database, "-journal");
}

#pragma mark - Basic
bool AbstractHandle::open()
{
    bool succeed = true;
    if (!isOpened()) {
        if (m_customOpenFlag == 0) {
            succeed = APIExit(sqlite3_open_v2(
            m_path.data(), &m_handle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_MAINDB_READONLY, 0));
        } else {
            succeed
            = APIExit(sqlite3_open_v2(m_path.data(), &m_handle, m_customOpenFlag, 0));
        }
        if (!succeed) {
            m_handle = nullptr;
        }
    }
    return succeed;
}

bool AbstractHandle::isOpened() const
{
    return m_handle != nullptr;
}

void AbstractHandle::close()
{
    if (isOpened()) {
        finalizeStatements();
        m_transactionLevel = 0;
        m_notification.purge();
        APIExit(sqlite3_close_v2(m_handle));
        m_handle = nullptr;
    }
}

bool AbstractHandle::executeSQL(const UnsafeStringView &sql)
{
    // use seperated sqlite3_exec to get more information
    WCTAssert(isOpened());
    HandleStatement handleStatement(this);
    bool succeed = handleStatement.prepare(sql);
    if (succeed) {
        succeed = handleStatement.step();
        handleStatement.finalize();
    }
    return succeed;
}

bool AbstractHandle::executeStatement(const Statement &statement)
{
    return executeSQL(statement.getDescription());
}

long long AbstractHandle::getLastInsertedRowID()
{
    WCTAssert(isOpened());
    return sqlite3_last_insert_rowid(m_handle);
}

int AbstractHandle::getTotalChange()
{
    WCTAssert(isOpened());
    return sqlite3_total_changes(m_handle);
}

void AbstractHandle::enableWriteMainDB(bool enable)
{
    if (enable) {
        m_customOpenFlag = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    } else {
        m_customOpenFlag = 0;
    }
}

bool AbstractHandle::canWriteMainDB()
{
    return m_customOpenFlag & SQLITE_OPEN_READWRITE;
}

int AbstractHandle::getChanges()
{
    WCTAssert(isOpened());
    return sqlite3_changes(m_handle);
}

bool AbstractHandle::isReadonly()
{
    WCTAssert(isOpened());
    return sqlite3_db_readonly(m_handle, NULL) == 1;
}

bool AbstractHandle::isInTransaction()
{
    WCTAssert(isOpened());
    return sqlite3_get_autocommit(m_handle) == 0;
}

#pragma mark - Statement
HandleStatement *AbstractHandle::getStatement()
{
    m_handleStatements.push_back(HandleStatement(this));
    m_handleStatements.back().enableAutoAddColumn();
    return &m_handleStatements.back();
}

void AbstractHandle::returnStatement(HandleStatement *handleStatement)
{
    if (handleStatement != nullptr) {
        for (auto iter = m_handleStatements.begin(); iter != m_handleStatements.end(); ++iter) {
            if (&(*iter) == handleStatement) {
                m_handleStatements.erase(iter);
                return;
            }
        }
        WCTAssert(false);
    }
}

void AbstractHandle::resetAllStatements()
{
    for (auto &handleStatement : m_handleStatements) {
        if (!handleStatement.isPrepared()) continue;
        handleStatement.reset();
    }
}

void AbstractHandle::finalizeStatements()
{
    for (auto &handleStatement : m_handleStatements) {
        handleStatement.finalize();
    }
}

HandleStatement *AbstractHandle::getOrCreatePreparedStatement(const Statement &statement)
{
    const StringView &sql = statement.getDescription();
    if (sql.length() == 0) {
        m_error.setCode(Error::Code::Error, "invalid statement");
        m_error.level = Error::Level::Error;
        Notifier::shared().notify(m_error);
        return nullptr;
    }
    auto iter = m_preparedStatements.find(sql);
    HandleStatement *preparedStatement;
    if (iter == m_preparedStatements.end()) {
        preparedStatement = getStatement();
        m_preparedStatements[sql] = preparedStatement;
    } else {
        preparedStatement = iter->second;
    }
    WCTAssert(preparedStatement != nullptr);
    if (!preparedStatement->isPrepared() && !preparedStatement->prepare(statement)) {
        return nullptr;
    }
    return preparedStatement;
}

void AbstractHandle::returnAllPreparedStatement()
{
    for (const auto &iter : m_preparedStatements) {
        iter.second->finalize();
        returnStatement(iter.second);
    }
    m_preparedStatements.clear();
}

#pragma mark - Meta
Optional<bool> AbstractHandle::ft3TokenizerExists(const UnsafeStringView &tokenizer)
{
    markErrorAsIgnorable(Error::Code::Error);
    bool succeed = executeStatement(StatementSelect().select(
    Expression::function("fts3_tokenizer").invoke().arguments(tokenizer)));
    Optional<bool> exists;
    if (succeed) {
        exists = true;
    } else if (m_error.isIgnorable()) {
        exists = false;
    }
    markErrorAsUnignorable();
    return exists;
}

Optional<bool> AbstractHandle::tableExists(const UnsafeStringView &table)
{
    return tableExists(Schema(), table);
}

Optional<bool>
AbstractHandle::tableExists(const Schema &schema, const UnsafeStringView &table)
{
    StatementSelect statement
    = StatementSelect().select(1).from(TableOrSubquery(table).schema(schema)).limit(1);

    HandleStatement handleStatement(this);
    markErrorAsIgnorable(Error::Code::Error);
    bool succeed = handleStatement.prepare(statement);
    Optional<bool> exists;
    if (succeed) {
        handleStatement.finalize();
        exists = true;
    } else if (m_error.isIgnorable()) {
        exists = false;
    }
    markErrorAsUnignorable();
    return exists;
}

bool AbstractHandle::addColumn(const Schema &schema,
                               const UnsafeStringView &table,
                               const ColumnDef &column)
{
    StatementAlterTable statement
    = StatementAlterTable().alterTable(table).schema(schema).addColumn(column);
    HandleStatement handleStatement(this);
    bool hasCacheError = false;
    if (m_transactionError != TransactionError::Fatal) {
        hasCacheError = true;
        cacheCurrentTransactionError();
    }
    bool succeed = handleStatement.prepare(statement) && handleStatement.step();
    if (!succeed && getError().getMessage().hasPrefix("duplicate column name: ")) {
        succeed = true;
        if (hasCacheError) {
            resumeCacheTransactionError();
        }
    }
    handleStatement.finalize();
    return succeed;
}

Optional<std::set<StringView>> AbstractHandle::getColumns(const UnsafeStringView &table)
{
    return getColumns(Schema(), table);
}

Optional<std::set<StringView>>
AbstractHandle::getColumns(const Schema &schema, const UnsafeStringView &table)
{
    WCDB::StatementPragma statement
    = StatementPragma().pragma(Pragma::tableInfo()).schema(schema).with(table);
    return getValues(statement, 1);
}

Optional<std::vector<ColumnMeta>>
AbstractHandle::getTableMeta(const Schema &schema, const UnsafeStringView &table)
{
    Optional<std::vector<ColumnMeta>> metas;
    HandleStatement handleStatement(this);
    if (handleStatement.prepare(
        StatementPragma().pragma(Pragma::tableInfo()).schema(schema).with(table))) {
        bool succeed = false;
        std::vector<ColumnMeta> rows;
        while ((succeed = handleStatement.step()) && !handleStatement.done()) {
            rows.push_back(ColumnMeta(handleStatement.getInteger(0), // cid
                                      handleStatement.getText(1),    // name
                                      handleStatement.getText(2),    // type
                                      handleStatement.getInteger(3), // notnull
                                      handleStatement.getInteger(5)) // pk
            );
        }
        handleStatement.finalize();
        if (succeed) {
            metas = rows;
        }
    }
    return metas;
}

Optional<std::set<StringView>>
AbstractHandle::getValues(const Statement &statement, int index)
{
    Optional<std::set<StringView>> values;
    HandleStatement handleStatement(this);
    if (handleStatement.prepare(statement)) {
        std::set<StringView> rows;
        bool succeed = false;
        while ((succeed = handleStatement.step()) && !handleStatement.done()) {
            rows.emplace(handleStatement.getText(index));
        }
        handleStatement.finalize();
        if (succeed) {
            values = std::move(rows);
        }
    }
    return values;
}

#pragma mark - Transaction
void AbstractHandle::markErrorNotAllowedWithinTransaction()
{
    WCTRemedialAssert(m_transactionLevel == 0 || m_transactionLevel == 1,
                      "Transaction error state should be changed without nested.",
                      return;);
    if (m_transactionError == TransactionError::Allowed) {
        m_transactionError = TransactionError::NotAllowed;
    }
}

bool AbstractHandle::isErrorAllowedWithinTransaction() const
{
    return m_transactionError != TransactionError::NotAllowed;
}

void AbstractHandle::cacheCurrentTransactionError()
{
    m_cacheTransactionError = m_transactionError;
}

void AbstractHandle::resumeCacheTransactionError()
{
    m_transactionError = m_cacheTransactionError;
}

StringView AbstractHandle::getSavepointName(int transactionLevel)
{
    return StringView::formatted("wcdb_lv_%d", transactionLevel);
}

bool AbstractHandle::beginNestedTransaction()
{
    bool succeed = true;
    if (isInTransaction()) {
        if (m_transactionLevel == 0) {
            m_transactionLevel = 1;
        }

        if (m_transactionError == TransactionError::Allowed) {
            succeed = executeStatement(
            StatementSavepoint().savepoint(getSavepointName(m_transactionLevel + 1)));
        }
        if (succeed) {
            ++m_transactionLevel;
        }
    } else {
        succeed = beginTransaction();
    }
    return succeed;
}

bool AbstractHandle::commitOrRollbackNestedTransaction()
{
    bool succeed = true;
    if (isInTransaction()) {
        if (m_transactionLevel == 1) {
            succeed = commitOrRollbackTransaction();
        } else {
            if (m_transactionError == TransactionError::Allowed) {
                succeed = executeStatement(
                StatementRelease().release(getSavepointName(m_transactionLevel)));
            }
            if (succeed) {
                --m_transactionLevel;
            } else {
                rollbackNestedTransaction();
            }
        }
    }
    return succeed;
}

void AbstractHandle::rollbackNestedTransaction()
{
    if (isInTransaction()) {
        if (m_transactionLevel == 1) {
            rollbackTransaction();
        } else {
            bool succeed = true;
            if (m_transactionError == TransactionError::Allowed) {
                sqlite3_unimpeded(m_handle, true);
                succeed = executeStatement(StatementRollback().rollbackToSavepoint(
                getSavepointName(m_transactionLevel)));
                sqlite3_unimpeded(m_handle, false);
            }
            if (succeed) {
                --m_transactionLevel;
            }
        }
    }
}

bool AbstractHandle::beginTransaction()
{
    WCTRemedialAssert(!isInTransaction(),
                      "Last transaction is not committed or rollbacked.",
                      rollbackTransaction(););

    static const StatementBegin *s_beginImmediate
    = new StatementBegin(StatementBegin().beginImmediate());
    bool succeed = executeStatement(*s_beginImmediate);
    if (succeed) {
        m_transactionLevel = 1;
    }
    return succeed;
}

bool AbstractHandle::commitOrRollbackTransaction()
{
    bool succeed = true;
    if (m_transactionError != TransactionError::Fatal) {
        succeed = commitTransaction();
    } else {
        succeed = false;
    }
    if (!succeed) {
        // If certain kinds of errors occur within a transaction, the transaction may or may not be rolled back automatically: https://sqlite.org/lang_transaction.html
        rollbackTransaction();
        succeed = false;
    }
    return succeed;
}

bool AbstractHandle::commitTransaction()
{
    /*
    All statements must be reset before commit or rollback,
    because sqlite will downgrade handle to a read-only transaction state
    if some statements of this handle are still active.
    Please see the comment of btreeEndTransaction for more information.
    */
    resetAllStatements();
    bool succeed = true;
    // Transaction can be removed automatically in some case. e.g. interrupt step
    if (isInTransaction()) {
        static const StatementCommit *s_commit
        = new StatementCommit(StatementCommit().commit());

        succeed = executeStatement(*s_commit);
    }
    if (succeed) {
        m_transactionLevel = 0;
        m_transactionError = TransactionError::Allowed;
    }
    return succeed;
}

void AbstractHandle::rollbackTransaction()
{
    /*
    All statements must be reset before commit or rollback,
    because sqlite will downgrade handle to a read-only transaction state
    if some statements of this handle are still active.
    Please see the comment of btreeEndTransaction for more information.
    */
    resetAllStatements();
    bool succeed = true;
    // Transaction can be removed automatically in some case. e.g. interrupt step
    if (isInTransaction()) {
        static const StatementRollback *s_rollback
        = new StatementRollback(StatementRollback().rollback());
        sqlite3_unimpeded(m_handle, true);
        succeed = executeStatement(*s_rollback);
        sqlite3_unimpeded(m_handle, false);
    }
    if (succeed) {
        m_transactionLevel = 0;
        m_transactionError = TransactionError::Allowed;
    }
}

#pragma mark - Wal
void AbstractHandle::enableExtendedResultCodes(bool enable)
{
    WCTAssert(isOpened());
    APIExit(sqlite3_extended_result_codes(m_handle, (int) enable));
}

bool AbstractHandle::checkpoint(CheckpointMode mode)
{
    static_assert((int) CheckpointMode::Passive == SQLITE_CHECKPOINT_PASSIVE, "");
    static_assert((int) CheckpointMode::Full == SQLITE_CHECKPOINT_FULL, "");
    static_assert((int) CheckpointMode::Restart == SQLITE_CHECKPOINT_RESTART, "");
    static_assert((int) CheckpointMode::Truncate == SQLITE_CHECKPOINT_TRUNCATE, "");
    WCTAssert(isOpened());

    return APIExit(sqlite3_wal_checkpoint_v2(
    m_handle, Syntax::mainSchema.data(), (int) mode, nullptr, nullptr));
}

void AbstractHandle::disableCheckpointWhenClosing(bool disable)
{
    WCTAssert(isOpened());
    APIExit(sqlite3_db_config(
    m_handle, SQLITE_DBCONFIG_NO_CKPT_ON_CLOSE, (int) disable, nullptr));
}

void AbstractHandle::setWALFilePersist(int persist)
{
    if (m_path.compare(":memory:") == 0) {
        return;
    }
    WCTAssert(isOpened());
    APIExit(sqlite3_file_control(
    m_handle, Syntax::mainSchema.data(), SQLITE_FCNTL_PERSIST_WAL, &persist));
}

#pragma mark - Notification
void AbstractHandle::setNotificationWhenSQLTraced(const UnsafeStringView &name,
                                                  const SQLNotification &onTraced)
{
    WCTAssert(isOpened());
    m_notification.setNotificationWhenSQLTraced(name, onTraced);
}

void AbstractHandle::setNotificationWhenPerformanceTraced(const UnsafeStringView &name,
                                                          const PerformanceNotification &onTraced)
{
    WCTAssert(isOpened());
    m_notification.setNotificationWhenPerformanceTraced(name, onTraced);
}

void AbstractHandle::setNotificationWhenCommitted(int order,
                                                  const UnsafeStringView &name,
                                                  const CommittedNotification &onCommitted)
{
    WCTAssert(isOpened());
    m_notification.setNotificationWhenCommitted(order, name, onCommitted);
}

void AbstractHandle::unsetNotificationWhenCommitted(const UnsafeStringView &name)
{
    WCTAssert(isOpened());
    m_notification.unsetNotificationWhenCommitted(name);
}

void AbstractHandle::setNotificationWhenCheckpointed(const UnsafeStringView &name,
                                                     const CheckpointedNotification &checkpointed)
{
    WCTAssert(isOpened());
    m_notification.setNotificationWhenCheckpointed(name, checkpointed);
}

void AbstractHandle::setNotificationWhenBusy(const BusyNotification &busyNotification)
{
    WCTAssert(isOpened());
    m_notification.setNotificationWhenBusy(busyNotification);
}

void AbstractHandle::setNotificationWhenTableModified(const UnsafeStringView &name,
                                                      const TableModifiedNotification &tableModifiedNotification)
{
    m_notification.setNotificationWhenTableModified(name, tableModifiedNotification);
}

void AbstractHandle::postTableNotification(const UnsafeStringView &newTable,
                                           const UnsafeStringView &modifiedTable)
{
    m_notification.postTableModifiedNotification(newTable, modifiedTable);
}

bool AbstractHandle::needMonitorTable()
{
    return !m_tableMonitorForbidden && m_notification.needMonitorTable();
}

void AbstractHandle::setTableMonitorEnable(bool enable)
{
    m_tableMonitorForbidden = !enable;
}

#pragma mark - Error
bool AbstractHandle::APIExit(int rc)
{
    return APIExit(rc, nullptr);
}

bool AbstractHandle::APIExit(int rc, const UnsafeStringView &sql)
{
    return APIExit(rc, sql.data());
}

bool AbstractHandle::APIExit(int rc, const char *sql)
{
    bool result = true;
    if (Error::isError(rc)) {
        notifyError(rc, sql);
        result = false;
    }
    return result;
}

void AbstractHandle::notifyError(int rc, const char *sql)
{
    WCTAssert(Error::isError(rc));
    if (Error::rc2c(rc) != Error::Code::Misuse && sqlite3_errcode(m_handle) == rc) {
        m_error.setSQLiteCode(rc, sqlite3_errmsg(m_handle));
    } else {
        // extended error code/message will not be set in some case for misuse error
        m_error.setSQLiteCode(rc);
    }
    if (std::find(m_ignorableCodes.begin(), m_ignorableCodes.end(), rc)
        == m_ignorableCodes.end()) {
        m_error.level = Error::Level::Error;
        if (m_transactionError != TransactionError::Allowed) {
            m_transactionError = TransactionError::Fatal;
        }
    } else {
        m_error.level = Error::Level::Ignore;
    }
    m_error.infos.insert_or_assign(ErrorStringKeySQL, sql);
    Notifier::shared().notify(m_error);
}

void AbstractHandle::markErrorAsIgnorable(Error::Code ignorableCode)
{
    m_ignorableCodes.emplace_back((int) ignorableCode);
}

void AbstractHandle::markErrorAsUnignorable(int count)
{
    while (!m_ignorableCodes.empty() && count > 0) {
        m_ignorableCodes.pop_back();
        --count;
    }
}

#pragma mark - Suspend
void AbstractHandle::suspend(bool suspend)
{
    doSuspend(suspend && m_canBeSuspended);
}

void AbstractHandle::markAsCanBeSuspended(bool canBeSuspended)
{
    if (!(m_canBeSuspended = canBeSuspended)) {
        doSuspend(false);
    }
}

void AbstractHandle::doSuspend(bool suspend)
{
    if (isOpened()) {
        sqlite3_suspend(m_handle, suspend);
    }
}

#pragma mark - Cipher

void *AbstractHandle::getCipherContext()
{
    WCTAssert(isOpened());
    return sqlite3_getCipherContext(m_handle, Schema::main().getDescription().data());
}

size_t AbstractHandle::getCipherPageSize()
{
    WCTAssert(isOpened());
    HandleStatement handleStatement(this);
    Statement cipherPageSize = StatementPragma().pragma(Pragma::cipherPageSize());
    size_t size = 0;
    if (handleStatement.prepare(cipherPageSize) && handleStatement.step()) {
        size = handleStatement.getInteger();
    }
    handleStatement.finalize();
    return size;
}

bool AbstractHandle::setCipherKey(const UnsafeData &data)
{
    WCTAssert(isOpened());
    return APIExit(sqlite3_key(m_handle, data.buffer(), (int) data.size()));
}

bool AbstractHandle::setCipherPageSize(int pageSize)
{
    WCTAssert(isOpened());
    HandleStatement handleStatement(this);
    Statement cipherPageSize
    = StatementPragma().pragma(Pragma::cipherPageSize()).to(pageSize);
    bool succeed = handleStatement.prepare(cipherPageSize) && handleStatement.step();
    handleStatement.finalize();
    return succeed;
}

StringView AbstractHandle::getCipherSalt()
{
    WCTAssert(isOpened());
    HandleStatement handleStatement(this);
    Statement cipherSalt = StatementPragma().pragma(Pragma::cipherSalt());
    StringView salt;
    if (handleStatement.prepare(cipherSalt) && handleStatement.step()
        && !handleStatement.done()) {
        salt = handleStatement.getText();
    }
    handleStatement.finalize();
    return salt;
}
bool AbstractHandle::setCipherSalt(const UnsafeStringView &salt)
{
    WCTAssert(isOpened());
    HandleStatement handleStatement(this);
    StatementPragma cipherSalt = StatementPragma().pragma(Pragma::cipherSalt());
    if (salt.length() == 32 && !salt.hasPrefix("x'")) {
        cipherSalt.to(StringView::formatted("x'%s'", salt.data()));
    } else {
        cipherSalt.to(salt);
    }
    bool succeed = handleStatement.prepare(cipherSalt) && handleStatement.step();
    handleStatement.finalize();
    return succeed;
}

} //namespace WCDB
