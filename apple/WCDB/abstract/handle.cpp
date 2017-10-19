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
#include <WCDB/Path.hpp>
#include <WCDB/SQLiteRepairKit.h>
#include <WCDB/handle.hpp>
#include <WCDB/handle_statement.hpp>
#include <WCDB/macro.hpp>
#include <WCDB/statement.hpp>
#include <WCDB/statement_transaction.hpp>
#include <sqlcipher/sqlite3.h>

namespace WCDB {

const std::string Handle::backupSuffix("-backup");

static void GlobalLog(void *userInfo, int code, const char *message)
{
    Error::ReportSQLiteGlobal(code, message ? message : "", nullptr);
}

const auto UNUSED_UNIQUE_ID = []() {
    sqlite3_config(SQLITE_CONFIG_LOG, GlobalLog, nullptr);
    sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
    sqlite3_config(SQLITE_CONFIG_MEMSTATUS, false);
    //    sqlite3_config(SQLITE_CONFIG_MMAP_SIZE, 0x7fff0000, 0x7fff0000);
    return nullptr;
}();

Handle::Handle(const std::string &p)
    : m_handle(nullptr)
    , m_tag(InvalidTag)
    , path(p)
    , m_performanceTrace(nullptr)
    , m_sqlTrace(nullptr)
    , m_cost(0)
    , m_aggregation(false)
{
}

Handle::~Handle()
{
    reportPerformance();
    close();
}

bool Handle::open()
{
    Error innerError;
    File::createDirectoryWithIntermediateDirectories(Path::getBaseName(path),
                                                     innerError);
    int rc = sqlite3_open(path.c_str(), (sqlite3 **) &m_handle);
    if (rc == SQLITE_OK) {
        m_error.reset();
        return true;
    }
    Error::ReportSQLite(m_tag, path, Error::HandleOperation::Open, rc,
                        sqlite3_errmsg((sqlite3 *) m_handle), &m_error);
    return false;
}

void Handle::close()
{
    int rc = sqlite3_close((sqlite3 *) m_handle);
    if (rc == SQLITE_OK) {
        m_handle = nullptr;
        m_error.reset();
        return;
    }
    Error::ReportSQLite(m_tag, path, Error::HandleOperation::Close, rc,
                        sqlite3_extended_errcode((sqlite3 *) m_handle),
                        sqlite3_errmsg((sqlite3 *) m_handle), &m_error);
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
            (sqlite3 *) m_handle, flag,
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
        sqlite3_trace_v2((sqlite3 *) m_handle, 0, nullptr, nullptr);
    }
}

void Handle::setSQLTrace(const SQLTrace &trace)
{
    m_sqlTrace = trace;
    setupTrace();
}

void Handle::setPerformanceTrace(const PerformanceTrace &trace)
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
        m_performanceTrace(m_tag, m_footprint, m_cost);
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

std::shared_ptr<StatementHandle> Handle::prepare(const Statement &statement)
{
    if (statement.getStatementType() == Statement::Type::Transaction) {
        Error::Abort(
            "[prepare] a transaction is not allowed, use [exec] instead",
            &m_error);
        return nullptr;
    }
    sqlite3_stmt *stmt = nullptr;
    int rc = sqlite3_prepare_v2((sqlite3 *) m_handle,
                                statement.getDescription().c_str(), -1, &stmt,
                                nullptr);
    if (rc == SQLITE_OK) {
        m_error.reset();
        return std::shared_ptr<StatementHandle>(
            new StatementHandle(stmt, *this));
    }
    Error::ReportSQLite(m_tag, path, Error::HandleOperation::Prepare, rc,
                        sqlite3_extended_errcode((sqlite3 *) m_handle),
                        sqlite3_errmsg((sqlite3 *) m_handle),
                        statement.getDescription(), &m_error);
    return nullptr;
}

bool Handle::exec(const Statement &statement)
{
    int rc =
        sqlite3_exec((sqlite3 *) m_handle, statement.getDescription().c_str(),
                     nullptr, nullptr, nullptr);
    bool result = rc == SQLITE_OK;
    if (statement.getStatementType() == Statement::Type::Transaction) {
        const StatementTransaction &transaction =
            (const StatementTransaction &) statement;
        switch (transaction.getTransactionType()) {
            case StatementTransaction::Type::Begin:
                if (result) {
                    m_aggregation = true;
                }
                break;
            case StatementTransaction::Type::Commit:
                if (result) {
                    m_aggregation = false;
                }
                break;
            case StatementTransaction::Type::Rollback:
                m_aggregation = false;
                break;
        }
    }
    if (result) {
        m_error.reset();
        return true;
    }
    Error::ReportSQLite(m_tag, path, Error::HandleOperation::Exec, rc,
                        sqlite3_extended_errcode((sqlite3 *) m_handle),
                        sqlite3_errmsg((sqlite3 *) m_handle),
                        statement.getDescription(), &m_error);
    return false;
}

