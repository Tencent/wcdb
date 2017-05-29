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

#define MM_BACKUP_INCLUDE_INTERNAL
#include "mm_backup.h"
#include <errno.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <openssl/rc4.h>
#include <zlib.h>

#define INPUT_BUFFER_SIZE 8192
#define OUTPUT_BUFFER_SIZE 8192
struct mm_backup_ctx {
    sqlite3 *db; // database connection, not initialized until actual running.
    z_stream zstrm;  // output compression stream.
    RC4_KEY rc4_key; // output file encryption key.
    FILE *fp;        // output file handle.
    uint32_t flags;  // configuration flags.
    mm_logfunc log;  // log function.
    uint32_t stmt_count;
    uint32_t sec_size;
    uint32_t sec_pos;

    mm_backup_tabdesc *tabdesc;
    int num_tabdesc;

    volatile char canceled; // task status.

    char writable_schema; // whether PRAGMA writable_schema has been set.
    char corrupted;       // whether database is corrupted.

    pthread_t write_thread;
    pthread_mutex_t buf_lock;
    pthread_cond_t buf_cond;
    char in_finish;
    char in_avail;
    char in_full;
    char write_error;

    struct mm_backup_buffer {
        uint_t avail;
        uint8_t data[INPUT_BUFFER_SIZE];
    } * p_buf, buffer[2];

    char errmsg[2048];
};

static void *write_thread_entry(void *ud)
{
    mm_backup_ctx *ctx = (mm_backup_ctx *) ud;

    for (;;) {
        int finish;

        pthread_mutex_lock(&ctx->buf_lock);
        {
            while (!ctx->in_avail && !ctx->in_finish && !ctx->canceled)
                pthread_cond_wait(&ctx->buf_cond, &ctx->buf_lock);

            ctx->zstrm.next_in = ctx->p_buf->data;
            ctx->zstrm.avail_in = ctx->p_buf->avail;
            finish = ctx->in_finish;

            ctx->p_buf = (ctx->p_buf == &ctx->buffer[0]) ? &ctx->buffer[1]
                                                         : &ctx->buffer[0];
            ctx->p_buf->avail = 0;

            ctx->in_avail = 0;
            ctx->in_full = 0;
        }
        pthread_mutex_unlock(&ctx->buf_lock);
        pthread_cond_signal(&ctx->buf_cond);

        int ret;
        uint8_t out_buf[OUTPUT_BUFFER_SIZE];

        if (ctx->canceled)
            return NULL;
        do {
            ctx->zstrm.next_out = out_buf;
            ctx->zstrm.avail_out = sizeof(out_buf);
            ret = deflate(&ctx->zstrm, finish ? Z_FINISH : Z_NO_FLUSH);
            if (ret == Z_BUF_ERROR)
                break;
            if (ret != Z_OK && ret != Z_STREAM_END)
                goto bail;

            uint_t out_len = sizeof(out_buf) - ctx->zstrm.avail_out;
            if ((ctx->flags & MMBAK_FLAG_NO_CIPHER) == 0)
                RC4(&ctx->rc4_key, out_len, out_buf, out_buf);

            const uint8_t *p = out_buf;
            while (out_len > 0) {
                size_t ret = fwrite(p, 1, out_len, ctx->fp);
                if (ret == 0 && ferror(ctx->fp))
                    goto bail;
                out_len -= ret;
                p += ret;
                ctx->sec_size += ret;
            }
        } while (ctx->zstrm.avail_out == 0 && ret != Z_STREAM_END);
        if (ctx->zstrm.avail_in != 0)
            goto bail;

        if (ret == Z_STREAM_END)
            break;
    }
    return NULL;

bail:
    pthread_mutex_lock(&ctx->buf_lock);
    ctx->write_error = 1;
    pthread_cond_signal(&ctx->buf_cond);
    pthread_mutex_unlock(&ctx->buf_lock);
    return NULL;
}

