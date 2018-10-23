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

#include <WCDB/Assertion.hpp>
#include <WCDB/Handle.hpp>
#include <WCDB/Macro.hpp>
#include <WCDB/Notifier.hpp>
#include <WCDB/Path.hpp>
#include <WCDB/RepairKit.h>
#include <WCDB/SQLite.h>
#include <WCDB/String.hpp>

namespace WCDB {

#pragma mark - Initialize
Handle::Handle(const std::string &path_)
: path(path_)
, m_handle(nullptr)
, m_handleStatement(this)
, m_notification(this)
, m_nestedLevel(0)
, m_codeToBeIgnored(SQLITE_OK)
{
    m_error.infos.set("Path", path);
}

Handle::~Handle()
{
    close();
}

#pragma mark - Global
void Handle::enableMultithread()
{
    sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
}

void Handle::setMemoryMapSize(int64_t defaultSizeLimit, int64_t maximumAllowedSizeLimit)
{
    sqlite3_config(SQLITE_CONFIG_MMAP_SIZE, defaultSizeLimit, maximumAllowedSizeLimit);
}

void Handle::enableMemoryStatus(bool enable)
{
    sqlite3_config(SQLITE_CONFIG_MEMSTATUS, enable);
}

void Handle::setNotificationForLog(const Log &log)
{
    sqlite3_config(SQLITE_CONFIG_LOG, log, nullptr);
}

void Handle::setNotificationWhenVFSOpened(const VFSOpen &vfsOpen)
{
    sqlite3_vfs *vfs = sqlite3_vfs_find(nullptr);
    vfs->xSetSystemCall(vfs, "open", (void (*)(void)) vfsOpen);
}

#pragma mark - Path
std::string Handle::getSHMSubfix()
{
    return "-shm";
}

std::string Handle::getWALSubfix()
{
    return "-wal";
}

std::string Handle::getJournalSubfix()
{
    return "-journal";
}

#pragma mark - Basic
bool Handle::open()
{
    if (isOpened()) {
        return true;
    }
    int rc = sqlite3_open(path.c_str(), (sqlite3 **) &m_handle);
    if (rc != SQLITE_OK) {
        setError(rc);
        return false;
    }
    return true;
}

bool Handle::isOpened() const
{
    return m_handle != nullptr;
}

void Handle::close()
{
    finalize();
    if (m_handle) {
        WCTRemedialAssert(m_nestedLevel == 0 && !isInTransaction(),
                          "Unpaired transaction.",
                          m_nestedLevel = 0;
                          rollbackTransaction(););
        m_notification.purge();
        // disable checkpoint when closing. If ones need a checkpoint, they should do it manually.
        constexpr const char *name = "close";
        m_notification.setNotificationWhenWillCheckpoint(
        std::numeric_limits<int>::min(),
        name,
        [](Handle *) -> bool { return false; },
        true);
        sqlite3_close_v2((sqlite3 *) m_handle);
        m_handle = nullptr;
    }
}

bool Handle::execute(const Statement &statement)
{
    WCTInnerAssert(isOpened());
    int rc = sqlite3_exec(
    (sqlite3 *) m_handle, statement.getDescription().c_str(), nullptr, nullptr, nullptr);
    if (rc == SQLITE_OK) {
        return true;
    }
    setError(rc, statement.getDescription());
    return false;
}

int Handle::getExtendedErrorCode()
{
    WCTInnerAssert(isOpened());
    return sqlite3_extended_errcode((sqlite3 *) m_handle);
}

long long Handle::getLastInsertedRowID()
{
    WCTInnerAssert(isOpened());
    return sqlite3_last_insert_rowid((sqlite3 *) m_handle);
}

int Handle::getResultCode()
{
    WCTInnerAssert(isOpened());
    return sqlite3_errcode((sqlite3 *) m_handle);
}

const char *Handle::getErrorMessage()
{
    WCTInnerAssert(isOpened());
    return sqlite3_errmsg((sqlite3 *) m_handle);
}

int Handle::getChanges()
{
    WCTInnerAssert(isOpened());
    return sqlite3_changes((sqlite3 *) m_handle);
}

bool Handle::isReadonly()
{
    WCTInnerAssert(isOpened());
    return sqlite3_db_readonly((sqlite3 *) m_handle, NULL) == 1;
}

bool Handle::isInTransaction()
{
    WCTInnerAssert(isOpened());
    return sqlite3_get_autocommit((sqlite3 *) m_handle) == 0;
}

#pragma mark - Statement
bool Handle::prepare(const Statement &statement)
{
    WCTInnerAssert(isOpened());
    WCTInnerAssert(!isPrepared());
    return m_handleStatement.prepare(statement);
}

bool Handle::isPrepared() const
{
    return m_handleStatement.isPrepared();
}

void Handle::reset()
{
    WCTInnerAssert(isPrepared());
    m_handleStatement.reset();
}

bool Handle::step(bool &done)
{
    WCTInnerAssert(isPrepared());
    return m_handleStatement.step(done);
}

bool Handle::step()
{
    WCTInnerAssert(isPrepared());
    bool unused;
    return step(unused);
}

int Handle::getColumnCount()
{
    WCTInnerAssert(isPrepared());
    return m_handleStatement.getColumnCount();
}

const char *Handle::getColumnName(int index)
{
    WCTInnerAssert(isPrepared());
    return m_handleStatement.getColumnName(index);
}

const char *Handle::getColumnTableName(int index)
{
    WCTInnerAssert(isPrepared());
    return m_handleStatement.getColumnTableName(index);
}

ColumnType Handle::getType(int index)
{
    WCTInnerAssert(isPrepared());
    return m_handleStatement.getType(index);
}

void Handle::bindInteger32(const Integer32 &value, int index)
{
    WCTInnerAssert(isPrepared());
    m_handleStatement.bindInteger32(value, index);
}

void Handle::bindInteger64(const Integer64 &value, int index)
{
    WCTInnerAssert(isPrepared());
    m_handleStatement.bindInteger64(value, index);
}

void Handle::bindDouble(const Float &value, int index)
{
    WCTInnerAssert(isPrepared());
    m_handleStatement.bindDouble(value, index);
}

void Handle::bindText(const Text &value, int index)
{
    WCTInnerAssert(isPrepared());
    bindText(value, -1, index);
}

void Handle::bindText(const Text &value, int length, int index)
{
    WCTInnerAssert(isPrepared());
    m_handleStatement.bindText(value, length, index);
}

void Handle::bindBLOB(const BLOB &value, int index)
{
    WCTInnerAssert(isPrepared());
    m_handleStatement.bindBLOB(value, index);
}

void Handle::bindNull(int index)
{
    WCTInnerAssert(isPrepared());
    m_handleStatement.bindNull(index);
}

Handle::Integer32 Handle::getInteger32(int index)
{
    WCTInnerAssert(isPrepared());
    return m_handleStatement.getInteger32(index);
}

Handle::Integer64 Handle::getInteger64(int index)
{
    WCTInnerAssert(isPrepared());
    return m_handleStatement.getInteger64(index);
}

Handle::Float Handle::getDouble(int index)
{
    WCTInnerAssert(isPrepared());
    return m_handleStatement.getDouble(index);
}

Handle::Text Handle::getText(int index)
{
    WCTInnerAssert(isPrepared());
    return m_handleStatement.getText(index);
}

Handle::BLOB Handle::getBLOB(int index)
{
    WCTInnerAssert(isPrepared());
    return m_handleStatement.getBLOB(index);
}

void Handle::finalize()
{
    WCTInnerAssert(isPrepared());
    m_handleStatement.finalize();
}

bool Handle::isStatementReadonly()
{
    WCTInnerAssert(isPrepared());
    return m_handleStatement.isReadonly();
}

bool Handle::isPrepared()
{
    return m_handleStatement.isPrepared();
}

#pragma mark - Convenient
std::pair<bool, bool> Handle::tableExists(const TableOrSubquery &table)
{
    StatementSelect statementSelect = StatementSelect().select(1).from(table).limit(0);
    markErrorAsIgnorable(SQLITE_ERROR);
    bool unused;
    bool result = prepare(statementSelect) && step(unused);
    markErrorAsUnignorable();
    finalize();
    return { result || getResultCode() == SQLITE_ERROR, result };
}

std::pair<bool, std::set<std::string>>
Handle::getUnorderedColumnsWithTable(const Schema &schema, const std::string &table)
{
    WCDB::StatementPragma statement
    = StatementPragma().pragma(schema, Pragma::tableInfo()).with(table);
    return getUnorderedValues(statement, 1);
}

std::pair<bool, std::set<std::string>>
Handle::getUnorderedColumnsWithTable(const std::string &table)
{
    return getUnorderedColumnsWithTable(Schema::main(), table);
}

std::pair<bool, std::set<std::string>>
Handle::getUnorderedValues(const Statement &statement, int index)
{
#warning TODO - Prepare/Step/Execute/... within Handle.cpp should use their own implementation or use the overrided one?
    if (prepare(statement)) {
        bool done = false;
        std::set<std::string> values;
        while (step(done) && !done) {
            values.emplace(getText(index));
        }
        finalize();
        if (done) {
            return { true, std::move(values) };
        }
    }
    return { false, {} };
}

const std::string &Handle::savepointPrefix()
{
    static const std::string s_savepointPrefix("WCDBSavepoint_");
    return s_savepointPrefix;
}

bool Handle::beginNestedTransaction()
{
    if (!isInTransaction()) {
        return beginTransaction();
    }
    std::string savepointName = savepointPrefix() + std::to_string(++m_nestedLevel);
    return execute(StatementSavepoint().savepoint(savepointName));
}

bool Handle::commitOrRollbackNestedTransaction()
{
    if (m_nestedLevel == 0) {
        return commitOrRollbackTransaction();
    }
    std::string savepointName = savepointPrefix() + std::to_string(m_nestedLevel--);
    if (!execute(StatementRelease().release(savepointName))) {
        markErrorAsIgnorable(-1);
        execute(StatementRollback().rollbackToSavepoint(savepointName));
        markErrorAsUnignorable();
        return false;
    }
    return true;
}

void Handle::rollbackNestedTransaction()
{
    if (m_nestedLevel == 0) {
        return rollbackTransaction();
    }
    std::string savepointName = savepointPrefix() + std::to_string(m_nestedLevel--);
    markErrorAsIgnorable(-1);
    execute(StatementRollback().rollbackToSavepoint(savepointName));
    markErrorAsUnignorable();
}

bool Handle::runNestedTransaction(const TransactionCallback &transaction)
{
    if (!beginNestedTransaction()) {
        return false;
    }
    if (transaction(this)) {
        return commitOrRollbackNestedTransaction();
    }
    rollbackNestedTransaction();
    return false;
}

bool Handle::beginTransaction()
{
    return execute(StatementBegin().beginImmediate());
}

bool Handle::commitOrRollbackTransaction()
{
    m_nestedLevel = 0;
    if (!execute(StatementCommit().commit())) {
        markErrorAsIgnorable(-1);
        execute(StatementRollback().rollback());
        markErrorAsUnignorable();
        return false;
    }
    return true;
}

void Handle::rollbackTransaction()
{
    m_nestedLevel = 0;
    markErrorAsIgnorable(-1);
    execute(StatementRollback().rollback());
    markErrorAsUnignorable();
}

bool Handle::runTransaction(const TransactionCallback &transaction)
{
    if (!beginTransaction()) {
        return false;
    }
    if (transaction(this)) {
        return commitOrRollbackTransaction();
    }
    rollbackTransaction();
    return false;
}

#pragma mark - Cipher
bool Handle::setCipherKey(const UnsafeData &data)
{
    WCTInnerAssert(isOpened());
    int rc = sqlite3_key((sqlite3 *) m_handle, data.buffer(), (int) data.size());
    if (rc == SQLITE_OK) {
        return true;
    }
    setError(rc);
    return false;
}

#pragma mark - Notification
void Handle::setNotificationWhenSQLTraced(const std::string &name, const SQLNotification &onTraced)
{
    WCTInnerAssert(isOpened());
    m_notification.setNotificationWhenSQLTraced(name, onTraced);
}

void Handle::setNotificationWhenPerformanceTraced(const std::string &name,
                                                  const PerformanceNotification &onTraced)
{
    WCTInnerAssert(isOpened());
    m_notification.setNotificationWhenPerformanceTraced(name, onTraced);
}

void Handle::setNotificationWhenCommitted(int order,
                                          const std::string &name,
                                          const CommittedNotification &onCommitted)
{
    WCTInnerAssert(isOpened());
    m_notification.setNotificationWhenCommitted(order, name, onCommitted);
}

void Handle::unsetNotificationWhenCommitted(const std::string &name)
{
    WCTInnerAssert(isOpened());
    m_notification.unsetNotificationWhenCommitted(name);
}

bool Handle::setNotificationWhenWillCheckpoint(int order,
                                               const std::string &name,
                                               const WCDB::Handle::WillCheckpointNotification &willCheckpoint)
{
    WCTInnerAssert(isOpened());
    return m_notification.setNotificationWhenWillCheckpoint(order, name, willCheckpoint);
}

bool Handle::unsetNotificationWhenWillCheckpoint(const std::string &name)
{
    WCTInnerAssert(isOpened());
    return m_notification.unsetNotificationWhenWillCheckpoint(name);
}

bool Handle::setNotificationWhenCheckpointed(const std::string &name,
                                             const CheckpointedNotification &checkpointed)
{
    WCTInnerAssert(isOpened());
    return m_notification.setNotificationWhenCheckpointed(name, checkpointed);
}

#pragma mark - Error
void Handle::setError(int rc, const std::string &sql)
{
    WCTInnerAssert(rc != SQLITE_OK);
    if (m_codeToBeIgnored >= 0 && rc != m_codeToBeIgnored) {
        doSetError(m_error, rc, sql);
    } else {
        Error error = m_error;
        doSetError(error, rc, sql);
    }
}

void Handle::doSetError(Error &error, int rc, const std::string &sql)
{
    if (rc != SQLITE_MISUSE) {
        // extended error code will not be set in some case for misuse error
        error.setSQLiteCode(rc, getExtendedErrorCode());
    } else {
        error.setSQLiteCode(rc);
    }
    if (m_codeToBeIgnored >= 0 && rc != m_codeToBeIgnored) {
        error.level = Error::Level::Error;
    } else {
        error.level = Error::Level::Ignore;
    }
    const char *message = getErrorMessage();
    if (message) {
        error.message = message;
    } else {
        error.message = String::empty();
    }
    error.infos.set("SQL", sql);
    Notifier::shared()->notify(error);
}

void Handle::markErrorAsIgnorable(int codeToBeIgnored)
{
    m_codeToBeIgnored = codeToBeIgnored;
}

void Handle::markErrorAsUnignorable()
{
    m_codeToBeIgnored = SQLITE_OK;
}

} //namespace WCDB
