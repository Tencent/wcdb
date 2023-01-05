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

#include "SQLiteRepairKit.h"
#include <assert.h>
#include <errno.h>
#include <map>
#ifdef WCDB_BUILTIN_SQLCIPHER
#include <sqlcipher/sqlite3.h>
#else  //WCDB_BUILTIN_SQLCIPHER
#include <sqlite3.h>
#endif //WCDB_BUILTIN_SQLCIPHER
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <strings.h>
#include <vector>
#include <zlib.h>
#if defined(__APPLE__)
#include <CommonCrypto/CommonCrypto.h>
#include <Security/Security.h>
#else
#include <openssl/rc4.h>
#endif

extern "C" {
#include "sqliterk_btree.h"
#include "sqliterk_os.h"
}

class CipherContext {
public:
    enum Op {
        Encrypt,
        Decrypt,
    };
    CipherContext(Op op)
#if defined(__APPLE__)
    : m_op(op), m_key(nullptr), m_keyLength(0), m_cryptor(nullptr)
#endif
    {
    }

    void setKey(const void *key, unsigned int keyLength)
    {
#if defined(__APPLE__)
        m_keyLength = keyLength;
        m_key = (unsigned char *) realloc(m_key, m_keyLength);
        memcpy(m_key, key, m_keyLength);
#else
        RC4_set_key(&m_rc4Key, keyLength, (const unsigned char *) key);
#endif
    }

    void cipher(unsigned char *data, unsigned int length)
    {
#if defined(__APPLE__)
        if (!m_cryptor) {
            CCCryptorCreate(kCCEncrypt, kCCAlgorithmRC4, 0, m_key, m_keyLength,
                            nullptr, &m_cryptor);
        }

        size_t cryptBytes = 0;
        CCCryptorUpdate(m_cryptor, data, length, data, length, &cryptBytes);
        CCCryptorFinal(m_cryptor, data, length, &cryptBytes);
#else
        RC4(&m_rc4Key, length, data, data);
#endif
    }

    ~CipherContext()
    {
#if defined(__APPLE__)
        if (m_cryptor) {
            CCCryptorRelease(m_cryptor);
        }
        if (m_key) {
            free(m_key);
        }
#endif
    }

protected:
#if defined(__APPLE__)
    unsigned char *m_key;
    unsigned int m_keyLength;
    Op m_op;
    CCCryptorRef m_cryptor;
#else
    RC4_KEY m_rc4Key;
#endif
};

struct sqliterk_master_entity {
    sqliterk_type type;
    std::string sql;
    int root_page;

    sqliterk_master_entity() {}

    sqliterk_master_entity(sqliterk_type type_,
                           const char *sql_,
                           int root_page_)
            : type(type_), sql(sql_), root_page(root_page_)
    {
    }
};
typedef std::map<std::string, sqliterk_master_entity> sqliterk_master_map;
struct sqliterk_master_info : public sqliterk_master_map {
};

struct sqliterk_output_column_info {
    char affinity;
    bool not_null;
    sqlite3_value *default_value = NULL;

    sqliterk_output_column_info(char aff, bool nn, sqlite3_value *dflt) :
            affinity(aff), not_null(nn), default_value(dflt) {}

    sqliterk_output_column_info(sqliterk_output_column_info&& other) :
            affinity(other.affinity), not_null(other.not_null), default_value(other.default_value) {
        other.default_value = NULL;
    }

    ~sqliterk_output_column_info() {
        if (default_value) {
            sqlite3_value_free(default_value);
        }
    }
};

struct sqliterk_output_ctx {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int real_columns;
    std::vector<sqliterk_output_column_info> column_info;
    int ipk_column;

    sqliterk_master_map tables;
    sqliterk_master_map::const_iterator table_cursor;
    unsigned int flags;

    unsigned int success_count;
    unsigned int fail_count;
    volatile unsigned cancelled;

    int (*callback)(void *user,
                    sqliterk *rk,
                    sqliterk_table *table,
                    sqliterk_column *column);
    void *user;
};

static void dummyParseTableCallback(sqliterk *rk, sqliterk_table *table)
{
}

