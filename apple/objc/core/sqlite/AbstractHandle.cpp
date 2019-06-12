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

#pragma mark - Path
void AbstractHandle::setPath(const String &path)
{
    if (m_path != path) {
        close();
        m_path = path;
        m_error.infos.set(ErrorStringKeyPath, path);
    }
}

void AbstractHandle::clearPath()
{
    close();
    m_path.clear();
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
    bool succeed = true;
    if (!isOpened()) {
        succeed = APIExit(sqlite3_open(m_path.c_str(), &m_handle));
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
        WCTRemedialAssert(m_nestedLevel == 0 && !isInTransaction(),
                          "Unpaired transaction.",
                          rollbackTransaction(););
        m_notification.purge();
        APIExit(sqlite3_close_v2(m_handle));
        m_handle = nullptr;
    }
}

bool AbstractHandle::executeSQL(const String &sql)
{
    // use seperated sqlite3_exec to get more information
    WCTInnerAssert(isOpened());
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
    WCTInnerAssert(isOpened());
    return sqlite3_last_insert_rowid(m_handle);
}

//Error::Code AbstractHandle::getResultCode()
//{
//    WCTInnerAssert(isOpened());
//    return (Error::Code) sqlite3_errcode(m_handle);
//}
//
//const char *AbstractHandle::getErrorMessage()
//{
//    WCTInnerAssert(isOpened());
//    return sqlite3_errmsg(m_handle);
//}
//int AbstractHandle::getExtendedErrorCode()
//{
//    WCTInnerAssert(isOpened());
//    return sqlite3_extended_errcode(m_handle);
//}

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