static int dump_write_stream(mm_backup_ctx *ctx, const void *data, uint_t size)
{
    struct mm_backup_buffer *p_buf;
    const uint8_t *p_data = (const uint8_t *) data;

    pthread_mutex_lock(&ctx->buf_lock);
    while (size > 0) {
        while (ctx->in_full && !ctx->canceled && !ctx->write_error)
            pthread_cond_wait(&ctx->buf_cond, &ctx->buf_lock);

        if (ctx->write_error) {
            pthread_mutex_unlock(&ctx->buf_lock);
            LOGE_("Writing thread reported error.");
            return -1;
        }

        p_buf = ctx->p_buf;

        uint_t size_to_write = (p_buf->avail + size > INPUT_BUFFER_SIZE)
                                   ? INPUT_BUFFER_SIZE - p_buf->avail
                                   : size;
        memcpy(p_buf->data + p_buf->avail, p_data, size_to_write);
        size -= size_to_write;
        p_buf->avail += size_to_write;
        p_data += size_to_write;

        ctx->in_avail = p_buf->avail > INPUT_BUFFER_SIZE / 4;
        ctx->in_full = p_buf->avail == INPUT_BUFFER_SIZE;
        if (ctx->in_avail || ctx->in_full)
            pthread_cond_signal(&ctx->buf_cond);
    }
    pthread_mutex_unlock(&ctx->buf_lock);
    return 0;
}

static int dump_write_end(mm_backup_ctx *ctx)
{
    int ret;

    // flush padding input.
    pthread_mutex_lock(&ctx->buf_lock);
    {
        ctx->in_finish = 1;
        if (ctx->write_error) {
            LOGE_("Writing thread reported error.");
            ret = -1;
        } else
            ret = 0;
    }
    pthread_mutex_unlock(&ctx->buf_lock);
    pthread_cond_signal(&ctx->buf_cond);

    return ret;
}

#define dump_write_byte(ctx, v)                                                \
    ({                                                                         \
        int _ret;                                                              \
        uint8_t b1 = (uint8_t)(v);                                             \
        _ret = dump_write_stream((ctx), &b1, 1);                               \
        _ret;                                                                  \
    })

// assume little-endian
#define dump_write_short(ctx, v)                                               \
    ({                                                                         \
        int _ret;                                                              \
        uint16_t s1 = (uint16_t)(v);                                           \
        _ret = dump_write_stream((ctx), &s1, 2);                               \
        _ret;                                                                  \
    })

#define dump_write_byte_byte(ctx, b1, b2)                                      \
    ({                                                                         \
        int _ret;                                                              \
        uint8_t buf[2];                                                        \
        buf[0] = (uint8_t)(b1);                                                \
        buf[1] = (uint8_t)(b2);                                                \
        _ret = dump_write_stream((ctx), buf, 2);                               \
        _ret;                                                                  \
    })

// assume little-endian
#define dump_write_byte_short(ctx, b, s)                                       \
    ({                                                                         \
        int _ret;                                                              \
        uint8_t buf[3];                                                        \
        uint16_t s1 = (uint16_t)(s);                                           \
        buf[0] = (uint8_t)(b);                                                 \
        buf[1] = (uint8_t)(s1 & 0xFF);                                         \
        buf[2] = (uint8_t)((s1 >> 8) & 0xFF);                                  \
        _ret = dump_write_stream((ctx), buf, 3);                               \
        _ret;                                                                  \
    })

static int dump_write_varint(mm_backup_ctx *ctx, int64_t val)
{
    uint8_t buf[12];
    uint8_t *p = buf;

    if (val < 0) {
        *p++ = MMBAK_TAG_BIND_VARINT_MINUS;
        val = ~val;
    } else
        *p++ = MMBAK_TAG_BIND_VARINT;

    do {
        uint8_t b = val & 0x7F;
        val >>= 7;
        if (val)
            b |= 0x80;
        *p++ = b;
    } while (val);

    return dump_write_stream(ctx, buf, p - buf);
}

#define string_literal(str) (str), (sizeof(str) - 1)