static int master_onParseColumn(sqliterk *rk,
                                sqliterk_table *table,
                                sqliterk_column *column)
{
    sqliterk_output_ctx *ctx =
            (sqliterk_output_ctx *) sqliterk_get_user_info(rk);

    if (ctx->cancelled)
        return SQLITERK_CANCELLED;

    // For master table, check whether we should ignore, or create table
    // and prepare for insertion.
    if (sqliterk_table_type(table) != sqliterk_type_master)
        return SQLITERK_MISUSE;

    const char *typestr = sqliterk_column_text(column, 0);
    const char *name = sqliterk_column_text(column, 1);
    const char *tbl_name = sqliterk_column_text(column, 2);
    int root_page = sqliterk_column_integer(column, 3);
    const char *sql = sqliterk_column_text(column, 4);
    sqliterk_type type;

    if (!typestr || !name || !sql || root_page <= 0)
        return SQLITERK_OK;

    if (strcmp(typestr, "table") == 0)
        type = sqliterk_type_table;
    else if (strcmp(typestr, "index") == 0)
        type = sqliterk_type_index;
    else
        return SQLITERK_OK;

    // TODO: deal with system tables.
    if (strncmp(name, "sqlite_", 7) == 0)
        return SQLITERK_OK;

    // Skip table if we are not interested.
    if (!(ctx->flags & SQLITERK_OUTPUT_ALL_TABLES)) {
        sqliterk_master_map::iterator it = ctx->tables.find(tbl_name);
        if (it == ctx->tables.end())
            return SQLITERK_OK;
    }

    // Check CREATE statement if requested.
    if (ctx->flags & SQLITERK_OUTPUT_CHECK_TABLE_COLUMNS) {
        sqliterk_master_map::iterator it = ctx->tables.find(name);
        if (it != ctx->tables.end()) {
            const sqliterk_master_entity &e = it->second;
            if (e.root_page > 0 && !e.sql.empty() &&
                (e.type == sqliterk_type_table || e.type == sqliterk_type_index)) {
                if (e.sql != sql) {
                    sqliterkOSWarning(SQLITERK_DAMAGED, "SQL mismatch: '%s' <-> '%s'", sql, e.sql.c_str());
                    if (strlen(sql) < e.sql.size()) {
                        return SQLITERK_OK;
                    }
                }
            }
        }
    }

    ctx->tables[name] = sqliterk_master_entity(type, sql, root_page);
    return SQLITERK_OK;
}

static void fini_insert(sqliterk_output_ctx *ctx)
{
    if (ctx->stmt) {
        sqlite3_finalize(ctx->stmt);
        ctx->stmt = NULL;
    }

    ctx->column_info.clear();
    ctx->real_columns = 0;
    ctx->ipk_column = 0;
}

static sqlite3_value *eval_value(sqlite3 *db, const char *value)
{
    if (!value || !*value)
        return NULL;

    std::string sql("SELECT ");
    sql += value;
    sql += ';';
    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK || !stmt) {
        return NULL;
    }
    rc = sqlite3_step(stmt);
    sqlite3_value *result = NULL;
    if (rc == SQLITE_ROW) {
        result = sqlite3_column_value(stmt, 0);
        result = sqlite3_value_dup(result);
    }
    sqlite3_finalize(stmt);
    return result;
}

static constexpr char SQLITE_AFF_BLOB = 'A';
static constexpr char SQLITE_AFF_TEXT = 'B';
static constexpr char SQLITE_AFF_NUMERIC = 'C';
static constexpr char SQLITE_AFF_INTEGER = 'D';
static constexpr char SQLITE_AFF_REAL = 'E';

static char parse_affinity(const char *type)
{
    if (!type || !*type) {
        return SQLITE_AFF_BLOB;
    }

    uint32_t h = 0;
    char aff = SQLITE_AFF_NUMERIC;
    do {
        h = (h << 8) + toupper((*type++) & 0xFF);
        if ((h << 8) == ('I'<<16)+('N'<<8)+'T') {
            aff = SQLITE_AFF_INTEGER;
        } else {
            switch (h) {
                case ('C'<<24)+('H'<<16)+('A'<<8)+'R':/* CHAR */
                case ('C'<<24)+('L'<<16)+('O'<<8)+'B':/* CLOB */
                case ('T'<<24)+('E'<<16)+('X'<<8)+'T':/* TEXT */
                    aff = SQLITE_AFF_TEXT;
                    break;
                case ('B'<<24)+('L'<<16)+('O'<<8)+'B':/* BLOB */
                    if (aff == SQLITE_AFF_NUMERIC || aff == SQLITE_AFF_REAL)
                        aff = SQLITE_AFF_BLOB;
                    break;
                case ('R'<<24)+('E'<<16)+('A'<<8)+'L':/* REAL */
                case ('F'<<24)+('L'<<16)+('O'<<8)+'A':/* FLOA */
                case ('D'<<24)+('O'<<16)+('U'<<8)+'B':/* DOUB */
                    if (aff == SQLITE_AFF_NUMERIC)
                        aff = SQLITE_AFF_REAL;
                    break;
            }
        }
    } while (*type);
    return aff;
}

