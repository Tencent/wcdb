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

#include <WCDB/File.hpp>
#include <WCDB/Handle.hpp>
#include <WCDB/Macro.hpp>
#include <WCDB/Path.hpp>
#include <sqlcipher/sqlite3.h>
#include <sqliterk/SQLiteRepairKit.h>

namespace WCDB {

std::shared_ptr<Handle> Handle::handleWithPath(const std::string &path)
{
    static std::once_flag s_flag;
    std::call_once(s_flag, []() {
        sqlite3_config(SQLITE_CONFIG_LOG,
                       [](void *userInfo, int code, const char *message) {
                           Error error;
                           error.setError(Error::Type::SQLiteGlobal, code);
                           error.setMessage(message ? message : nullptr);
                       },
                       nullptr);
        sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
        sqlite3_config(SQLITE_CONFIG_MEMSTATUS, false);
        //    sqlite3_config(SQLITE_CONFIG_MMAP_SIZE, 0x7fff0000, 0x7fff0000);
    });
    return std::shared_ptr<Handle>(new Handle(path));
}

Handle::Handle(const std::string &p)
    : m_handle(nullptr)
    , m_stmt(nullptr)
    , path(p)
    , m_performanceTrace(nullptr)
    , m_sqlTrace(nullptr)
    , m_cost(0)
    , m_aggregation(false)
    , m_skipError(false)
{
}

Handle::~Handle()
{
    reportPerformance();
}

bool Handle::open()
{
    if (!File::createDirectoryWithIntermediateDirectories(
            Path::getBaseName(path))) {
        File::getError().report();
    }
    if (sqlite3_open(path.c_str(), (sqlite3 **) &m_handle) == SQLITE_OK) {
        return true;
    }
    setupAndReportErrorWithPath(m_error, path);
    return false;
}

void Handle::close()
{
    finalize();
    sqlite3_close(m_handle);
}

std::string Handle::getSHMPath() const
{
    return Path::addExtention(path, Handle::getSHMSubfix());
}

std::string Handle::getWALPath() const
{
    return Path::addExtention(path, Handle::getWALSubfix());
}

std::string Handle::getJournalPath() const
{
    return Path::addExtention(path, Handle::getJournalSubfix());
}

std::string Handle::getBackupPath() const
{
    return Path::addExtention(path, Handle::getBackupSubfix());
}

std::list<std::string> Handle::getPaths() const
{
    return {path, getWALPath(), getSHMPath(), getJournalPath(),
            getBackupPath()};
}

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

void Handle::setupTrace()
{
    unsigned flag = 0;
    if (m_sqlTrace) {
        flag |= SQLITE_TRACE_STMT;
    }
    if (m_performanceTrace) {
        flag |= SQLITE_TRACE_PROFILE;
    }
    if (flag > 0) {
        sqlite3_trace_v2(
            m_handle, flag,
            [](unsigned int flag, void *M, void *P, void *X) -> int {
                Handle *handle = (Handle *) M;
                sqlite3_stmt *stmt = (sqlite3_stmt *) P;
                switch (flag) {
                    case SQLITE_TRACE_STMT: {
                        const char *sql = sqlite3_sql(stmt);
                        if (sql) {
                            handle->reportSQL(sql);
                        }
                    } break;
                    case SQLITE_TRACE_PROFILE: {
                        sqlite3_int64 *cost = (sqlite3_int64 *) X;
                        const char *sql = sqlite3_sql(stmt);

                        //report last trace
                        if (!handle->shouldPerformanceAggregation()) {
                            handle->reportPerformance();
                        }

                        if (sql) {
                            handle->addPerformanceTrace(sql, *cost);
                        }
                    } break;
                    default:
                        break;
                }

                return SQLITE_OK;
            },
            this);
    } else {
        sqlite3_trace_v2(m_handle, 0, nullptr, nullptr);
    }
}

void Handle::setSQLTrace(const SQLTraceCallback &trace)
{
    m_sqlTrace = trace;
    setupTrace();
}

void Handle::setPerformanceTrace(const PerformanceTraceCallback &trace)
{
    m_performanceTrace = trace;
    setupTrace();
}

bool Handle::shouldPerformanceAggregation() const
{
    return m_aggregation;
}

void Handle::addPerformanceTrace(const std::string &sql, const int64_t &cost)
{
    auto iter = m_footprint.find(sql);
    if (iter == m_footprint.end()) {
        m_footprint.insert({sql, 1});
    } else {
        ++iter->second;
    }
    m_cost += cost;
}

void Handle::reportPerformance()
{
    if (!m_footprint.empty()) {
        m_performanceTrace(m_footprint, m_cost);
        m_footprint.clear();
        m_cost = 0;
    }
}

void Handle::reportSQL(const std::string &sql)
{
    if (m_sqlTrace) {
        m_sqlTrace(sql);
    }
}

bool Handle::prepare(const Statement &statement)
{
    //TODO hook step
    if (statement.getType() == Statement::Type::Begin ||
        statement.getType() == Statement::Type::Commit ||
        statement.getType() == Statement::Type::Rollback) {
        Error::Abort(
            "[prepare] a transaction is not allowed, use [execute] instead");
        return false;
    }
    if (sqlite3_prepare_v2(m_handle, statement.getDescription().c_str(), -1,
                           &m_stmt, nullptr) == SQLITE_OK) {
        return true;
    }
    setupAndReportErrorWithSQL(m_error, statement.getDescription());
    return false;
}

void Handle::reset()
{
    sqlite3_reset(m_stmt);
}

bool Handle::execute(const Statement &statement)
{
    int rc = sqlite3_exec(m_handle, statement.getDescription().c_str(), nullptr,
                          nullptr, nullptr);
    bool result = rc == SQLITE_OK;
    switch (statement.getType()) {
        case Statement::Type::Begin:
            if (result) {
                m_aggregation = true;
            }
            break;
        case Statement::Type::Commit:
            if (result) {
                m_aggregation = false;
            }
            break;
        case Statement::Type::Rollback:
            m_aggregation = false;
            break;
        default:
            break;
    }
    if (result) {
        return true;
    }
    setupAndReportErrorWithSQL(m_error, statement.getDescription());
    return false;
}

bool Handle::step(bool &done)
{
    int rc = sqlite3_step(m_stmt);
    bool result = rc == SQLITE_OK || rc == SQLITE_ROW || rc == SQLITE_DONE;
    done = rc == SQLITE_DONE;
    if (result) {
        return true;
    }
    setupAndReportErrorWithSQL(m_error, sqlite3_sql(m_stmt));
    return false;
}

bool Handle::step()
{
    bool unused;
    return step(unused);
}

void Handle::finalize()
{
    if (m_stmt) {
        sqlite3_finalize(m_stmt);
        m_stmt = nullptr;
    }
}

int Handle::getExtendedErrorCode()
{
    return sqlite3_extended_errcode(m_handle);
}

long long Handle::getLastInsertedRowID()
{
    return sqlite3_last_insert_rowid(m_handle);
}

int Handle::getResultCode() const
{
    return sqlite3_errcode(m_handle);
}

const char *Handle::getErrorMessage()
{
    return sqlite3_errmsg(m_handle);
}

int Handle::getColumnCount()
{
    return sqlite3_column_count(m_stmt);
}

const char *Handle::getColumnName(int index)
{
    return sqlite3_column_name(m_stmt, index);
}

const char *Handle::getColumnTableName(int index)
{
    return sqlite3_column_table_name(m_stmt, index);
}

ColumnType Handle::getType(int index)
{
    switch (sqlite3_column_type(m_stmt, index)) {
        case SQLITE_INTEGER:
            return ColumnType::Integer64;
        case SQLITE_FLOAT:
            return ColumnType::Float;
        case SQLITE_BLOB:
            return ColumnType::BLOB;
        case SQLITE_TEXT:
            return ColumnType::Text;
        default:
            return ColumnType::Null;
    }
}

void Handle::bindInteger32(
    const ColumnTypeInfo<ColumnType::Integer32>::UnderlyingType &value,
    int index)
{
    sqlite3_bind_int(m_stmt, index, value);
}

void Handle::bindInteger64(
    const ColumnTypeInfo<ColumnType::Integer64>::UnderlyingType &value,
    int index)
{
    sqlite3_bind_int64(m_stmt, index, value);
}

void Handle::bindDouble(
    const ColumnTypeInfo<ColumnType::Float>::UnderlyingType &value, int index)
{
    sqlite3_bind_double(m_stmt, index, value);
}

void Handle::bindText(
    const ColumnTypeInfo<ColumnType::Text>::UnderlyingType &value, int index)
{
    sqlite3_bind_text(m_stmt, index, value, -1, SQLITE_TRANSIENT);
}

void Handle::bindBLOB(
    const ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType &value, int index)
{
    sqlite3_bind_blob(m_stmt, index, value.data, (int) value.size,
                      SQLITE_TRANSIENT);
}

void Handle::bindNull(int index)
{
    sqlite3_bind_null(m_stmt, index);
}

ColumnTypeInfo<ColumnType::Integer32>::UnderlyingType
Handle::getInteger32(int index)
{
    return (typename ColumnTypeInfo<ColumnType::Integer32>::UnderlyingType)
        sqlite3_column_int(m_stmt, index);
}

ColumnTypeInfo<ColumnType::Integer64>::UnderlyingType
Handle::getInteger64(int index)
{
    return (typename ColumnTypeInfo<ColumnType::Integer64>::UnderlyingType)
        sqlite3_column_int64(m_stmt, index);
}

ColumnTypeInfo<ColumnType::Float>::UnderlyingType Handle::getDouble(int index)
{
    return (typename ColumnTypeInfo<ColumnType::Float>::UnderlyingType)
        sqlite3_column_double(m_stmt, index);
}

ColumnTypeInfo<ColumnType::Text>::UnderlyingType Handle::getText(int index)
{
    return (typename ColumnTypeInfo<ColumnType::Text>::UnderlyingType)
        sqlite3_column_text(m_stmt, index);
}

ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType Handle::getBLOB(int index)
{
    int size = sqlite3_column_bytes(m_stmt, index);
    const unsigned char *data =
        (const unsigned char *) sqlite3_column_blob(m_stmt, index);
    return ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType(data, size);
}

bool Handle::setCipherKey(const void *data, int size)
{
#ifdef SQLITE_HAS_CODEC
    int rc = sqlite3_key(m_handle, data, size);
    if (rc == SQLITE_OK) {
        return true;
    }
    setupAndReportError(m_error);
    return false;
#else  //SQLITE_HAS_CODEC
    Error::Abort("[sqlite3_key] is not supported for current config");
    return false;
#endif //SQLITE_HAS_CODEC
}

bool Handle::isStatementReadonly()
{
    return sqlite3_stmt_readonly(m_stmt);
}

bool Handle::isPrepared()
{
    return m_stmt != nullptr;
}

void Handle::setCommittedHook(const CommittedCallback &onCommitted,
                                   void *info)
{
    m_committedHookInfo.onCommitted = onCommitted;
    m_committedHookInfo.info = info;
    m_committedHookInfo.handle = this;
    if (m_committedHookInfo.onCommitted) {
        sqlite3_wal_hook(
            m_handle,
            [](void *p, sqlite3 *, const char *, int pages) -> int {
                CommittedHookInfo *committedHookInfo = (CommittedHookInfo *) p;
                committedHookInfo->onCommitted(committedHookInfo->handle, pages,
                                               committedHookInfo->info);
                return SQLITE_OK;
            },
            &m_committedHookInfo);
    } else {
        sqlite3_wal_hook(m_handle, nullptr, nullptr);
    }
}

bool Handle::backup(const void *key, unsigned int length)
{
    std::string backupPath = getBackupPath();
    int rc = sqliterk_save_master(m_handle, backupPath.c_str(), key, length);
    if (rc == SQLITERK_OK) {
        return true;
    }
    m_error.clear();
    m_error.setError(WCDB::Error::Type::RepairKit, rc);
    m_error.setOperation(Error::Operation::RepairKitBackup);
    m_error.setPath(backupPath);
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

        rc = sqliterk_output(rk, m_handle, info, SQLITERK_OUTPUT_ALL_TABLES);
    } while (false);
    if (rc == SQLITERK_OK) {
        return true;
    }
    m_error.clear();
    m_error.setError(WCDB::Error::Type::RepairKit, rc);
    m_error.setOperation(Error::Operation::RepairKitRepair);
    m_error.setPath(corruptedDBPath);
    return false;
}