long long Handle::getLastInsertedRowID()
{
    return sqlite3_last_insert_rowid((sqlite3 *) m_handle);
}

bool Handle::setCipherKey(const void *data, int size)
{
#ifdef SQLITE_HAS_CODEC
    int rc = sqlite3_key((sqlite3 *) m_handle, data, size);
    if (rc == SQLITE_OK) {
        m_error.reset();
        return true;
    }
    Error::ReportSQLite(m_tag, path, Error::HandleOperation::SetCipherKey, rc,
                        sqlite3_extended_errcode((sqlite3 *) m_handle),
                        sqlite3_errmsg((sqlite3 *) m_handle), &m_error);
    return false;
#else  //SQLITE_HAS_CODEC
    Error::ReportSQLite(m_tag, path, Error::HandleOperation::SetCipherKey,
                        SQLITE_MISUSE, SQLITE_MISUSE,
                        "[sqlite3_key] is not supported for current config",
                        &m_error);
    return false;
#endif //SQLITE_HAS_CODEC
}

void Handle::registerCommitedHook(const CommitedHook &onCommited, void *info)
{
    m_commitedHookInfo.onCommited = onCommited;
    m_commitedHookInfo.info = info;
    m_commitedHookInfo.handle = this;
    if (m_commitedHookInfo.onCommited) {
        sqlite3_wal_hook(
            (sqlite3 *) m_handle,
            [](void *p, sqlite3 *, const char *, int pages) -> int {
                CommitedHookInfo *commitedHookInfo = (CommitedHookInfo *) p;
                commitedHookInfo->onCommited(commitedHookInfo->handle, pages,
                                             commitedHookInfo->info);
                return SQLITE_OK;
            },
            &m_commitedHookInfo);
    } else {
        sqlite3_wal_hook((sqlite3 *) m_handle, nullptr, nullptr);
    }
}

std::string Handle::getBackupPath() const
{
    return path + backupSuffix;
}

bool Handle::backup(const void *key, const unsigned int &length)
{
    std::string backupPath = getBackupPath();
    int rc = sqliterk_save_master((sqlite3 *) m_handle, backupPath.c_str(), key,
                                  length);
    if (rc == SQLITERK_OK) {
        m_error.reset();
        return true;
    }
    Error::ReportRepair(path, Error::RepairOperation::SaveMaster, rc, &m_error);
    return false;
}

bool Handle::recoverFromPath(const std::string &corruptedDBPath,
                             const int pageSize,
                             const void *backupKey,
                             const unsigned int &backupKeyLength,
                             const void *databaseKey,
                             const unsigned int &databaseKeyLength)
{
    std::string backupPath = corruptedDBPath + backupSuffix;
    sqliterk_master_info *info;
    unsigned char kdfSalt[16];
    memset(kdfSalt, 0, 16);
    int rc = sqliterk_load_master(backupPath.c_str(), backupKey,
                                  backupKeyLength, nullptr, 0, &info, kdfSalt);
    if (rc != SQLITERK_OK) {
        Error::ReportRepair(
            backupPath, WCDB::Error::RepairOperation::LoadMaster, rc, &m_error);
        return false;
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
        Error::ReportRepair(
            backupPath, WCDB::Error::RepairOperation::LoadMaster, rc, &m_error);
        return false;
    }

    rc = sqliterk_output(rk, (sqlite3 *) m_handle, info,
                         SQLITERK_OUTPUT_ALL_TABLES);
    if (rc != SQLITERK_OK) {
        Error::ReportRepair(corruptedDBPath,
                            WCDB::Error::RepairOperation::Repair, rc, &m_error);
        return false;
    }
    m_error.reset();
    return true;
}

void Handle::setTag(Tag tag)
{
    m_tag = tag;
}

Tag Handle::getTag() const
{
    return m_tag;
}

const Error &Handle::getError() const
{
    return m_error;
}

int Handle::getChanges()
{
    return sqlite3_changes((sqlite3 *) m_handle);
}

bool Handle::isReadonly()
{
    return sqlite3_db_readonly((sqlite3 *) m_handle, NULL) == 1;
}

} //namespace WCDB