static int init_insert(sqliterk_output_ctx *ctx, const std::string &table)
{
    std::string sql;
    sqlite3_stmt *table_info_stmt;

    assert(ctx->stmt == NULL && ctx->column_info.empty());

    sql.reserve(512);
    sql = "PRAGMA table_info(";
    sql += table;
    sql += ");";
    int rc =
            sqlite3_prepare_v2(ctx->db, sql.c_str(), -1, &table_info_stmt, NULL);
    if (rc != SQLITE_OK) {
        sqliterkOSWarning(rc, "Failed to prepare SQL: %s [SQL: %s]",
                          sqlite3_errmsg(ctx->db), sql.c_str());
        fini_insert(ctx);
        return -1;
    }

    sql = "REPLACE INTO ";
    sql += table;
    sql += " VALUES(";
    ctx->real_columns = 0;
    int ipk_column = 0;
    while (sqlite3_step(table_info_stmt) == SQLITE_ROW) {
        // Record the real column count defined by the SQL.
        ctx->real_columns++;

        // Evaluate the default value for the rows missing some columns in B-tree.
        const char *expr = (const char *) sqlite3_column_text(table_info_stmt, 4);
        sqlite3_value *default_value = eval_value(ctx->db, expr);

        // Evaluate column affinity for dirty detection.
        expr = (const char *) sqlite3_column_text(table_info_stmt, 2);
        char aff = parse_affinity(expr);

        bool not_null = sqlite3_column_int(table_info_stmt, 3) != 0;
        ctx->column_info.emplace_back(aff, not_null, default_value);

        // determine INTEGER PRIMARY KEY
        if (ipk_column >= 0) {
            int pk_idx = sqlite3_column_int(table_info_stmt, 5);
            if (pk_idx == 1) {
                const char *column_type =
                        (const char *) sqlite3_column_text(table_info_stmt, 2);
                if (strcasecmp(column_type, "INTEGER") == 0)
                    ipk_column = ctx->real_columns;
            } else if (pk_idx != 0) {
                ipk_column = -1;
            }
        }

        sql += "?,";
    }
    rc = sqlite3_finalize(table_info_stmt);
    if (rc != SQLITE_OK || ctx->real_columns == 0) {
        sqliterkOSWarning(
                rc, "Failed to execute SQL: %s [SQL: PRAGMA table_info(%s);]",
                sqlite3_errmsg(ctx->db), table.c_str());
        fini_insert(ctx);
        return -1;
    }

    sql[sql.length() - 1] = ')';
    sql += ';';

    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(ctx->db, sql.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        sqliterkOSWarning(rc, "Failed to prepare SQL: %s [SQL: %s]",
                          sqlite3_errmsg(ctx->db), sql.c_str());
        fini_insert(ctx);
        return -1;
    }
    ctx->stmt = stmt;
    ctx->ipk_column = (ipk_column > 0) ? ipk_column : 0;

    return ctx->real_columns;
}

static void table_onBeginParseTable(sqliterk *rk, sqliterk_table *table)
{
    sqliterk_output_ctx *ctx =
            (sqliterk_output_ctx *) sqliterk_get_user_info(rk);

    sqliterkBtreeSetMeta((sqliterk_btree *) table,
                         ctx->table_cursor->first.c_str(),
                         sqliterk_btree_type_table);
}

