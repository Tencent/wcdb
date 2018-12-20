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
Handle::Handle()
: m_handle(nullptr), m_notification(this), m_nestedLevel(0), m_codeToBeIgnored(SQLITE_OK)
{
    m_mainStatement = getStatement();
}

Handle::~Handle()
{
    close();
}

void **Handle::getRawHandle()
{
    return &m_handle;
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
void Handle::setPath(const String &path)
{
    WCTRemedialAssert(!isOpened(), "Path can't be changed after opened.", return;);
    m_path = path;
    m_error.infos.set("Path", path);
}

const String &Handle::getPath() const
{
    return m_path;
}

String Handle::getSHMSubfix()
{
    return "-shm";
}

String Handle::getWALSubfix()
{
    return "-wal";
}

String Handle::getJournalSubfix()
{
    return "-journal";
}

#pragma mark - Basic
bool Handle::open()
{
    if (isOpened()) {
        return true;
    }
    int rc = sqlite3_open(m_path.c_str(), (sqlite3 **) &m_handle);
    if (rc != SQLITE_OK) {
        return error(rc);
    }
    return true;
}

bool Handle::isOpened() const
{
    return m_handle != nullptr;
}

void Handle::close()
{
    WCTRemedialAssert(!isPrepared(), "Statement is not finalized.", finalize(););
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
        m_notification.purge();
    }
}

