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
#include <WCDB/Macro.hpp>
#include <WCDB/Path.hpp>
#include <sqlcipher/sqlite3.h>
#include <sqliterk/SQLiteRepairKit.h>

namespace WCDB {

#pragma mark - Initialize
std::shared_ptr<Handle> Handle::handleWithPath(const std::string &path)
{
    return std::shared_ptr<Handle>(new Handle(path));
}

Handle::Handle(const std::string &path_)
    : m_handle(nullptr), path(path_), m_nestedLevel(0), m_tag(invalidTag)
{
    m_error.infos.set("Path", path);
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

std::string Handle::getBackupSubfix()
{
    return "-backup";
}

std::array<std::string, 5> Handle::getSubfixs()
{
    return std::array<std::string, 5>{
        "", //main db file
        Handle::getWALSubfix(),
        Handle::getSHMSubfix(),
        Handle::getJournalSubfix(),
        Handle::getBackupSubfix(),
    };
}

#pragma mark - Basic
void Handle::setTag(Tag tag)
{
    m_tag = tag;
    m_error.infos.set("Tag", m_tag);
}

Handle::Tag Handle::getTag() const
{
    return m_tag;
}

bool Handle::open()
{
    int rc = sqlite3_open(path.c_str(), (sqlite3 **) &m_handle);
    if (rc == SQLITE_OK) {
        return true;
    }
    setError(rc);
    return false;
}

void Handle::close()
{
    finalize();
    sqlite3_close_v2((sqlite3 *) m_handle);
}

bool Handle::execute(const Statement &statement)
{
    int rc =
        sqlite3_exec((sqlite3 *) m_handle, statement.getDescription().c_str(),
                     nullptr, nullptr, nullptr);
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
        handleStatement.setup(this, statement, stmt);
        return true;
    }
    setError(rc, statement.getDescription());
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
    return handleStatement.step(done);
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
    StatementSelect statementSelect =
        StatementSelect().select(1).from(table).limit(0);
    m_error.level = Error::Level::Ignore;
    bool unused;
    bool result = Handle::prepare(statementSelect) && Handle::step(unused);
    m_error.level = Error::Level::Error;
    finalize();
    return {result || getResultCode() == SQLITE_ERROR, result};
}

std::pair<bool, std::set<std::string>>
Handle::getUnorderedColumnsWithTable(const std::string &tableName,
                                     const std::string &schemaName)
{
    WCDB::StatementPragma statement = StatementPragma()
                                          .withSchema(schemaName)
                                          .pragma(Pragma::tableInfo())
                                          .with(tableName);
    return getUnorderedValues(statement, 1);
}

std::pair<bool, std::set<std::string>> Handle::getUnorderedAttachedSchemas()
{
    return getUnorderedValues(StatementPragma().pragma(Pragma::databaseList()),
                              1);
}

std::pair<bool, std::set<std::string>>
Handle::getUnorderedValues(const Statement &statement, int index)
{
    if (Handle::prepare(statement)) {
        bool done = false;
        std::set<std::string> values;
        while (Handle::step(done) && !done) {
            values.insert(getText(index));
        }
        finalize();
        if (done) {
            return {true, std::move(values)};
        }
    }
    return {false, {}};
}

const std::string Handle::savepointPrefix()
{
    return "WCDBSavepoint_";
}

bool Handle::beginNestedTransaction()
{
    if (!isInTransaction()) {
        return beginTransaction();
    }
    std::string savepointName =
        Handle::savepointPrefix() + std::to_string(++m_nestedLevel);
    return Handle::execute(StatementSavepoint().savepoint(savepointName));
}

bool Handle::commitOrRollbackNestedTransaction()
{
    if (m_nestedLevel == 0) {
        return commitOrRollbackTransaction();
    }
    std::string savepointName =
        Handle::savepointPrefix() + std::to_string(m_nestedLevel--);
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
    std::string savepointName =
        Handle::savepointPrefix() + std::to_string(m_nestedLevel--);
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
    sqlite3_exec((sqlite3 *) m_handle, statement.getDescription().c_str(),
                 nullptr, nullptr, nullptr);
}

#pragma mark - Cipher
bool Handle::setCipherKey(const void *data, int size)
{
#ifdef SQLITE_HAS_CODEC
    int rc = sqlite3_key((sqlite3 *) m_handle, data, size);
    if (rc == SQLITE_OK) {
        return true;
    }
    setError(rc);
#else  //SQLITE_HAS_CODEC
    WCTFatalError("[sqlite3_key] is not supported for current config");
#endif //SQLITE_HAS_CODEC
    return false;
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
bool Handle::backup(const NoCopyData &data)
{
    std::string backupPath = Path::addExtention(path, getBackupSubfix());
    int rc = sqliterk_save_master((sqlite3 *) m_handle, backupPath.c_str(),
                                  data.data, (int) data.size);
    if (rc == SQLITERK_OK) {
        return true;
    }
    setError(rc);
    return false;
}

bool Handle::recoverFromPath(const std::string &corruptedDBPath,
                             int pageSize,
                             const NoCopyData &backupCipher,
                             const NoCopyData &databaseCipher)
{
    int rc = SQLITERK_OK;
    do {
        std::string backupPath =
            Path::addExtention(corruptedDBPath, Handle::getBackupSubfix());
        sqliterk_master_info *info;
        unsigned char kdfSalt[16];
        memset(kdfSalt, 0, 16);
        rc = sqliterk_load_master(backupPath.c_str(), backupCipher.data,
                                  (int) backupCipher.size, nullptr, 0, &info,
                                  kdfSalt);
        if (rc != SQLITERK_OK) {
            break;
        }

        sqliterk_cipher_conf conf;
        memset(&conf, 0, sizeof(sqliterk_cipher_conf));
        conf.key = databaseCipher.data;
        conf.key_len = (int) databaseCipher.size;
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
    setError(rc);
    return false;
}

#pragma mark - Error
void Handle::setError(int rc, const std::string &sql)
{
    m_error.setSQLiteCode(rc, getExtendedErrorCode());
    const char *message = getErrorMessage();
    if (message) {
        m_error.message = message;
    }
    m_error.infos.set("SQL", sql);
    Reporter::shared()->report(m_error);
}

} //namespace WCDB