static int table_onParseColumn(sqliterk *rk,
                               sqliterk_table *table,
                               sqliterk_column *column)
{
    sqliterk_output_ctx *ctx =
            (sqliterk_output_ctx *) sqliterk_get_user_info(rk);

    if (ctx->cancelled)
        return SQLITERK_CANCELLED;

    int rc;
    if (ctx->callback) {
        rc = ctx->callback(ctx->user, rk, table, column);
        if (rc != SQLITERK_OK) {
            if (rc == SQLITERK_IGNORE)
                rc = SQLITERK_OK;
            return rc;
        }
    }

    int columns = sqliterk_column_count(column);
    sqlite3_stmt *stmt = ctx->stmt;

    if (!stmt) {
        // Invalid table_cursor means failed statement compilation.
        if (ctx->table_cursor == ctx->tables.end()) {
            ctx->fail_count++;
            return SQLITERK_OK;
        }

        rc = init_insert(ctx, ctx->table_cursor->first);
        if (rc <= 0) {
            ctx->table_cursor = ctx->tables.end();
            ctx->fail_count++;
            return SQLITERK_OK;
        }

        // Begin transaction.
        char *errmsg;
        rc = sqlite3_exec(ctx->db, "BEGIN;", NULL, NULL, &errmsg);
        if (errmsg) {
            sqliterkOSWarning(rc, "Failed to begin transaction: %s", errmsg);
            sqlite3_free(errmsg);
        }

        stmt = ctx->stmt;
    }

    int i;

    // Check column count: if row has more columns in B-tree than in SQL statement, it's probably
    // come from wrong table and should consider corrupted.
    if (columns > ctx->real_columns || columns > ctx->column_info.size()) {
        sqliterkOSWarning(SQLITERK_DAMAGED, "Column count mismatch: %d vs %d", columns, ctx->real_columns);
        ctx->fail_count++;
        return SQLITERK_OK;
    }

    // Bind values provided by the repair kit.
    sqlite3_clear_bindings(stmt);
    for (i = 0; i < columns; i++) {
        sqliterk_value_type type = sqliterk_column_type(column, i);
        switch (type) {
            case sqliterk_value_type_binary:
                sqlite3_bind_blob(stmt, i + 1,
                                  sqliterk_column_binary(column, i),
                                  sqliterk_column_bytes(column, i), NULL);
                break;
            case sqliterk_value_type_integer:
                // INTEGER value should not be stored in columns with TEXT affinity.
                if (ctx->column_info[i].affinity == SQLITE_AFF_TEXT) {
                    sqliterkOSWarning(SQLITERK_DAMAGED, "INTEGER value detected in column with TEXT affinity.");
                    ctx->fail_count++;
                    return SQLITERK_OK;
                }

                sqlite3_bind_int64(stmt, i + 1,
                                   sqliterk_column_integer64(column, i));
                break;
            case sqliterk_value_type_null:
                // If NOT NULL is defined in this column, the recovered row is considered corrupted.
                if (ctx->column_info[i].not_null) {
                    sqliterkOSWarning(SQLITERK_DAMAGED, "NULL value detected in NOT NULL column.");
                    ctx->fail_count++;
                    return SQLITERK_OK;
                }

                // If it's INTEGER PRIMARY KEY column, bind rowid instead.
                if (ctx->ipk_column == i + 1)
                    sqlite3_bind_int64(stmt, i + 1,
                                       sqliterk_column_rowid(column));
                else
                    sqlite3_bind_null(stmt, i + 1);
                break;
            case sqliterk_value_type_number:
                // REAL value should not be stored in columns with TEXT affinity.
                if (ctx->column_info[i].affinity == SQLITE_AFF_TEXT) {
                    sqliterkOSWarning(SQLITERK_DAMAGED, "REAL value detected in column with TEXT affinity.");
                    ctx->fail_count++;
                    return SQLITERK_OK;
                }

                sqlite3_bind_double(stmt, i + 1,
                                    sqliterk_column_number(column, i));
                break;
            case sqliterk_value_type_text:
                sqlite3_bind_text(stmt, i + 1, sqliterk_column_text(column, i),
                                  sqliterk_column_bytes(column, i), NULL);
                break;
        }
    }

    // Use defaults for remaining values.
    for (; i < ctx->real_columns; i++) {
        sqlite3_value *v = ctx->column_info[i].default_value;
        if (v) {
            sqlite3_bind_value(stmt, i, v);
        }
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
    }
    rc = sqlite3_reset(stmt);
    if (rc != SQLITE_OK) {
        sqliterkOSWarning(rc, "Failed to execute SQL: %s [SQL: %s]",
                          sqlite3_errmsg(ctx->db), sqlite3_sql(stmt));
        ctx->fail_count++;
        return SQLITERK_OK;
    }

    ctx->success_count++;
    if (ctx->success_count % 256 == 0) {
        char *errmsg;
        rc = sqlite3_exec(ctx->db, "COMMIT; BEGIN;", NULL, NULL, &errmsg);
        if (errmsg) {
            sqliterkOSWarning(rc, "Failed to commit transaction: %s", errmsg);
            sqlite3_free(errmsg);
        }
    }

    return SQLITERK_OK;
}

int sqliterk_output(sqliterk *rk,
                    sqlite3 *db,
                    sqliterk_master_info *master_,
                    unsigned int flags)
{
    return sqliterk_output_cb(rk, db, master_, flags, NULL, NULL);
}

