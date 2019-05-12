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

#include <WCDB/AbstractHandle.hpp>
#include <WCDB/Assertion.hpp>
#include <WCDB/CoreConst.h>
#include <WCDB/Notifier.hpp>
#include <WCDB/SQLite.h>
#include <WCDB/String.hpp>

namespace WCDB {

#pragma mark - Initialize
AbstractHandle::AbstractHandle()
: m_handle(nullptr), m_notification(this), m_nestedLevel(0)
{
}

AbstractHandle::~AbstractHandle()
{
    close();
}

sqlite3 *AbstractHandle::getRawHandle()
{
    WCTInnerAssert(isOpened());
    return m_handle;
}

#pragma mark - Global
void AbstractHandle::enableMultithread()
{
    int rc = sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
    WCTInnerAssert(rc == SQLITE_OK);
    tryNotifyError(rc);
}

//void AbstractHandle::setMemoryMapSize(int64_t defaultSizeLimit, int64_t maximumAllowedSizeLimit)
//{
//    int rc = sqlite3_config(SQLITE_CONFIG_MMAP_SIZE,
//                   (sqlite3_int64) defaultSizeLimit,
//                   (sqlite3_int64) maximumAllowedSizeLimit);
//    WCTInnerAssert(rc == SQLITE_OK);
//    tryNotifyError(rc);
//}

void AbstractHandle::enableMemoryStatus(bool enable)
{
    int rc = sqlite3_config(SQLITE_CONFIG_MEMSTATUS, (int) enable);
    WCTInnerAssert(rc == SQLITE_OK);
    tryNotifyError(rc);
}

void AbstractHandle::traceGlobalLog(const GlobalLog &log, void *parameter)
{
    int rc = sqlite3_config(SQLITE_CONFIG_LOG, log, parameter);
    WCTInnerAssert(rc == SQLITE_OK);
    tryNotifyError(rc);
}

void AbstractHandle::hookFileOpen(const FileOpen &open)
{
    sqlite3_vfs *vfs = sqlite3_vfs_find(nullptr);
    sqlite3_mutex *mutex = sqlite3_mutex_alloc(SQLITE_MUTEX_STATIC_MASTER);
    sqlite3_mutex_enter(mutex);
    vfs->xSetSystemCall(vfs, "open", (sqlite3_syscall_ptr) open);
    sqlite3_mutex_leave(mutex);
}

#pragma mark - Path
void AbstractHandle::setPath(const String &path)
{
    if (m_path != path) {
        close();
        m_path = path;
        m_error.infos.set(ErrorStringKeyPath, path);
    }
}

const String &AbstractHandle::getPath() const
{
    return m_path;
}

String AbstractHandle::getSHMSuffix()
{
    return "-shm";
}

String AbstractHandle::getWALSuffix()
{
    return "-wal";
}

String AbstractHandle::getJournalSuffix()
{
    return "-journal";
}

#pragma mark - Basic
bool AbstractHandle::open()
{
    WCTInnerAssert(!isOpened());
    bool succeed = exitAPI(sqlite3_open(m_path.c_str(), &m_handle));
    if (!succeed) {
        m_handle = nullptr;
    }
    return succeed;
}

bool AbstractHandle::isOpened() const
{
    return m_handle != nullptr;
}

void AbstractHandle::close()
{
    if (m_handle != nullptr) {
        finalizeStatements();
        WCTRemedialAssert(m_nestedLevel == 0 && !isInTransaction(),
                          "Unpaired transaction.",
                          rollbackTransaction(););
        m_notification.purge();
        exitAPI(sqlite3_close_v2(m_handle));
        m_handle = nullptr;
    }
}

bool AbstractHandle::executeSQL(const String &sql)
{
    // use seperated sqlite3_exec to get more information
    WCTInnerAssert(isOpened());
    HandleStatement handleStatement(this, &m_notification);
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

int AbstractHandle::getExtendedErrorCode()
{
    WCTInnerAssert(isOpened());
    return sqlite3_extended_errcode(m_handle);
}

long long AbstractHandle::getLastInsertedRowID()
{
    WCTInnerAssert(isOpened());
    return sqlite3_last_insert_rowid(m_handle);
}

Error::Code AbstractHandle::getResultCode()
{
    WCTInnerAssert(isOpened());
    return (Error::Code) sqlite3_errcode(m_handle);
}

const char *AbstractHandle::getErrorMessage()
{
    WCTInnerAssert(isOpened());
    return sqlite3_errmsg(m_handle);
}

int AbstractHandle::getChanges()
{
    WCTInnerAssert(isOpened());
    return sqlite3_changes(m_handle);
}

bool AbstractHandle::isReadonly()
{
    WCTInnerAssert(isOpened());
    return sqlite3_db_readonly(m_handle, NULL) == 1;
}

bool AbstractHandle::isInTransaction()
{
    WCTInnerAssert(isOpened());
    return sqlite3_get_autocommit(m_handle) == 0;
}

void AbstractHandle::interrupt()
{
    WCTInnerAssert(isOpened());
    sqlite3_interrupt(m_handle);
}

int AbstractHandle::getNumberOfDirtyPages()
{
    WCTInnerAssert(isOpened());
    return sqlite3_dirty_page_count(m_handle);
}

void AbstractHandle::enableExtendedResultCodes(bool enable)
{
    WCTInnerAssert(isOpened());
    exitAPI(sqlite3_extended_result_codes(m_handle, (int) enable));
}

bool AbstractHandle::checkpoint(CheckpointMode mode)
{
    static_assert((int) CheckpointMode::Passive == SQLITE_CHECKPOINT_PASSIVE, "");
    static_assert((int) CheckpointMode::Full == SQLITE_CHECKPOINT_FULL, "");
    static_assert((int) CheckpointMode::Restart == SQLITE_CHECKPOINT_RESTART, "");
    static_assert((int) CheckpointMode::Truncate == SQLITE_CHECKPOINT_TRUNCATE, "");
    WCTInnerAssert(isOpened());
    return exitAPI(sqlite3_wal_checkpoint_v2(
    m_handle, Syntax::mainSchema, (int) mode, nullptr, nullptr));
}

void AbstractHandle::disableCheckpointWhenClosing(bool disable)
{
    WCTInnerAssert(isOpened());
    exitAPI(sqlite3_db_config(
    m_handle, SQLITE_DBCONFIG_NO_CKPT_ON_CLOSE, (int) disable, nullptr));
}

#pragma mark - Statement
HandleStatement *AbstractHandle::getStatement()
{
    m_handleStatements.push_back(HandleStatement(this, &m_notification));
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
    }
}

void AbstractHandle::finalizeStatements()
{
    for (auto &handleStatement : m_handleStatements) {
        handleStatement.finalize();
    }
}

#pragma mark - Meta
std::pair<bool, bool> AbstractHandle::ft3TokenizerExists(const String &tokenizer)
{
    bool exists = false;
    markErrorAsIgnorable(Error::Code::Error);
    bool succeed = executeStatement(StatementSelect().select(
    Expression::function("fts3_tokenizer").invoke().arguments(tokenizer)));
    markErrorAsUnignorable();
    if (succeed) {
        exists = true;
    } else {
        if (getResultCode() == Error::Code::Error) {
            succeed = true;
        }
    }
    return { succeed, exists };
}

std::pair<bool, bool> AbstractHandle::tableExists(const String &table)
{
    return tableExists(Schema::main(), table);
}

std::pair<bool, bool> AbstractHandle::tableExists(const Schema &schema, const String &table)
{
    // TODO: use sqlite3_table_column_metadata to check if table exists? 1. performance 2. table created by other handles?
    HandleStatement *handleStatement = getStatement();
    StatementSelect statementSelect
    = StatementSelect().select(1).from(TableOrSubquery(table).schema(schema)).limit(0);
    markErrorAsIgnorable(Error::Code::Error);
    bool exists = false;
    bool succeed = handleStatement->prepare(statementSelect);
    if (succeed) {
        exists = true;
        finalizeStatements();
    } else {
        if (getResultCode() == Error::Code::Error) {
            succeed = true;
        }
    }
    markErrorAsUnignorable();
    returnStatement(handleStatement);
    return { succeed, exists };
}

std::pair<bool, std::set<String>> AbstractHandle::getColumns(const String &table)
{
    return getColumns(Schema::main(), table);
}

std::pair<bool, std::set<String>>
AbstractHandle::getColumns(const Schema &schema, const String &table)
{
    WCDB::StatementPragma statement
    = StatementPragma().pragma(Pragma::tableInfo()).schema(schema).with(table);
    return getValues(statement, 1);
}

std::pair<bool, std::vector<ColumnMeta>>
AbstractHandle::getTableMeta(const Schema &schema, const String &table)
{
    std::vector<ColumnMeta> columnMetas;
    HandleStatement *handleStatement = getStatement();
    bool succeed = false;
    do {
        if (handleStatement->prepare(
            StatementPragma().pragma(Pragma::tableInfo()).schema(schema).with(table))) {
            while ((succeed = handleStatement->step()) && !handleStatement->done()) {
                columnMetas.push_back(ColumnMeta(handleStatement->getInteger32(0), // cid
                                                 handleStatement->getText(1), // name
                                                 handleStatement->getText(2), // type
                                                 handleStatement->getInteger32(3), // notnull
                                                 handleStatement->getInteger32(5)) // pk
                );
            }
            handleStatement->finalize();
        }
    } while (false);
    returnStatement(handleStatement);
    if (!succeed) {
        columnMetas.clear();
    }
    return { succeed, std::move(columnMetas) };
}

std::pair<bool, std::set<String>>
AbstractHandle::getValues(const Statement &statement, int index)
{
    HandleStatement *handleStatement = getStatement();
    bool succeed = false;
    std::set<String> values;
    if (handleStatement->prepare(statement)) {
        while ((succeed = handleStatement->step()) && !handleStatement->done()) {
            values.emplace(handleStatement->getText(index));
        }
        handleStatement->finalize();
    }
    returnStatement(handleStatement);
    if (!succeed) {
        values.clear();
    }
    return { succeed, std::move(values) };
}

#pragma mark - Transaction
String AbstractHandle::getSavepointName(int nestedLevel)
{
    return "WCDBSavepoint_" + std::to_string(nestedLevel);
}

bool AbstractHandle::beginNestedTransaction()
{
    bool succeed = true;
    if (isInTransaction()) {
        ++m_nestedLevel;
        succeed = executeStatement(
        StatementSavepoint().savepoint(getSavepointName(m_nestedLevel)));
    } else {
        succeed = beginTransaction();
    }
    return succeed;
}

bool AbstractHandle::commitOrRollbackNestedTransaction()
{
    bool succeed = true;
    if (m_nestedLevel == 0) {
        succeed = commitOrRollbackTransaction();
    } else {
        String savepointName = getSavepointName(m_nestedLevel);
        if (!executeStatement(StatementRelease().release(savepointName))) {
            executeStatement(StatementRollback().rollbackToSavepoint(savepointName));
            succeed = false;
        }
        --m_nestedLevel;
    }
    return succeed;
}

void AbstractHandle::rollbackNestedTransaction()
{
    if (m_nestedLevel == 0) {
        rollbackTransaction();
    } else {
        String savepointName = getSavepointName(m_nestedLevel);
        executeStatement(StatementRollback().rollbackToSavepoint(savepointName));
        --m_nestedLevel;
    }
}

bool AbstractHandle::beginTransaction()
{
    static const String *s_beginImmediate
    = new String(StatementBegin().beginImmediate().getDescription());
    return executeSQL(*s_beginImmediate);
}

bool AbstractHandle::commitOrRollbackTransaction()
{
    static const String *s_commit
    = new String(StatementCommit().commit().getDescription());
    if (!executeSQL(*s_commit)) {
        rollbackTransaction();
        return false;
    }
    m_nestedLevel = 0;
    return true;
}

void AbstractHandle::rollbackTransaction()
{
    m_nestedLevel = 0;
    if (isInTransaction()) {
        // Transaction can be removed automatically in some case. e.g. interrupt step
        static const String *s_rollback
        = new String(StatementRollback().rollback().getDescription());
        executeSQL(*s_rollback);
    }
}

#pragma mark - Cipher
void AbstractHandle::setCipherKey(const UnsafeData &data)
{
    WCTInnerAssert(isOpened());
    exitAPI(sqlite3_key(m_handle, data.buffer(), (int) data.size()));
}

#pragma mark - Notification
void AbstractHandle::setNotificationWhenSQLTraced(const String &name,
                                                  const SQLNotification &onTraced)
{
    WCTInnerAssert(isOpened());
    m_notification.setNotificationWhenSQLTraced(name, onTraced);
}

void AbstractHandle::setNotificationWhenPerformanceTraced(const String &name,
                                                          const PerformanceNotification &onTraced)
{
    WCTInnerAssert(isOpened());
    m_notification.setNotificationWhenPerformanceTraced(name, onTraced);
}

void AbstractHandle::setNotificationWhenCommitted(int order,
                                                  const String &name,
                                                  const CommittedNotification &onCommitted)
{
    WCTInnerAssert(isOpened());
    m_notification.setNotificationWhenCommitted(order, name, onCommitted);
}

void AbstractHandle::unsetNotificationWhenCommitted(const String &name)
{
    WCTInnerAssert(isOpened());
    m_notification.unsetNotificationWhenCommitted(name);
}

void AbstractHandle::setNotificationWhenCheckpointed(const String &name,
                                                     const CheckpointedNotification &checkpointed)
{
    WCTInnerAssert(isOpened());
    m_notification.setNotificationWhenCheckpointed(name, checkpointed);
}

void AbstractHandle::setNotificationWhenBusy(const BusyNotification &busyNotification)
{
    WCTInnerAssert(isOpened());
    m_notification.setNotificationWhenBusy(busyNotification);
}

void AbstractHandle::setNotificationWhenStatementDidStep(const String &name,
                                                         const StatementDidStepNotification &notification)
{
    WCTInnerAssert(isOpened());
    m_notification.setNotificationWhenStatementDidStep(name, notification);
}

void AbstractHandle::setNotificationWhenStatementWillStep(const String &name,
                                                          const StatementWillStepNotification &notification)
{
    WCTInnerAssert(isOpened());
    m_notification.setNotificationWhenStatementWillStep(name, notification);
}

#pragma mark - Error
bool AbstractHandle::exitAPI(int rc)
{
    return exitAPI(rc, nullptr);
}

bool AbstractHandle::exitAPI(int rc, const String &sql)
{
    return exitAPI(rc, sql.c_str());
}

bool AbstractHandle::exitAPI(int rc, const char *sql)
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
    WCTInnerAssert(Error::isError(rc));
    if (Error::rc2c(rc) != Error::Code::Misuse) {
        m_error.setSQLiteCode(rc, getErrorMessage());
    } else {
        // extended error code/message will not be set in some case for misuse error
        m_error.setSQLiteCode(rc);
    }
    if (std::find(m_ignorableCodes.begin(), m_ignorableCodes.end(), rc)
        == m_ignorableCodes.end()) {
        m_error.level = Error::Level::Error;
    } else {
        m_error.level = Error::Level::Ignore;
    }
    m_error.infos.set(ErrorStringKeySQL, sql);
    Notifier::shared()->notify(m_error);
}

void AbstractHandle::markErrorAsIgnorable(Error::Code ignorableCode)
{
    m_ignorableCodes.emplace_back((int) ignorableCode);
}

void AbstractHandle::markErrorAsUnignorable()
{
    WCTInnerAssert(!m_ignorableCodes.empty());
    m_ignorableCodes.pop_back();
}

void AbstractHandle::tryNotifyError(int rc)
{
    if (Error::isError(rc)) {
        Error error;
        error.level = Error::Level::Fatal;
        error.setSQLiteCode(rc);
        Notifier::shared()->notify(error);
    }
}

} //namespace WCDB