static int
dump_write_blob(mm_backup_ctx *ctx, uint8_t tag, const void *data, int size)
{
    const char *p = (const char *) data;
    int ret;

    if (size < 0)
        size = strlen(p);

    if (size >= 65536) {
        uint32_t ext_size = size & 0xFFFF0000;
        uint16_t blocks = ext_size >> 16;
        ret = dump_write_byte(ctx, MMBAK_TAG_LARGE_DATA);
        ret |= dump_write_short(ctx, blocks);
        ret |= dump_write_stream(ctx, p, ext_size);
        if (ret != 0)
            return -1;
        size -= ext_size;
        p += ext_size;
    }

    if (size < 256 &&
        (tag == MMBAK_TAG_BIND_TEXT || tag == MMBAK_TAG_BIND_BLOB))
        ret = dump_write_byte_byte(ctx, ++tag, (uint8_t) size);
    else
        ret = dump_write_byte_short(ctx, tag, (uint16_t) size);
    ret |= dump_write_stream(ctx, p, size);
    if (ret != 0)
        return -1;

    return 0;
}

static int dump_rows(mm_backup_ctx *ctx,
                     const char *table,
                     const char *condition,
                     int reverse)
{
    // find out number of columns, which is used in INSERT statements.
    int i, ret;
    int num_columns = 0;
    char sql_buf[8192];
    sqlite3_stmt *stmt = NULL;
    int row_count = 0;

    sql_buf[0] = '\0';

    int sql_len =
        snprintf(sql_buf, sizeof(sql_buf), "PRAGMA table_info(\"%s\")", table);
    ret = sqlite3_prepare_v2(ctx->db, sql_buf, sql_len, &stmt, NULL);
    if (ret != SQLITE_OK)
        goto bail_log_sql;

    ret = sqlite3_step(stmt);
    while (ret == SQLITE_ROW) {
        num_columns++;
        ret = sqlite3_step(stmt);
    }
    ret = sqlite3_finalize(stmt);
    stmt = NULL;
    if (ret != SQLITE_OK)
        goto bail_log_sql;

    if (num_columns == 0)
        return SQLITE_OK;

    // dump rows in table.
    if (condition)
        sql_len = snprintf(sql_buf, sizeof(sql_buf),
                           "SELECT * FROM \"%s\" WHERE %s;", table, condition);
    else
        sql_len = snprintf(sql_buf, sizeof(sql_buf), "SELECT * FROM \"%s\"%s;",
                           table, reverse ? " ORDER BY rowid DESC" : "");

    ret = sqlite3_prepare_v2(ctx->db, sql_buf, sql_len, &stmt, NULL);
    if (ret != SQLITE_OK)
        goto bail_log_sql;

    row_count = 0;
    ret = sqlite3_step(stmt);
    if (ret == SQLITE_ROW) {
        // output INSERT statement with parameters.
        sql_len = snprintf(sql_buf, sizeof(sql_buf),
                           "REPLACE INTO \"%s\" VALUES(", table);
        for (i = 0; i < num_columns; i++)
            sql_len +=
                strlcpy(sql_buf + sql_len, "?,", sizeof(sql_buf) - sql_len);
        sql_len--;
        sql_len += strlcpy(sql_buf + sql_len, ");", sizeof(sql_buf) - sql_len);
        if (dump_write_blob(ctx, MMBAK_TAG_SQL_REPEATED, sql_buf, sql_len)) {
            ret = SQLITE_ERROR;
            goto bail;
        }
    }
    while (ret == SQLITE_ROW) {
        // check cancel flag.
        if (ctx->canceled) {
            ret = SQLITE_ABORT;
            goto bail;
        }

        for (i = 0; i < num_columns; i++) {
            int type = sqlite3_column_type(stmt, i);
            switch (type) {
                case SQLITE_INTEGER: {
                    int64_t value = sqlite3_column_int64(stmt, i);
                    ret = dump_write_varint(ctx, value);
                } break;

                case SQLITE_FLOAT: {
                    double value = sqlite3_column_double(stmt, i);
                    ret = dump_write_byte(ctx, MMBAK_TAG_BIND_FLOAT);
                    ret |= dump_write_stream(ctx, &value, 8);
                } break;

                case SQLITE_TEXT: {
                    const char *value =
                        (const char *) sqlite3_column_text(stmt, i);
                    int len = sqlite3_column_bytes(stmt, i);
                    ret = dump_write_blob(ctx, MMBAK_TAG_BIND_TEXT, value, len);
                } break;

                case SQLITE_BLOB: {
                    const void *value = sqlite3_column_blob(stmt, i);
                    int len = sqlite3_column_bytes(stmt, i);
                    ret = dump_write_blob(ctx, MMBAK_TAG_BIND_BLOB, value, len);
                } break;

                case SQLITE_NULL:
                    ret = dump_write_byte(ctx, MMBAK_TAG_BIND_NULL);
                    break;

                default:
                    LOGE("Unreconized SQLite type: %d", type);
                    ret = SQLITE_INTERNAL;
                    goto bail;
            }

            if (ret != 0) {
                ret = SQLITE_ERROR;
                goto bail;
            }

            row_count++;
        }

        if (dump_write_byte(ctx, MMBAK_TAG_END_ROW)) {
            ret = SQLITE_ERROR;
            goto bail;
        }
        ctx->stmt_count++;

        ret = sqlite3_step(stmt);
    }
    if (row_count > 0)
        if (dump_write_byte(ctx, MMBAK_TAG_END_SQL)) {
            ret = SQLITE_ERROR;
            goto bail;
        }

    ret = sqlite3_finalize(stmt);
    stmt = NULL;
    if (ret != SQLITE_OK) {
        if (ret == SQLITE_CORRUPT)
            goto bail_log_corrupt;
        goto bail_log_sql;
    }
    return ret;

bail_log_corrupt:
    LOGE("Database corruption detected. [table: %s, condition: %s, row: %d, "
         "reverse: %s]",
         table, condition ? condition : "null", row_count,
         reverse ? "yes" : "no");
    goto bail;
bail_log_sql:
    LOGE("SQL execution failed: %s, [SQL: %s]", sqlite3_errmsg(ctx->db),
         stmt ? sqlite3_sql(stmt) : sql_buf);
bail:
    if (stmt)
        sqlite3_finalize(stmt);
    return ret;
}