int sqliterk_output_cb(sqliterk *rk,
                       sqlite3 *db,
                       sqliterk_master_info *master_,
                       unsigned int flags,
                       int (*callback)(void *user,
                                       sqliterk *rk,
                                       sqliterk_table *table,
                                       sqliterk_column *column),
                       void *user)
{
    if (!rk || !db)
        return SQLITERK_MISUSE;

    sqliterk_master_map *master = static_cast<sqliterk_master_map *>(master_);
    sqliterk_output_ctx ctx;
    ctx.db = db;
    ctx.stmt = NULL;
    ctx.flags = flags;
    ctx.success_count = 0;
    ctx.fail_count = 0;
    ctx.ipk_column = 0;
    ctx.callback = callback;
    ctx.user = user;
    ctx.cancelled = 0;

    if (!master)
        ctx.flags |= SQLITERK_OUTPUT_ALL_TABLES;
    else
        ctx.tables = *master;

    sqliterk_set_user_info(rk, &ctx);
    sqliterk_notify notify;
    notify.onBeginParseTable = dummyParseTableCallback;
    notify.onEndParseTable = dummyParseTableCallback;
    notify.onParseColumn = master_onParseColumn;
    notify.didParsePage = NULL;
    sqliterk_register_notify(rk, notify);
    sqliterk_set_recursive(rk, 0);

    const char *db_file = sqlite3_db_filename(db, "main");
    sqliterkOSInfo(SQLITERK_OK, "Output recovered data to '%s', flags 0x%04x",
                   db_file ? db_file : "unknown", flags);

    // Parse sqlite_master for table info.
    sqliterkOSDebug(SQLITERK_OK, "Begin parsing sqlite_master...");
    int rc = sqliterk_parse_page(rk, 1);
    if (rc == SQLITERK_CANCELLED) {
        goto cancelled;
    } else if (rc != SQLITERK_OK)
        sqliterkOSWarning(rc, "Failed to parse sqlite_master.");
    else
        sqliterkOSInfo(rc, "Parsed sqlite_master. [table/index: %zu]",
                       ctx.tables.size());

    // Parse all tables.
    notify.onBeginParseTable = table_onBeginParseTable;
    notify.onParseColumn = table_onParseColumn;
    sqliterk_register_notify(rk, notify);

    for (sqliterk_master_map::iterator it = ctx.tables.begin();
         it != ctx.tables.end(); ++it) {
        if (ctx.cancelled)
            goto cancelled;

        if (it->second.type != sqliterk_type_table)
            continue;

        // Run CREATE TABLE statements if necessary.
        if (!(ctx.flags & SQLITERK_OUTPUT_NO_CREATE_TABLES) &&
            !it->second.sql.empty()) {
            sqliterkOSInfo(SQLITERK_OK, ">>> %s", it->second.sql.c_str());
            char *errmsg = NULL;
            const char *sql = it->second.sql.c_str();
            rc = sqlite3_exec(ctx.db, sql, NULL, NULL, &errmsg);
            if (errmsg) {
                sqliterkOSWarning(rc, "EXEC FAILED: %s [SQL: %s]", errmsg, sql);
                ctx.fail_count++;
                sqlite3_free(errmsg);
            } else
                ctx.success_count++;
        }

        if (it->second.root_page != 0) {
            const char *name = it->first.c_str();
            int root_page = it->second.root_page;
            sqliterkOSInfo(SQLITERK_OK, "[%s] -> pgno: %d", name, root_page);
            ctx.table_cursor = it;
            rc = sqliterk_parse_page(rk, root_page);
            if (ctx.stmt) {
                const char *sql =
                        (rc == SQLITERK_CANCELLED) ? "ROLLBACK;" : "COMMIT;";

                // Commit transaction and free statement.
                char *errmsg;
                int rc2 = sqlite3_exec(ctx.db, sql, NULL, NULL, &errmsg);
                if (errmsg) {
                    sqliterkOSWarning(rc2, "Failed to commit transaction: %s",
                                      errmsg);
                    sqlite3_free(errmsg);
                }

                fini_insert(&ctx);
            }
            if (rc == SQLITERK_CANCELLED) {
                goto cancelled;
            } else if (rc != SQLITERK_OK) {
                sqliterkOSWarning(rc,
                                  "Failed to parse B-tree with root page %d.",
                                  it->second.root_page);
            }
        }
    }

    // Iterate through indices, create them if necessary.
    if (!(ctx.flags & SQLITERK_OUTPUT_NO_CREATE_TABLES)) {
        for (sqliterk_master_map::iterator it = ctx.tables.begin();
             it != ctx.tables.end(); ++it) {
            if (ctx.cancelled)
                goto cancelled;

            if (it->second.type != sqliterk_type_index)
                continue;

            const char *sql = it->second.sql.c_str();
            sqliterkOSDebug(SQLITERK_OK, ">>> %s", sql);
            char *errmsg = NULL;
            rc = sqlite3_exec(ctx.db, sql, NULL, NULL, &errmsg);
            if (errmsg) {
                sqliterkOSWarning(rc, "EXEC FAILED: %s [SQL: %s]", errmsg, sql);
                ctx.fail_count++;
                sqlite3_free(errmsg);
            } else
                ctx.success_count++;
        }
    }

    // Return OK only if we had successfully output at least one row.
    if (ctx.success_count == 0) {
        if (ctx.tables.empty())
            sqliterkOSError(SQLITERK_DAMAGED,
                            "No valid sqlite_master info available, "
                            "sqlite_master is corrupted.");
        else
            sqliterkOSError(SQLITERK_DAMAGED,
                            "No rows can be successfully output. [failed: %u]",
                            ctx.fail_count);
        return SQLITERK_DAMAGED;
    } else {
        sqliterkOSInfo(SQLITERK_OK,
                       "Recovery output finished. [succeeded: %u, failed: %u]",
                       ctx.success_count, ctx.fail_count);
        return SQLITERK_OK;
    }

    cancelled:
    sqliterkOSInfo(SQLITERK_CANCELLED,
                   "Recovery cancelled. [succeeded: %u, failed: %u]",
                   ctx.success_count, ctx.fail_count);
    return SQLITERK_CANCELLED;
}

void sqliterk_cancel(sqliterk *rk)
{
    sqliterk_output_ctx *ctx =
            (sqliterk_output_ctx *) sqliterk_get_user_info(rk);
    if (ctx) {
        ctx->cancelled = 1;
    }
}

