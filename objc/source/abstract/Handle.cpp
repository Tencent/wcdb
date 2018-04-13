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

#include <WCDB/ConvertibleImpl.hpp>
#include <WCDB/Handle.hpp>
#include <WCDB/Macro.hpp>
#include <WCDB/Path.hpp>
#include <WCDB/SQLiteError.hpp>
#include <sqlcipher/sqlite3.h>
#include <sqliterk/SQLiteRepairKit.h>

namespace WCDB {

#pragma mark - Initialize
std::shared_ptr<Handle> Handle::handleWithPath(const std::string &path, Tag tag)
{
    return std::shared_ptr<Handle>(new Handle(path, tag));
}

Handle::Handle(const std::string &path_, Tag tag)
    : m_handle(nullptr), path(path_), m_nestedLevel(0)
{
    m_error.tag = tag;
    m_error.path = path_;
}

#pragma mark - Path
const std::string &Handle::getSHMSubfix()
{
    static const std::string s_SHMSubfix = "-shm";
    return s_SHMSubfix;
}

const std::string &Handle::getWALSubfix()
{
    static const std::string s_WALSubfix = "-wal";
    return s_WALSubfix;
}

const std::string &Handle::getJournalSubfix()
{
    static const std::string s_journalSubfix = "-journal";
    return s_journalSubfix;
}

const std::string &Handle::getBackupSubfix()
{
    static const std::string s_backupSubfix = "-backup";
    return s_backupSubfix;
}

const std::array<std::string, 5> &Handle::getSubfixs()
{
    static const std::array<std::string, 5> s_subfixs = {
        "", //main db file
        Handle::getWALSubfix(),
        Handle::getSHMSubfix(),
        Handle::getJournalSubfix(),
        Handle::getBackupSubfix(),
    };
    return s_subfixs;
}

#pragma mark - Basic
void Handle::setTag(Tag tag)
{
    m_error.tag = tag;
}

Handle::Tag Handle::getTag() const
{
    return m_error.tag;
}

bool Handle::open()
{
    int rc = sqlite3_open(path.c_str(), (sqlite3 **) &m_handle);
    if (rc == SQLITE_OK) {
        return true;
    }
    setupError();
    m_error.operation = Operation::Open;
    m_error.report();
    return false;
}

void Handle::close()
{
    finalize();
    sqlite3_close((sqlite3 *) m_handle);
}

bool Handle::execute(const Statement &statement)
{
    int rc =
        sqlite3_exec((sqlite3 *) m_handle, statement.getDescription().c_str(),
                     nullptr, nullptr, nullptr);
    if (rc == SQLITE_OK) {
        return true;
    }
    setupError();
    m_error.operation = Operation::Execute;
    m_error.statement = statement;
    m_error.report();
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

void Handle::setCommittedHook(const CommittedCallback &onCommitted, void *info)
{
    m_committedHookInfo.onCommitted = onCommitted;
    m_committedHookInfo.info = info;
    m_committedHookInfo.handle = this;
    if (m_committedHookInfo.onCommitted) {
        sqlite3_wal_hook(
            (sqlite3 *) m_handle,
            [](void *p, sqlite3 *, const char *, int pages) -> int {
                CommittedHookInfo *committedHookInfo = (CommittedHookInfo *) p;
                committedHookInfo->onCommitted(committedHookInfo->handle, pages,
                                               committedHookInfo->info);
                return SQLITE_OK;
            },
            &m_committedHookInfo);
    } else {
        sqlite3_wal_hook((sqlite3 *) m_handle, nullptr, nullptr);
    }
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
    return prepare(statement, m_handleStatement);
}

bool Handle::prepare(const Statement &statement,
                     HandleStatement &handleStatement)
{
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2((sqlite3 *) m_handle,
                                statement.getDescription().c_str(), -1,
                                (sqlite3_stmt **) &stmt, nullptr);
    if (rc == SQLITE_OK) {
        handleStatement.setup(statement, stmt);
        return true;
    }
    setupError();
    m_error.operation = Operation::Prepare;
    m_error.statement = statement;
    m_error.report();
    return false;
}

void Handle::reset()
{
    m_handleStatement.reset();
}

bool Handle::step(bool &done)
{
    return step(m_handleStatement, done);
}

bool Handle::step()
{
    bool unused;
    return step(unused);
}

bool Handle::step(HandleStatement &handleStatement, bool &done)
{
    if (handleStatement.step(done)) {
        return true;
    }
    setupError();
    m_error.operation = Operation::Step;
    m_error.statement = handleStatement.getStatement();
    m_error.report();
    return false;
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
std::pair<bool, bool> Handle::isTableExists(const TableOrSubquery &table)
{
    static const StatementSelect s_statementSelect =
        StatementSelect().select(1).limit(0);
    StatementSelect statementSelect = s_statementSelect;
    statementSelect.from(table);
    m_error.level = Error::Level::Ignore;
    bool result = Handle::prepare(statementSelect) && step();
    m_error.level = Error::Level::Error;
    finalize();
    return {result || getResultCode() == SQLITE_ERROR, result};
}

std::pair<bool, std::list<std::string>>
Handle::getColumnsWithTable(const std::string &tableName)
{
    return getPragmaValues(
        StatementPragma().pragma(Pragma::TableInfo).to(tableName), 1);
}

std::pair<bool, std::list<std::string>> Handle::getAttachedSchemas()
{
    return getPragmaValues(StatementPragma().pragma(Pragma::DatabaseList), 1);
}

std::pair<bool, bool> Handle::isSchemaExists(const std::string &schemaName)
{
    std::pair<bool, std::list<std::string>> pair = getAttachedSchemas();
    if (!pair.first) {
        return {false, false};
    }
    for (const std::string &schema : pair.second) {
        if (schema == schemaName) {
            return {true, true};
        }
    }
    return {true, false};
}

std::pair<bool, std::list<std::string>>
Handle::getPragmaValues(const StatementPragma &statement, int index)
{
    if (prepare(statement)) {
        bool done;
        std::list<std::string> values;
        while (step(done) && !done) {
            values.push_back(getText(index));
        }
        finalize();
        if (done) {
            return {true, values};
        }
    }
    return {false, {}};
}

const std::string Handle::s_savepointPrefix("WCDBSavepoint_");

bool Handle::beginNestedTransaction()
{
    if (!isInTransaction()) {
        return beginTransaction();
    }
    std::string savepointName =
        s_savepointPrefix + std::to_string(++m_nestedLevel);
    return execute(StatementSavepoint().savepoint(savepointName));
}

bool Handle::commitOrRollbackNestedTransaction()
{
    if (m_nestedLevel == 0) {
        return commitOrRollbackTransaction();
    }
    std::string savepointName =
        s_savepointPrefix + std::to_string(m_nestedLevel--);
    if (!execute(StatementRelease().release(savepointName))) {
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
    std::string savepointName =
        s_savepointPrefix + std::to_string(m_nestedLevel--);
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
    return execute(StatementBegin::immediate);
}

bool Handle::commitOrRollbackTransaction()
{
    m_nestedLevel = 0;
    if (!execute(StatementCommit::commit)) {
        discardableExecute(StatementRollback::rollback);
        return false;
    }
    return true;
}

void Handle::rollbackTransaction()
{
    m_nestedLevel = 0;
    discardableExecute(StatementRollback::rollback);
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
    sqlite3_exec((sqlite3 *) m_handle, statement.getDescription().c_str(),
                 nullptr, nullptr, nullptr);
}

#pragma mark - Cipher
bool Handle::setCipherKey(const void *data, int size)
{
#ifdef SQLITE_HAS_CODEC
    if (sqlite3_key((sqlite3 *) m_handle, data, size) == SQLITE_OK) {
        return true;
    }
    setupError();
    m_error.operation = Operation::Cipher;
    m_error.report();
    return false;
#else  //SQLITE_HAS_CODEC
    Error::fatal("[sqlite3_key] is not supported for current config");
    return false;
#endif //SQLITE_HAS_CODEC
}

#pragma mark - Trace
void Handle::traceSQL(const SQLTraceCallback &trace)
{
    m_tracer.traceSQL(m_handle, trace);
}

void Handle::tracePerformance(const PerformanceTraceCallback &trace)
{
    m_tracer.tracePerformance(
        m_handle,
        [this, trace](const Tracer::Footprints &footprints, int64_t cost) {
            trace(getTag(), footprints, cost);
        });
}

#pragma mark - Repair Kit
bool Handle::backup(const void *key, unsigned int length)
{
    std::string backupPath = Path::addExtention(path, getBackupSubfix());
    int rc = sqliterk_save_master((sqlite3 *) m_handle, backupPath.c_str(), key,
                                  length);
    if (rc == SQLITERK_OK) {
        return true;
    }
    m_error.reset();
    m_error.code = rc;
    m_error.operation = Operation::Backup;
    m_error.report();
    return false;
}

bool Handle::recoverFromPath(const std::string &corruptedDBPath,
                             int pageSize,
                             const void *backupKey,
                             unsigned int backupKeyLength,
                             const void *databaseKey,
                             unsigned int databaseKeyLength)
{
    int rc = SQLITERK_OK;
    do {
        std::string backupPath =
            Path::addExtention(corruptedDBPath, Handle::getBackupSubfix());
        sqliterk_master_info *info;
        unsigned char kdfSalt[16];
        memset(kdfSalt, 0, 16);
        rc = sqliterk_load_master(backupPath.c_str(), backupKey,
                                  backupKeyLength, nullptr, 0, &info, kdfSalt);
        if (rc != SQLITERK_OK) {
            break;
        }

        sqliterk_cipher_conf conf;
        memset(&conf, 0, sizeof(sqliterk_cipher_conf));
        conf.key = databaseKey;
        conf.key_len = databaseKeyLength;
        conf.page_size = pageSize;
        conf.kdf_salt = kdfSalt;
        conf.use_hmac = true;

        sqliterk *rk;
        rc = sqliterk_open(corruptedDBPath.c_str(), &conf, &rk);
        if (rc != SQLITERK_OK) {
            break;
        }

        rc = sqliterk_output(rk, (sqlite3 *) m_handle, info,
                             SQLITERK_OUTPUT_ALL_TABLES);
    } while (false);
    if (rc == SQLITERK_OK) {
        return true;
    }
    m_error.reset();
    m_error.code = rc;
    m_error.operation = Operation::Repair;
    m_error.report();
    return false;
}

#pragma mark - Error
const HandleError &Handle::getError() const
{
    return m_error;
}

void Handle::setupError()
{
    m_error.reset();
    m_error.code = getResultCode();
    m_error.message = getErrorMessage();
    m_error.extendedCode = getExtendedErrorCode();
}

} //namespace WCDB