static int compar_table_tabdesc(const void *key, const void *value)
{
    return strcmp((const char *) key,
                  ((const mm_backup_tabdesc *) value)->table);
}

static int
dump_callback(void *ud, int num_columns, char **values, char **columns)
{
    mm_backup_ctx *ctx = (mm_backup_ctx *) ud;
    const char *table = values[0];
    const char *type = values[1];
    const char *sql = values[2];
    int ret;

    // check exit status.
    if (ctx->canceled)
        return -1;

    // deal with system tables.
    if (!strcmp(table, "sqlite_sequence")) {
        if (dump_write_blob(ctx, MMBAK_TAG_SQL_ONESHOT,
                            string_literal("DELETE FROM sqlite_sequence;")) !=
            0)
            return -1;
    } else if (!sqlite3_strglob("sqlite_stat?", table)) {
        if (dump_write_blob(ctx, MMBAK_TAG_SQL_ONESHOT,
                            string_literal("ANALYZE sqlite_master;")))
            return -1;
    } else if (!strncmp(table, "sqlite_", 7)) {
        return 0;
    }

    // find whether in dump list.
    const char *table_condition = NULL;
    if (ctx->tabdesc) {
        const mm_backup_tabdesc *desc = (const mm_backup_tabdesc *) bsearch(
            table, ctx->tabdesc, ctx->num_tabdesc, sizeof(mm_backup_tabdesc),
            compar_table_tabdesc);
        if (!desc)
            return 0;

        table_condition = desc->condition;
    }

    if (!strncmp(sql, "CREATE VIRTUAL TABLE", 20)) {
        if (!(ctx->flags & MMBAK_FLAG_NO_CREATE_TABLE)) {
            if (!ctx->writable_schema) {
                if (dump_write_blob(
                        ctx, MMBAK_TAG_SQL_ONESHOT,
                        string_literal("PRAGMA writable_schema=ON;")))
                    return -1;
                ctx->writable_schema = 1;
            }

            char *vt_sql = sqlite3_mprintf(
                "INSERT INTO sqlite_master(type,name,tbl_name,rootpage,sql)"
                "VALUES('table','%q','%q',0,'%q');",
                table, table, sql);
            if (dump_write_blob(ctx, MMBAK_TAG_SQL_ONESHOT, vt_sql, -1))
                return -1;
            sqlite3_free(vt_sql);
        }

        return 0;
    } else if (!(ctx->flags & MMBAK_FLAG_NO_CREATE_TABLE)) {
        if (dump_write_blob(ctx, MMBAK_TAG_SQL_ONESHOT, sql, -1))
            return -1;
    }

    // for tables, dump all rows.
    if (!strcmp(type, "table")) {
        ret = dump_rows(ctx, table, table_condition, 0);
        if (ret == SQLITE_CORRUPT && !table_condition &&
            (ctx->flags & MMBAK_FLAG_FIX_CORRUPTION)) {
            ret = dump_rows(ctx, table, table_condition, 1);
            if (ret == SQLITE_CORRUPT)
                ret = SQLITE_OK;
        }
        if (ret != SQLITE_OK)
            return -1;
    }
    return 0;
}