int sqliterk_make_master(const char **tables,
                         int num_tables,
                         sqliterk_master_info **out_master)
{
    if (!tables || !num_tables) {
        *out_master = NULL;
        return SQLITERK_OK;
    }

    sqliterk_master_map *master = new sqliterk_master_map();
    for (int i = 0; i < num_tables; i++)
        (*master)[tables[i]] =
                sqliterk_master_entity(sqliterk_type_unknown, "", 0);

    *out_master = static_cast<sqliterk_master_info *>(master);
    return SQLITERK_OK;
}

#define SQLITERK_SM_TYPE_TABLE 0x01
#define SQLITERK_SM_TYPE_INDEX 0x02

#define SQLITERK_SM_MAGIC "\0dBmSt"
#define SQLITERK_SM_VERSION 1

#pragma pack(push, 1)
struct master_file_header {
    char magic[6];
    uint16_t version;
    uint32_t entities;
    unsigned char kdf_salt[16];
};

struct master_file_entity {
    uint32_t root_page;
    uint8_t type;
    uint8_t name_len;
    uint8_t tbl_name_len;
    uint8_t reserved;
    uint16_t sql_len;

    unsigned char data[0];
};
#pragma pack(pop)

int sqliterk_save_master(sqlite3 *db,
                         const char *path,
                         const void *key,
                         int key_len)
{
    FILE *fp = NULL;
    int rc = SQLITERK_OK;
    sqlite3_stmt *stmt = NULL;
    z_stream zstrm = {0};
    CipherContext cipherContext(CipherContext::Encrypt);
    unsigned char in_buf[512 + 8];
    unsigned char out_buf[2048];
    uint32_t entities = 0;
    master_file_header header;

    // Prepare deflate stream.
    rc = deflateInit(&zstrm, Z_DEFAULT_COMPRESSION);
    if (rc != Z_OK)
        goto bail_zlib;
    zstrm.data_type = Z_TEXT;

    // Open output file.
    fp = fopen(path, "wb");
    if (!fp)
        goto bail_errno;

    // Prepare cipher key.
    if (key && key_len) {
        cipherContext.setKey(key, key_len);
    } else {
        key = NULL;
    }

    // Prepare SQL statement.
    rc =
            sqlite3_prepare_v2(db, "SELECT * FROM sqlite_master;", -1, &stmt, NULL);
    if (rc != SQLITE_OK)
        goto bail_sqlite;

    // Write dummy header.
    memset(&header, 0, sizeof(header));
    if (fwrite(&header, sizeof(header), 1, fp) != 1)
        goto bail_errno;

    // Read all rows.
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const char *typestr = (const char *) sqlite3_column_text(stmt, 0);
        int name_len = sqlite3_column_bytes(stmt, 1);
        const char *name = (const char *) sqlite3_column_text(stmt, 1);
        int tbl_name_len = sqlite3_column_bytes(stmt, 2);
        const char *tbl_name = (const char *) sqlite3_column_text(stmt, 2);
        int root_page = sqlite3_column_int(stmt, 3);
        int sql_len = sqlite3_column_bytes(stmt, 4);
        const char *sql = (const char *) sqlite3_column_text(stmt, 4);

        // Skip invalid rows.
        if (!typestr || !name || !tbl_name || !sql)
            continue;

        // Skip system tables and indices.
        if (strncmp(name, "sqlite_", 7) == 0)
            continue;

        unsigned char type;
        if (strcmp(typestr, "table") == 0)
            type = SQLITERK_SM_TYPE_TABLE;
        else if (strcmp(typestr, "index") == 0)
            type = SQLITERK_SM_TYPE_INDEX;
        else
            continue;

        if (name_len > 255 || tbl_name_len > 255 || sql_len > 65535) {
            sqliterkOSError(SQLITERK_IOERR,
                            "Table/index has name longer than 255: %s, %s",
                            name, tbl_name);
            goto bail;
        }

        // Write to zip-stream buffer.
        master_file_entity *entity = (master_file_entity *) in_buf;
        entity->root_page = root_page;
        entity->type = type;
        entity->name_len = (unsigned char) name_len;
        entity->tbl_name_len = (unsigned char) tbl_name_len;
        entity->reserved = 0;
        entity->sql_len = (unsigned short) sql_len;
        unsigned char *p_data = entity->data;
        memcpy(p_data, name, name_len + 1);
        p_data += name_len + 1;
        memcpy(p_data, tbl_name, tbl_name_len + 1);
        p_data += tbl_name_len + 1;

        zstrm.next_in = in_buf;
        zstrm.avail_in = (uInt)(p_data - in_buf);

        do {
            zstrm.next_out = out_buf;
            zstrm.avail_out = sizeof(out_buf);
            rc = deflate(&zstrm, Z_NO_FLUSH);
            if (rc == Z_STREAM_ERROR)
                goto bail_zlib;

            unsigned have = sizeof(out_buf) - zstrm.avail_out;
            if (key) {
                cipherContext.cipher(out_buf, have);
            }
            if (fwrite(out_buf, 1, have, fp) != have) {
                sqliterkOSError(SQLITERK_IOERR,
                                "Cannot write to backup file: %s",
                                strerror(errno));
                goto bail;
            }
        } while (zstrm.avail_out == 0);

        zstrm.next_in = (unsigned char *) sql;
        zstrm.avail_in = sql_len + 1;

        do {
            zstrm.next_out = out_buf;
            zstrm.avail_out = sizeof(out_buf);
            rc = deflate(&zstrm, Z_NO_FLUSH);
            if (rc == Z_STREAM_ERROR)
                goto bail_zlib;

            unsigned have = sizeof(out_buf) - zstrm.avail_out;
            if (key) {
                cipherContext.cipher(out_buf, have);
            }
            if (fwrite(out_buf, 1, have, fp) != have) {
                sqliterkOSError(SQLITERK_IOERR,
                                "Cannot write to backup file: %s",
                                strerror(errno));
                goto bail;
            }
        } while (zstrm.avail_out == 0);

        entities++;
    } // sqlite3_step
    rc = sqlite3_finalize(stmt);
    stmt = NULL;
    if (rc != SQLITE_OK)
        goto bail_sqlite;

    // Flush Z-stream.
    zstrm.avail_in = 0;
    do {
        zstrm.next_out = out_buf;
        zstrm.avail_out = sizeof(out_buf);
        rc = deflate(&zstrm, Z_FINISH);
        if (rc == Z_STREAM_ERROR)
            goto bail_zlib;

        unsigned have = sizeof(out_buf) - zstrm.avail_out;
        if (key) {
            cipherContext.cipher(out_buf, have);
        }
        if (fwrite(out_buf, 1, have, fp) != have) {
            sqliterkOSError(SQLITERK_IOERR, "Cannot write to backup file: %s",
                            strerror(errno));
            goto bail;
        }
    } while (zstrm.avail_out == 0);
    deflateEnd(&zstrm);

    // Read KDF salt from file header.
    sqlite3_file *db_file;
    rc = sqlite3_file_control(db, "main", SQLITE_FCNTL_FILE_POINTER, &db_file);
    if (rc != SQLITE_OK)
        goto bail_sqlite;

    rc = db_file->pMethods->xRead(db_file, header.kdf_salt,
                                  sizeof(header.kdf_salt), 0);
    if (rc != SQLITE_OK)
        goto bail_sqlite;

    // Write real header.
    memcpy(header.magic, SQLITERK_SM_MAGIC, sizeof(header.magic));
    header.version = SQLITERK_SM_VERSION;
    header.entities = entities;
    fseek(fp, 0, SEEK_SET);
    fwrite(&header, sizeof(header), 1, fp);
    fclose(fp);

    sqliterkOSInfo(SQLITERK_OK, "Saved master info with %u entries.", entities);
    return SQLITERK_OK;

    bail_zlib:
    sqliterkOSError(SQLITERK_CANTOPEN, "Failed to backup master table: %s",
                    zstrm.msg);
    goto bail;
    bail_errno:
    sqliterkOSError(rc, "Failed to backup master table: %s", strerror(errno));
    goto bail;
    bail_sqlite:
    sqliterkOSError(rc, "Failed to backup master table: %s",
                    sqlite3_errmsg(db));

    bail:
    if (fp)
        fclose(fp);
    if (stmt)
        sqlite3_finalize(stmt);
    deflateEnd(&zstrm);
    return rc;
}