int Handle::getChanges()
{
    return sqlite3_changes(m_handle);
}

bool Handle::isReadonly()
{
    return sqlite3_db_readonly(m_handle, NULL) == 1;
}

void Handle::resetError()
{
    m_error.reset();
}

void Handle::skipError(bool skip)
{
    m_skipError = skip;
}

void Handle::setupAndReportError(Error &error)
{
    if (!m_skipError) {
        setupError(error);
        error.report();
    }
}

void Handle::setupAndReportErrorWithSQL(Error &error, const std::string &sql)
{
    if (!m_skipError) {
        setupError(error);
        error.setSQL(sql);
        error.report();
    }
}

void Handle::setupAndReportErrorWithPath(Error &error, const std::string &path)
{
    if (!m_skipError) {
        setupError(error);
        error.setPath(path);
        error.report();
    }
}

void Handle::setupError(Error &error)
{
    error.clear();
    error.setError(WCDB::Error::Type::SQLite, sqlite3_errcode(m_handle));
    if (!error.isOK()) {
        const char *message = sqlite3_errmsg(m_handle);
        if (message) {
            error.setMessage(message);
        }
        error.setExtendedCode(sqlite3_extended_errcode(m_handle));
    }
}

bool Handle::isInTransaction()
{
    return sqlite3_get_autocommit(m_handle) == 0;
}

const Error &Handle::getError()
{
    return m_error;
}

} //namespace WCDB