static int run_schema_dump_query(mm_backup_ctx *ctx, const char *query_sql)
{
    // check cancel status.
    if (ctx->canceled)
        return MMBAK_RET_CANCELED;

    // dump all tables.
    char *errmsg = NULL;
    int ret = sqlite3_exec(ctx->db, query_sql, dump_callback, ctx, &errmsg);
    if (ret == SQLITE_ABORT) {
        // abort due to user cancellation or error.
        if (errmsg)
            sqlite3_free(errmsg);
        return (ctx->canceled) ? MMBAK_RET_CANCELED : MMBAK_RET_FAILED;
    }

    if (ret == SQLITE_CORRUPT && (ctx->flags & MMBAK_FLAG_FIX_CORRUPTION)) {
        // in case of database corruption, dump tables in reverse order.
        if (errmsg) {
            LOGE("SQL execution failed: %s [SQL: %s]", errmsg, query_sql);
            sqlite3_free(errmsg);
            errmsg = NULL;
        }

        size_t len = strlen(query_sql);
        char *query_sql_reverse = (char *) malloc(len + 32);
        if (!query_sql_reverse)
            return ret;
        sqlite3_snprintf(len + 32, query_sql_reverse, "%s ORDER BY rowid DESC",
                         query_sql);

        ret = sqlite3_exec(ctx->db, query_sql_reverse, dump_callback, ctx,
                           &errmsg);
        free(query_sql_reverse);
    }

    if (errmsg) {
        LOGE("SQL execution failed: %s [SQL: %s]", errmsg, query_sql);
        sqlite3_free(errmsg);
    }
    return (ret == SQLITE_OK) ? MMBAK_RET_OK : MMBAK_RET_FAILED;
}