static const size_t IN_BUF_SIZE = 4096;
static int inflate_read(FILE *fp,
                        z_streamp strm,
                        void *buf,
                        unsigned size,
                        CipherContext *cipherContext)
{
    int ret;
    if (size == 0)
        return SQLITERK_OK;

    strm->next_out = (unsigned char *) buf;
    strm->avail_out = size;

    do {
        if (strm->avail_in == 0 && !feof(fp)) {
            unsigned char *in_buf = strm->next_in - strm->total_in;
            strm->total_in = 0;

            ret = (int) fread(in_buf, 1, IN_BUF_SIZE, fp);
            if (ret == 0 && ferror(fp))
                return SQLITERK_IOERR;
            if (ret > 0) {
                if (cipherContext) {
                    cipherContext->cipher(in_buf, ret);
                }
                strm->next_in = in_buf;
                strm->avail_in = ret;
            }
        }

        ret = inflate(strm, Z_NO_FLUSH);
        if (ret != Z_OK && ret != Z_STREAM_END)
            return SQLITERK_DAMAGED;

    } while (strm->avail_out > 0 && ret != Z_STREAM_END);

    return strm->avail_out ? SQLITERK_DAMAGED : SQLITERK_OK;
}

static int pstrcmp(const void *p1, const void *p2)
{
    return strcmp(*(char *const *) p1, *(char *const *) p2);
}