#pragma mark - Statement
HandleStatement *AbstractHandle::getStatement()
{
    m_handleStatements.push_back(HandleStatement(this));
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
        WCTInnerAssert(false);
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
        if (m_error.code() == Error::Code::Error) {
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
    StatementSelect statement
    = StatementSelect().select(1).from(TableOrSubquery(table).schema(schema)).limit(1);

    HandleStatement handleStatement(this);
    markErrorAsIgnorable(Error::Code::Error);
    bool succeed = handleStatement.prepare(statement);
    bool exists = succeed;
    if (succeed) {
        handleStatement.finalize();
    } else if (isErrorIgnorable()) {
        succeed = true;
    }
    markErrorAsUnignorable();
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
    HandleStatement handleStatement(this);
    bool succeed = false;
    do {
        if (handleStatement.prepare(
            StatementPragma().pragma(Pragma::tableInfo()).schema(schema).with(table))) {
            while ((succeed = handleStatement.step()) && !handleStatement.done()) {
                columnMetas.push_back(ColumnMeta(handleStatement.getInteger32(0), // cid
                                                 handleStatement.getText(1), // name
                                                 handleStatement.getText(2), // type
                                                 handleStatement.getInteger32(3), // notnull
                                                 handleStatement.getInteger32(5)) // pk
                );
            }
            handleStatement.finalize();
        }
    } while (false);
    if (!succeed) {
        columnMetas.clear();
    }
    return { succeed, std::move(columnMetas) };
}

std::pair<bool, std::set<String>>
AbstractHandle::getValues(const Statement &statement, int index)
{
    HandleStatement handleStatement(this);
    bool succeed = false;
    std::set<String> values;
    if (handleStatement.prepare(statement)) {
        while ((succeed = handleStatement.step()) && !handleStatement.done()) {
            values.emplace(handleStatement.getText(index));
        }
        handleStatement.finalize();
    }
    if (!succeed) {
        values.clear();
    }
    return { succeed, std::move(values) };
}

#pragma mark - Transaction
String AbstractHandle::getSavepointName(int nestedLevel)
{
    return String::formatted("WCDBSavepoint_%d", nestedLevel);
}

bool AbstractHandle::beginNestedTransaction()
{
    bool succeed = true;
    if (isInTransaction()) {
        ++m_nestedLevel;

        static const String *s_savepoint_1 = new String(
        StatementSavepoint().savepoint(getSavepointName(1)).getDescription());
        static const String *s_savepoint_2 = new String(
        StatementSavepoint().savepoint(getSavepointName(2)).getDescription());
        static const String *s_savepoint_3 = new String(
        StatementSavepoint().savepoint(getSavepointName(3)).getDescription());

        switch (m_nestedLevel) {
        case 1:
            succeed = executeSQL(*s_savepoint_1);
            break;
        case 2:
            succeed = executeSQL(*s_savepoint_2);
            break;
        case 3:
            succeed = executeSQL(*s_savepoint_3);
            break;
        default:
            succeed = executeStatement(
            StatementSavepoint().savepoint(getSavepointName(m_nestedLevel)));
            break;
        }
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
        static const String *s_savepoint_1
        = new String(StatementRelease().release(getSavepointName(1)).getDescription());
        static const String *s_savepoint_2
        = new String(StatementRelease().release(getSavepointName(2)).getDescription());
        static const String *s_savepoint_3
        = new String(StatementRelease().release(getSavepointName(3)).getDescription());

        switch (m_nestedLevel) {
        case 1:
            succeed = executeSQL(*s_savepoint_1);
            break;
        case 2:
            succeed = executeSQL(*s_savepoint_2);
            break;
        case 3:
            succeed = executeSQL(*s_savepoint_3);
            break;
        default:
            succeed = executeStatement(
            StatementRelease().release(getSavepointName(m_nestedLevel)));
            break;
        }
        if (succeed) {
            --m_nestedLevel;
        } else {
            rollbackNestedTransaction();
        }
    }
    return succeed;
}

void AbstractHandle::rollbackNestedTransaction()
{
    if (m_nestedLevel == 0) {
        rollbackTransaction();
    } else {
        static const String *s_savepoint_1 = new String(
        StatementRollback().rollbackToSavepoint(getSavepointName(1)).getDescription());
        static const String *s_savepoint_2 = new String(
        StatementRollback().rollbackToSavepoint(getSavepointName(2)).getDescription());
        static const String *s_savepoint_3 = new String(
        StatementRollback().rollbackToSavepoint(getSavepointName(3)).getDescription());

        bool succeed = false;
        switch (m_nestedLevel) {
        case 1:
            succeed = executeSQL(*s_savepoint_1);
            break;
        case 2:
            succeed = executeSQL(*s_savepoint_2);
            break;
        case 3:
            succeed = executeSQL(*s_savepoint_3);
            break;
        default:
            succeed = executeStatement(
            StatementRollback().rollbackToSavepoint(getSavepointName(m_nestedLevel)));
            break;
        }
        if (succeed) {
            --m_nestedLevel;
        }
    }
}

bool AbstractHandle::beginTransaction()
{
    return beginTransaction(false);
}

bool AbstractHandle::beginTransaction(bool force)
{
    if (!force) {
        WCTRemedialAssert(!isInTransaction(),
                          "Last transaction is not committed or rollbacked.",
                          rollbackTransaction(););
    }

    static const String *s_beginImmediate
    = new String(StatementBegin().beginImmediate().getDescription());
    bool succeed = executeSQL(*s_beginImmediate);
    if (succeed) {
        m_nestedLevel = 0;
    }
    return succeed;
}

bool AbstractHandle::commitOrRollbackTransaction()
{
    static const String *s_commit
    = new String(StatementCommit().commit().getDescription());
    bool succeed = executeSQL(*s_commit);
    if (succeed) {
        m_nestedLevel = 0;
    } else {
        // If certain kinds of errors occur within a transaction, the transaction may or may not be rolled back automatically: https://sqlite.org/lang_transaction.html
        rollbackTransaction();
    }
    return succeed;
}

void AbstractHandle::rollbackTransaction()
{
    // Transaction can be removed automatically in some case. e.g. interrupt step
    static const String *s_rollback
    = new String(StatementRollback().rollback().getDescription());
    if (isInTransaction()) {
        if (executeSQL(*s_rollback)) {
            m_nestedLevel = 0;
        }
    }
}

#pragma mark - Interface
void AbstractHandle::setCipherKey(const UnsafeData &data)
{
    WCTInnerAssert(isOpened());
    APIExit(sqlite3_key(m_handle, data.buffer(), (int) data.size()));
}

int AbstractHandle::getNumberOfDirtyPages()
{
    WCTInnerAssert(isOpened());
    return sqlite3_dirty_page_count(m_handle);
}

void AbstractHandle::enableExtendedResultCodes(bool enable)
{
    WCTInnerAssert(isOpened());
    APIExit(sqlite3_extended_result_codes(m_handle, (int) enable));
}

bool AbstractHandle::checkpoint(CheckpointMode mode)
{
    static_assert((int) CheckpointMode::Passive == SQLITE_CHECKPOINT_PASSIVE, "");
    static_assert((int) CheckpointMode::Full == SQLITE_CHECKPOINT_FULL, "");
    static_assert((int) CheckpointMode::Restart == SQLITE_CHECKPOINT_RESTART, "");
    static_assert((int) CheckpointMode::Truncate == SQLITE_CHECKPOINT_TRUNCATE, "");
    WCTInnerAssert(isOpened());

    return APIExit(sqlite3_wal_checkpoint_v2(
    m_handle, Syntax::mainSchema, (int) mode, nullptr, nullptr));
}

void AbstractHandle::disableCheckpointWhenClosing(bool disable)
{
    WCTInnerAssert(isOpened());
    APIExit(sqlite3_db_config(
    m_handle, SQLITE_DBCONFIG_NO_CKPT_ON_CLOSE, (int) disable, nullptr));
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

#pragma mark - Error
bool AbstractHandle::APIExit(int rc)
{
    return APIExit(rc, nullptr);
}

bool AbstractHandle::APIExit(int rc, const String &sql)
{
    return APIExit(rc, sql.c_str());
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
    WCTInnerAssert(Error::isError(rc));
    if (Error::rc2c(rc) != Error::Code::Misuse && sqlite3_errcode(m_handle) == rc) {
        m_error.setSQLiteCode(rc, sqlite3_errmsg(m_handle));
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

bool AbstractHandle::isErrorIgnorable() const
{
    bool ignorable = true;
    int rc = Error::c2rc(m_error.code());
    if (Error::isError(rc)) {
        ignorable = false;
        for (const auto &code : m_ignorableCodes) {
            if (code == rc) {
                ignorable = true;
                break;
            }
        }
    }
    return ignorable;
}

} //namespace WCDB