mm_backup_ctx *mm_backup_init(const unsigned char *key,
                              int key_len,
                              const char *out_path,
                              unsigned int flags,
                              mm_logfunc logfunc)
{
    if (!logfunc)
        logfunc = dummy_log;

    mm_backup_ctx *ctx = (mm_backup_ctx *) malloc(sizeof(mm_backup_ctx));
    if (!ctx) {
        logfunc(MMBAK_LOG_ERROR, "Memory allocation failed.");
        return NULL;
    }
    memset(ctx, 0, sizeof(mm_backup_ctx));
    ctx->log = logfunc;

    int compress_level =
        (flags & MMBAK_FLAG_NO_COMPRESS) ? 0 : Z_DEFAULT_COMPRESSION;
    if (Z_OK != deflateInit(&ctx->zstrm, compress_level))
        LOG_AND_FAIL("Failed to initialize deflate: %s",
                     ctx->zstrm.msg ? ctx->zstrm.msg : "Unknown");

    if (key && key_len > 0)
        RC4_set_key(&ctx->rc4_key, key_len, key);
    else
        flags |= MMBAK_FLAG_NO_CIPHER;

    if (flags & MMBAK_FLAG_INCREMENTAL) {
        ctx->fp = fopen(out_path, "rb+");
        if (!ctx->fp)
            LOG_AND_FAIL("Cannot open file '%s' for appending: %s", out_path,
                         strerror(errno));

        // check header and version.
        mm_backup_header header;
        int ret = fread(&header, sizeof(mm_backup_header), 1, ctx->fp);
        if (ret < 1)
            LOG_AND_FAIL("Invalid header in base file '%s'.", out_path);

        if (memcmp(header.magic, MMBAK_MAGIC, sizeof(MMBAK_MAGIC) - 1) != 0 ||
            header.version != MMBAK_VERSION)
            LOG_AND_FAIL("Invalid header in base file '%s'.", out_path);

        // seek to end.
        ret = fseek(ctx->fp, 0, SEEK_END);
        if (ret != 0)
            LOG_AND_FAIL("Cannot seek in file '%s': %s", out_path,
                         strerror(errno));
    } else {
        ctx->fp = fopen(out_path, "wb");
        if (!ctx->fp)
            LOG_AND_FAIL("Cannot open file '%s' for writing: %s", out_path,
                         strerror(errno));

        // write file header.
        mm_backup_header header;
        memcpy(header.magic, MMBAK_MAGIC, sizeof(MMBAK_MAGIC) - 1);
        header.version = MMBAK_VERSION;
        int ret = fwrite(&header, sizeof(header), 1, ctx->fp);
        if (ret < 1)
            LOG_AND_FAIL("Cannot write to file '%s': %s", out_path,
                         strerror(errno));
    }
    ctx->sec_pos = (uint32_t) ftell(ctx->fp);

    pthread_mutex_init(&ctx->buf_lock, NULL);
    pthread_cond_init(&ctx->buf_cond, NULL);
    ctx->flags = flags;
    ctx->canceled = 1;
    ctx->in_finish = 0;
    ctx->in_avail = 0;
    ctx->in_full = 0;
    ctx->write_error = 0;
    ctx->buffer[0].avail = 0;
    ctx->buffer[1].avail = 0;
    ctx->p_buf = &ctx->buffer[0];

    LOGI("Database backup context initialized. [output: %s, flags: 0x%04x]",
         out_path, flags);
    return ctx;

bail:
    if (ctx) {
        deflateEnd(&ctx->zstrm);
        if (ctx->fp)
            fclose(ctx->fp);
        free(ctx);
    }
    return NULL;
}

static int compar_tabdesc(const void *a, const void *b)
{
    return strcmp(((const mm_backup_tabdesc *) a)->table,
                  ((const mm_backup_tabdesc *) b)->table);
}

