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

#include <WCDB/Abstract.h>
#include <WCDB/Assertion.hpp>
#include <WCDB/Macro.hpp>
#include <WCDB/Notifier.hpp>
#include <WCDB/Path.hpp>
#include <WCDB/RepairKit.h>
#include <WCDB/String.hpp>
#include <sqlcipher/sqlite3.h>

namespace WCDB {

#pragma mark - Initialize
Handle::Handle(const std::string &path_)
: m_handle(nullptr)
, m_handleStatement(this)
, m_notification(this)
, path(path_)
, m_nestedLevel(0)
, m_ignorableCode(SQLITE_OK)
{
    m_error.infos.set("Path", path);
}

Handle::~Handle()
{
    close();
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
    if (!m_handle) {
        int rc = sqlite3_open(path.c_str(), (sqlite3 **) &m_handle);
        if (rc != SQLITE_OK) {
            setError(rc);
            return false;
        }
    }
    return true;
}

void Handle::close()
{
    finalize();
    if (m_handle) {
        m_notification.purge();
        //disable checkpoint when closing. If ones need a checkpoint, they should do it manually.
        constexpr const char *name = "close";
        m_notification.setNotificationWhenWillCheckpoint(
        std::numeric_limits<int>::min(),
        name,
        [](Handle *) -> bool { return false; },
        true);
        sqlite3_close_v2((sqlite3 *) m_handle);
        m_notification.unsetNotificationWhenWillCheckpoint(name, true);
        m_handle = nullptr;
    }
}

bool Handle::execute(const Statement &statement)
{
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
    return sqlite3_extended_errcode((sqlite3 *) m_handle);
}

long long Handle::getLastInsertedRowID()
{
    return sqlite3_last_insert_rowid((sqlite3 *) m_handle);
}

int Handle::getResultCode() const
{
    return sqlite3_errcode((sqlite3 *) m_handle);
}

const char *Handle::getErrorMessage()
{
    return sqlite3_errmsg((sqlite3 *) m_handle);
}

int Handle::getChanges()
{
    return sqlite3_changes((sqlite3 *) m_handle);
}

bool Handle::isReadonly()
{
    return sqlite3_db_readonly((sqlite3 *) m_handle, NULL) == 1;
}

bool Handle::isInTransaction()
{
    return sqlite3_get_autocommit((sqlite3 *) m_handle) == 0;
}

#pragma mark - Statement
bool Handle::prepare(const Statement &statement)
{
    return m_handleStatement.prepare(statement);
}

void Handle::reset()
{
    m_handleStatement.reset();
}

bool Handle::step(bool &done)
{
    return m_handleStatement.step(done);
}

bool Handle::step()
{
    bool unused;
    return step(unused);
}

int Handle::getColumnCount()
{
    return m_handleStatement.getColumnCount();
}

const char *Handle::getColumnName(int index)
{
    return m_handleStatement.getColumnName(index);
}

const char *Handle::getColumnTableName(int index)
{
    return m_handleStatement.getColumnTableName(index);
}

ColumnType Handle::getType(int index)
{
    return m_handleStatement.getType(index);
}

void Handle::bindInteger32(const Integer32 &value, int index)
{
    m_handleStatement.bindInteger32(value, index);
}

void Handle::bindInteger64(const Integer64 &value, int index)
{
    m_handleStatement.bindInteger64(value, index);
}

void Handle::bindDouble(const Float &value, int index)
{
    m_handleStatement.bindDouble(value, index);
}

void Handle::bindText(const Text &value, int index)
{
    bindText(value, -1, index);
}

void Handle::bindText(const Text &value, int length, int index)
{
    m_handleStatement.bindText(value, length, index);
}

void Handle::bindBLOB(const BLOB &value, int index)
{
    m_handleStatement.bindBLOB(value, index);
}

void Handle::bindNull(int index)
{
    m_handleStatement.bindNull(index);
}

Handle::Integer32 Handle::getInteger32(int index)
{
    return m_handleStatement.getInteger32(index);
}

Handle::Integer64 Handle::getInteger64(int index)
{
    return m_handleStatement.getInteger64(index);
}

Handle::Float Handle::getDouble(int index)
{
    return m_handleStatement.getDouble(index);
}

Handle::Text Handle::getText(int index)
{
    return m_handleStatement.getText(index);
}

Handle::BLOB Handle::getBLOB(int index)
{
    return m_handleStatement.getBLOB(index);
}

void Handle::finalize()
{
    m_handleStatement.finalize();
}

bool Handle::isStatementReadonly()
{
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
    markAsIgnorable(SQLITE_ERROR);
    bool unused;
    bool result = Handle::prepare(statementSelect) && Handle::step(unused);
    markAsUnignorable();
    finalize();
    return { result || getResultCode() == SQLITE_ERROR, result };
}

std::pair<bool, std::set<std::string>>
Handle::getUnorderedColumnsWithTable(const std::string &tableName, const std::string &schemaName)
{
    WCDB::StatementPragma statement
    = StatementPragma().withSchema(schemaName).pragma(Pragma::tableInfo()).with(tableName);
    return getUnorderedValues(statement, 1);
}

std::pair<bool, std::set<std::string>> Handle::getUnorderedAttachedSchemas()
{
    return getUnorderedValues(StatementPragma().pragma(Pragma::databaseList()), 1);
}

std::pair<bool, std::set<std::string>>
Handle::getUnorderedValues(const Statement &statement, int index)
{
    if (Handle::prepare(statement)) {
        bool done = false;
        std::set<std::string> values;
        while (Handle::step(done) && !done) {
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
    std::string savepointName = Handle::savepointPrefix() + std::to_string(++m_nestedLevel);
    return Handle::execute(StatementSavepoint().savepoint(savepointName));
}

bool Handle::commitOrRollbackNestedTransaction()
{
    if (m_nestedLevel == 0) {
        return commitOrRollbackTransaction();
    }
    std::string savepointName
    = Handle::savepointPrefix() + std::to_string(m_nestedLevel--);
    if (!Handle::execute(StatementRelease().release(savepointName))) {
        discardableExecute(StatementRollback().rollbackTo(savepointName));
        return false;
    }
    return true;
}

void Handle::rollbackNestedTransaction()
{
    if (m_nestedLevel == 0) {
        return rollbackTransaction();
    }
    std::string savepointName
    = Handle::savepointPrefix() + std::to_string(m_nestedLevel--);
    discardableExecute(StatementRollback().rollbackTo(savepointName));
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
    return Handle::execute(StatementBegin::immediate());
}

bool Handle::commitOrRollbackTransaction()
{
    m_nestedLevel = 0;
    if (!Handle::execute(StatementCommit::commit())) {
        discardableExecute(StatementRollback::rollback());
        return false;
    }
    return true;
}

void Handle::rollbackTransaction()
{
    m_nestedLevel = 0;
    discardableExecute(StatementRollback::rollback());
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

void Handle::discardableExecute(const Statement &statement)
{
    sqlite3_exec(
    (sqlite3 *) m_handle, statement.getDescription().c_str(), nullptr, nullptr, nullptr);
}

#pragma mark - Cipher
bool Handle::setCipherKey(const UnsafeData &data)
{
#ifdef SQLITE_HAS_CODEC
    int rc = sqlite3_key((sqlite3 *) m_handle, data.buffer(), (int) data.size());
    if (rc == SQLITE_OK) {
        return true;
    }
    setError(rc);
    return false;
#else  //SQLITE_HAS_CODEC
    Error::fatal("[sqlite3_key] is not supported for current config");
    return false;
#endif //SQLITE_HAS_CODEC
}

#pragma mark - Notification
void Handle::setNotificationWhenSQLTraced(const std::string &name, const SQLNotification &onTraced)
{
    m_notification.setNotificationWhenSQLTraced(name, onTraced);
}

void Handle::setNotificationWhenPerformanceTraced(const std::string &name,
                                                  const PerformanceNotification &onTraced)
{
    m_notification.setNotificationWhenPerformanceTraced(name, onTraced);
}

void Handle::setNotificationWhenCommitted(int order,
                                          const std::string &name,
                                          const CommittedNotification &onCommitted)
{
    m_notification.setNotificationWhenCommitted(order, name, onCommitted);
}

void Handle::unsetNotificationWhenCommitted(const std::string &name)
{
    m_notification.unsetNotificationWhenCommitted(name);
}

bool Handle::setNotificationWhenWillCheckpoint(int order,
                                               const std::string &name,
                                               const WCDB::Handle::WillCheckpointNotification &willCheckpoint)
{
    return m_notification.setNotificationWhenWillCheckpoint(order, name, willCheckpoint);
}

bool Handle::unsetNotificationWhenWillCheckpoint(const std::string &name)
{
    return m_notification.unsetNotificationWhenWillCheckpoint(name);
}

bool Handle::setNotificationWhenCheckpointed(const std::string &name,
                                             const CheckpointedNotification &checkpointed)
{
    return m_notification.setNotificationWhenCheckpointed(name, checkpointed);
}

#pragma mark - Error
void Handle::setError(int rc, const std::string &sql)
{
    if (rc != SQLITE_MISUSE) {
        m_error.setSQLiteCode(rc, getExtendedErrorCode());
    } else {
        m_error.setSQLiteCode(rc);
    }
    if (rc != m_ignorableCode) {
        m_error.level = Error::Level::Error;
    } else {
        m_error.level = Error::Level::Ignore;
    }
    const char *message = getErrorMessage();
    if (message) {
        m_error.message = message;
    } else {
        m_error.message = String::empty();
    }
    m_error.infos.set("SQL", sql);
    Notifier::shared()->notify(m_error);
}

void Handle::markAsIgnorable(int code)
{
    m_ignorableCode = code;
}

void Handle::markAsUnignorable()
{
    m_ignorableCode = SQLITE_OK;
}

} //namespace WCDB