bool Handle::execute(const String &sql)
{
    WCTInnerAssert(isOpened());
    int rc = sqlite3_exec((sqlite3 *) m_handle, sql.c_str(), nullptr, nullptr, nullptr);
    if (rc == SQLITE_OK) {
        return true;
    }
    return error(rc, sql);
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

void Handle::interrupt()
{
    WCTInnerAssert(isOpened());
    sqlite3_interrupt((sqlite3 *) m_handle);
}

#pragma mark - Statement
bool Handle::execute(const Statement &statement)
{
    return execute(statement.getDescription());
}

bool Handle::prepare(const Statement &statement)
{
    return m_mainStatement->prepare(statement);
}

bool Handle::isPrepared()
{
    return m_mainStatement->isPrepared();
}

void Handle::reset()
{
    WCTInnerAssert(isPrepared());
    m_mainStatement->reset();
}

bool Handle::step(bool &done)
{
    WCTInnerAssert(isPrepared());
    return m_mainStatement->step(done);
}

bool Handle::step()
{
    bool unused;
    return step(unused);
}

int Handle::getColumnCount()
{
    WCTInnerAssert(isPrepared());
    return m_mainStatement->getColumnCount();
}

const UnsafeString Handle::getOriginColumnName(int index)
{
    WCTInnerAssert(isPrepared());
    return m_mainStatement->getOriginColumnName(index);
}

const UnsafeString Handle::getColumnName(int index)
{
    WCTInnerAssert(isPrepared());
    return m_mainStatement->getColumnName(index);
}

const UnsafeString Handle::getColumnTableName(int index)
{
    WCTInnerAssert(isPrepared());
    return m_mainStatement->getColumnTableName(index);
}

ColumnType Handle::getType(int index)
{
    WCTInnerAssert(isPrepared());
    return m_mainStatement->getType(index);
}

void Handle::bindInteger32(const Integer32 &value, int index)
{
    WCTInnerAssert(isPrepared());
    m_mainStatement->bindInteger32(value, index);
}

void Handle::bindInteger64(const Integer64 &value, int index)
{
    WCTInnerAssert(isPrepared());
    m_mainStatement->bindInteger64(value, index);
}

void Handle::bindDouble(const Float &value, int index)
{
    WCTInnerAssert(isPrepared());
    m_mainStatement->bindDouble(value, index);
}

void Handle::bindText(const Text &value, int index)
{
    WCTInnerAssert(isPrepared());
    m_mainStatement->bindText(value, index);
}

void Handle::bindBLOB(const BLOB &value, int index)
{
    WCTInnerAssert(isPrepared());
    m_mainStatement->bindBLOB(value, index);
}

void Handle::bindNull(int index)
{
    WCTInnerAssert(isPrepared());
    m_mainStatement->bindNull(index);
}

Handle::Integer32 Handle::getInteger32(int index)
{
    WCTInnerAssert(isPrepared());
    return m_mainStatement->getInteger32(index);
}

Handle::Integer64 Handle::getInteger64(int index)
{
    WCTInnerAssert(isPrepared());
    return m_mainStatement->getInteger64(index);
}

Handle::Float Handle::getDouble(int index)
{
    WCTInnerAssert(isPrepared());
    return m_mainStatement->getDouble(index);
}

Handle::Text Handle::getText(int index)
{
    WCTInnerAssert(isPrepared());
    return m_mainStatement->getText(index);
}

Handle::BLOB Handle::getBLOB(int index)
{
    WCTInnerAssert(isPrepared());
    return m_mainStatement->getBLOB(index);
}

void Handle::finalize()
{
    m_mainStatement->finalize();
}

bool Handle::isStatementReadonly()
{
    WCTInnerAssert(isPrepared());
    return m_mainStatement->isReadonly();
}

HandleStatement *Handle::getStatement()
{
    m_handleStatements.push_back(HandleStatement(this));
    return &m_handleStatements.back();
}

void Handle::returnStatement(HandleStatement *handleStatement)
{
    if (handleStatement) {
        for (auto iter = m_handleStatements.begin(); iter != m_handleStatements.end(); ++iter) {
            if (&(*iter) == handleStatement) {
                m_handleStatements.erase(iter);
                return;
            }
        }
    }
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

std::pair<bool, std::set<String>>
Handle::getUnorderedColumns(const Schema &schema, const String &table)
{
    WCDB::StatementPragma statement
    = StatementPragma().pragma(Pragma::tableInfo()).schema(schema).with(table);
    return getUnorderedValues(statement, 1);
}

std::pair<bool, std::set<String>>
Handle::getUnorderedValues(const Statement &statement, int index)
{
    if (prepare(statement)) {
        bool done = false;
        std::set<String> values;
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

const String &Handle::savepointPrefix()
{
    static const String s_savepointPrefix("WCDBSavepoint_");
    return s_savepointPrefix;
}

bool Handle::beginNestedTransaction()
{
    if (!isInTransaction()) {
        return beginTransaction();
    }
    String savepointName = savepointPrefix() + std::to_string(++m_nestedLevel);
    return execute(StatementSavepoint().savepoint(savepointName));
}

bool Handle::commitOrRollbackNestedTransaction()
{
    if (m_nestedLevel == 0) {
        return commitOrRollbackTransaction();
    }
    String savepointName = savepointPrefix() + std::to_string(m_nestedLevel--);
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
    String savepointName = savepointPrefix() + std::to_string(m_nestedLevel--);
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
    return error(rc);
}

#pragma mark - Notification
void Handle::setNotificationWhenSQLTraced(const String &name, const SQLNotification &onTraced)
{
    WCTInnerAssert(isOpened());
    m_notification.setNotificationWhenSQLTraced(name, onTraced);
}

void Handle::setNotificationWhenPerformanceTraced(const String &name,
                                                  const PerformanceNotification &onTraced)
{
    WCTInnerAssert(isOpened());
    m_notification.setNotificationWhenPerformanceTraced(name, onTraced);
}

void Handle::setNotificationWhenCommitted(int order,
                                          const String &name,
                                          const CommittedNotification &onCommitted)
{
    WCTInnerAssert(isOpened());
    m_notification.setNotificationWhenCommitted(order, name, onCommitted);
}

void Handle::unsetNotificationWhenCommitted(const String &name)
{
    WCTInnerAssert(isOpened());
    m_notification.unsetNotificationWhenCommitted(name);
}

bool Handle::setNotificationWhenWillCheckpoint(
int order, const String &name, const WCDB::Handle::WillCheckpointNotification &willCheckpoint)
{
    WCTInnerAssert(isOpened());
    return m_notification.setNotificationWhenWillCheckpoint(order, name, willCheckpoint);
}

bool Handle::unsetNotificationWhenWillCheckpoint(const String &name)
{
    WCTInnerAssert(isOpened());
    return m_notification.unsetNotificationWhenWillCheckpoint(name);
}

bool Handle::setNotificationWhenCheckpointed(const String &name,
                                             const CheckpointedNotification &checkpointed)
{
    WCTInnerAssert(isOpened());
    return m_notification.setNotificationWhenCheckpointed(name, checkpointed);
}

#pragma mark - Error
bool Handle::error(int rc, const String &sql)
{
    WCTInnerAssert(rc != SQLITE_OK);
    if (m_codeToBeIgnored >= 0 && rc != m_codeToBeIgnored) {
        // non-ignorable
        setupAndNotifyError(m_error, rc, sql);
        return false;
    } else {
        // ignorable
        Error error = m_error;
        setupAndNotifyError(error, rc, sql);
        return true;
    }
}

void Handle::setupAndNotifyError(Error &error, int rc, const String &sql)
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
        error.message = String::null();
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