int mm_backup_run(mm_backup_ctx *ctx,
                  sqlite3 *db,
                  const mm_backup_tabdesc *desc,
                  int num_desc)
{
    char *errmsg = NULL;
    int ret = MMBAK_RET_FAILED;

    const char *dbfile = sqlite3_db_filename(db, "main");
    LOGI("Database backup started. [db: %s]",
         dbfile ? dbfile : "(temp or memory)");

    // Make a sorted copy of tabdesc.
    if (desc && num_desc > 0) {
        ctx->tabdesc =
            (mm_backup_tabdesc *) malloc(sizeof(mm_backup_tabdesc) * num_desc);
        if (!ctx->tabdesc)
            LOG_AND_FAIL("Not enough memory.");
        memcpy(ctx->tabdesc, desc, sizeof(mm_backup_tabdesc) * num_desc);
        qsort(ctx->tabdesc, num_desc, sizeof(mm_backup_tabdesc),
              compar_tabdesc);

        ctx->num_tabdesc = num_desc;

        // adjust/print tabdesc.
        LOGI("Backup with table descriptors. [count: %d]", ctx->num_tabdesc);
        int i;
        for (i = 0; i < num_desc; i++) {
            if (ctx->tabdesc[i].condition && ctx->tabdesc[i].condition[0] == 0)
                ctx->tabdesc[i].condition = NULL;
            LOGI("  > table: %s, condition: %s", ctx->tabdesc[i].table,
                 ctx->tabdesc[i].condition ? ctx->tabdesc[i].condition
                                           : "null");
        }
    } else {
        ctx->tabdesc = NULL;
        ctx->num_tabdesc = 0;
    }

    ctx->db = db;
    ctx->canceled = 0;
    ctx->stmt_count = 0;
    ctx->sec_size = 0;

    // initialize write thread.
    if (pthread_create(&ctx->write_thread, NULL, write_thread_entry, ctx) != 0)
        LOG_AND_FAIL("Cannot initialize writer thread.");

    if (sqlite3_exec(db, "SAVEPOINT dump; PRAGMA writable_schema=ON;", 0, 0,
                     &errmsg) != SQLITE_OK)
        LOG_AND_FAIL("Cannot execute startup SQL: %s",
                     errmsg ? errmsg : "Unknown");

    // write section placeholder.
    mm_backup_section section = {.flags = ctx->flags, .sec_size = 0};
    if (fwrite(&section, sizeof(mm_backup_section), 1, ctx->fp) < 1)
        LOG_AND_FAIL("Cannot write to file: %s", strerror(errno));

    // dump tables.
    if ((ret = run_schema_dump_query(
             ctx, "SELECT name, type, sql FROM sqlite_master WHERE sql NOT NULL"
                  " AND type=='table' AND name!='sqlite_sequence'")) !=
        MMBAK_RET_OK)
        goto bail;
    if ((ret = run_schema_dump_query(
             ctx, "SELECT name, type, sql FROM sqlite_master WHERE "
                  "name=='sqlite_sequence'")) != MMBAK_RET_OK)
        goto bail;
    if ((ret = run_schema_dump_query(
             ctx, "SELECT name, type, sql FROM sqlite_master WHERE sql NOT NULL"
                  " AND type IN ('index','trigger','view')")) != MMBAK_RET_OK)
        goto bail;

    // recover writable_schema.
    if (ctx->writable_schema)
        if (dump_write_blob(ctx, MMBAK_TAG_SQL_ONESHOT,
                            string_literal("PRAGMA writable_schema=OFF;")) != 0)
            goto bail;

    if (dump_write_end(ctx) != 0)
        goto bail;

    sqlite3_exec(db, "PRAGMA writable_schema=OFF; RELEASE dump;", 0, 0, 0);
    pthread_join(ctx->write_thread, NULL);

    // write section header.
    section.sec_size = ctx->sec_size;
    ret = fseek(ctx->fp, ctx->sec_pos, SEEK_SET);
    if (ret != 0)
        LOG_AND_FAIL("Cannot seek in file: %s", strerror(errno));
    ret = fwrite(&section, sizeof(mm_backup_section), 1, ctx->fp);
    if (ret < 1)
        LOG_AND_FAIL("Cannot write to file: %s", strerror(errno));
    fflush(ctx->fp);

    ctx->canceled = 1;

    LOGI("Database backup finished. [items: %u, section size: %u]",
         ctx->stmt_count, ctx->sec_size);
    return 0;

bail:
    if (!ctx->canceled) {
        ctx->canceled = 1;
        pthread_cond_broadcast(&ctx->buf_cond);
    }
    pthread_detach(ctx->write_thread);
    sqlite3_exec(db, "PRAGMA writable_schema=OFF; RELEASE dump;", 0, 0, 0);
    if (errmsg)
        sqlite3_free(errmsg);
    return ret;
}

void mm_backup_cancel(mm_backup_ctx *ctx)
{
    ctx->canceled = 1;
    pthread_cond_broadcast(&ctx->buf_cond);
    LOGI_("Backup operation canceled.");
}

void mm_backup_finish(mm_backup_ctx *ctx)
{
    if (!ctx->canceled) {
        ctx->canceled = 1;
        pthread_cond_broadcast(&ctx->buf_cond);
        pthread_detach(ctx->write_thread);
    }

    free(ctx->tabdesc);
    pthread_mutex_destroy(&ctx->buf_lock);
    pthread_cond_destroy(&ctx->buf_cond);

    if (ctx->fp)
        fclose(ctx->fp);
    deflateEnd(&ctx->zstrm);
    free(ctx);
}

void mm_backup_statistics(mm_backup_ctx *ctx, unsigned int *stmt_count)
{
    if (stmt_count)
        *stmt_count = ctx->stmt_count;
}

const char *mm_backup_last_error(mm_backup_ctx *ctx)
{
    return ctx->errmsg;
}