int sqliterk_load_master(const char *path,
                         const void *key,
                         int key_len,
                         const char **tables,
                         int num_tables,
                         sqliterk_master_info **out_master,
                         unsigned char *out_kdf_salt)
{
    FILE *fp = NULL;
    z_stream zstrm = {0};
    CipherContext cipherContext(CipherContext::Decrypt);
    int rc;
    unsigned char in_buf[IN_BUF_SIZE];
    const char **filter = NULL;
    sqliterk_master_map *master = NULL;
    uint32_t entities;

    // Allocate output buffer.
    char *str_buf = (char *) malloc(65536 + 512);
    if (!str_buf)
        goto bail_errno;

    // Sort table filter for later binary searching.
    if (tables && num_tables) {
        sqliterk_make_master(tables, num_tables,
                             (sqliterk_master_info **) &master);

        size_t filter_size = num_tables * sizeof(const char *);
        filter = (const char **) malloc(filter_size);
        if (!filter)
            goto bail_errno;

        memcpy(filter, tables, filter_size);
        qsort(filter, num_tables, sizeof(const char *), pstrcmp);
    }
    if (!master)
        master = new sqliterk_master_map();

    fp = fopen(path, "rb");
    if (!fp)
        goto bail_errno;

    // Read header.
    master_file_header header;
    if (fread(&header, sizeof(header), 1, fp) != 1)
        goto bail_errno;
    if (memcmp(header.magic, SQLITERK_SM_MAGIC, sizeof(header.magic)) != 0 ||
        header.version != SQLITERK_SM_VERSION) {
        sqliterkOSError(SQLITERK_DAMAGED, "Invalid format: %s", path);
        goto bail;
    }

    // Initialize zlib and RC4.
    rc = inflateInit(&zstrm);
    if (rc != Z_OK)
        goto bail_zlib;

    if (key && key_len) {
        cipherContext.setKey(key, key_len);
    } else {
        key = NULL;
    }

    // Read all entities.
    entities = header.entities;
    zstrm.next_in = in_buf;
    zstrm.avail_in = 0;
    while (entities--) {
        // Read entity header.
        master_file_entity entity;
        rc = inflate_read(fp, &zstrm, &entity, sizeof(entity),
                          key ? &cipherContext : NULL);
        if (rc == SQLITERK_IOERR)
            goto bail_errno;
        else if (rc == SQLITERK_DAMAGED)
            goto bail_zlib;

        // Read names and SQL.
        rc = inflate_read(fp, &zstrm, str_buf,
                          entity.name_len + entity.tbl_name_len +
                          entity.sql_len + 3,
                          key ? &cipherContext : NULL);
        if (rc == SQLITERK_IOERR)
            goto bail_errno;
        else if (rc == SQLITERK_DAMAGED)
            goto bail_zlib;

        const char *name = str_buf;
        const char *tbl_name = name + entity.name_len + 1;
        const char *sql = tbl_name + entity.tbl_name_len + 1;
        if (name[entity.name_len] != '\0' ||
            tbl_name[entity.tbl_name_len] != '\0' ||
            sql[entity.sql_len] != '\0') {
            sqliterkOSError(SQLITERK_DAMAGED,
                            "Invalid string. File corrupted.");
            goto bail;
        }

        // Filter tables.
        if (!filter || bsearch(&tbl_name, filter, num_tables,
                               sizeof(const char *), pstrcmp)) {
            sqliterk_master_entity e(sqliterk_type_unknown, sql,
                                     entity.root_page);
            if (entity.type == SQLITERK_SM_TYPE_TABLE)
                e.type = sqliterk_type_table;
            else if (entity.type == SQLITERK_SM_TYPE_INDEX)
                e.type = sqliterk_type_index;

            (*master)[name] = e;
        }
    }

    inflateEnd(&zstrm);
    free(str_buf);
    free(filter);
    fclose(fp);

    if (out_kdf_salt)
        memcpy(out_kdf_salt, header.kdf_salt, sizeof(header.kdf_salt));
    *out_master = static_cast<sqliterk_master_info *>(master);
    sqliterkOSInfo(SQLITERK_OK, "Loaded master info with %zu valid entries.",
                   master->size());
    return SQLITERK_OK;

    bail_errno:
    sqliterkOSError(SQLITERK_IOERR, "Cannot load master table: %s",
                    strerror(errno));
    goto bail;
    bail_zlib:
    sqliterkOSError(SQLITERK_DAMAGED, "Cannot load master table: %s",
                    zstrm.msg);
    bail:
    if (master)
        delete master;
    free(str_buf);
    free(filter);
    inflateEnd(&zstrm);
    if (fp)
        fclose(fp);
    return SQLITERK_DAMAGED;
}

void sqliterk_free_master(sqliterk_master_info *master)
{
    if (master)
        delete static_cast<sqliterk_master_map *>(master);
}
