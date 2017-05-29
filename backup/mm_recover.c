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
#include <stdio.h>
#include <stdlib.h>

#include <openssl/rc4.h>
#include <zlib.h>

struct mm_recover_ctx {
    sqlite3 *db; // database connection, not initialized until actual running.
    uint32_t flags;         // section flags.
    z_stream zstrm;         // input compression stream.
    RC4_KEY rc4_template;   // key template.
    RC4_KEY rc4_key;        // input file decryption key.
    FILE *fp;               // input file handle.
    mm_logfunc log;         // log function.
    uint32_t sec_size;      // section size.
    uint32_t succ_count;    // count of successfully executed statments.
    uint32_t fail_count;    // count of failed statments.
    uint32_t section_count; // count of processed sections.
    uint16_t version;       // dump file version.

    volatile char canceled; // task status.

    uint8_t in_buf[4096]; // gzip input buffer.

    char errmsg[2048];
};

static const char *const BIND_TAG_TEXT[] = {
    "BIND_NULL",         // 0x00
    "BIND_VARINT",       // 0x01
    "BIND_VARINT_MINUS", // 0x02
    "BIND_FLOAT",        // 0x03
    "BIND_TEXT",         // 0x04
    "BIND_TEXT_SHORT",   // 0x05
    "BIND_BLOB",         // 0x06
    "BIND_BLOB_SHORT",   // 0x07
};
static const char *const FUNC_TAG_TEXT[] = {
    "LARGE_DATA",   // 0x80
    "SQL_ONESHOT",  // 0x81
    "SQL_REPEATED", // 0x82
    "END_ROW",      // 0x83
    "END_SQL",      // 0x84
};
static const char *tag2text(uint8_t tag)
{
    return (tag & 0x80) ? FUNC_TAG_TEXT[tag & 0x7F] : BIND_TAG_TEXT[tag];
}

static int read_header(mm_recover_ctx *ctx)
{
    mm_backup_header header;

    int ret = fread(&header, sizeof(header), 1, ctx->fp);
    if (ret != 1)
        LOG_AND_FAIL("Cannot read backup header.");

    if (memcmp(header.magic, MMBAK_MAGIC, sizeof(MMBAK_MAGIC) - 1) != 0)
        LOG_AND_FAIL("Invalid backup file format.");

    if (header.version != 1 && header.version != 2)
        LOG_AND_FAIL("Invalid backup file version: %d", header.version);

    ctx->version = header.version;
    return 0;

bail:
    return MMBAK_RET_FAILED;
}

static int dump_read_stream(mm_recover_ctx *ctx, void *buf, uint_t size)
{
    int ret;

    if (size == 0)
        return 0;

    ctx->zstrm.next_out = (uint8_t *) buf;
    ctx->zstrm.avail_out = size;

    do {
        if (ctx->zstrm.avail_in == 0 && ctx->sec_size > 0) {
            ret = (ctx->sec_size > sizeof(ctx->in_buf)) ? sizeof(ctx->in_buf)
                                                        : ctx->sec_size;

            ret = fread(ctx->in_buf, 1, ret, ctx->fp);
            if (ret == 0 && ferror(ctx->fp))
                LOG_AND_FAIL("Failed reading stream: %s", strerror(errno));
            if (ret > 0) {
                if ((ctx->flags & MMBAK_FLAG_NO_CIPHER) == 0)
                    RC4(&ctx->rc4_key, ret, ctx->in_buf, ctx->in_buf);
                ctx->zstrm.next_in = ctx->in_buf;
                ctx->zstrm.avail_in = ret;

                ctx->sec_size -= ret;
            }
        }

        ret = inflate(&ctx->zstrm, Z_NO_FLUSH);
        if (ret != Z_OK && ret != Z_STREAM_END)
            LOG_AND_FAIL("Inflate error: %s",
                         ctx->zstrm.msg ? ctx->zstrm.msg : "Unknown");

    } while (ctx->zstrm.avail_out > 0 && ret != Z_STREAM_END);

    return size - ctx->zstrm.avail_out;

bail:
    return MMBAK_RET_FAILED;
}

static int dump_read_varint(mm_recover_ctx *ctx, int64_t *val, int minus)
{
    uint8_t b;
    int ret;
    int64_t v = 0;
    int shift = 0;

    do {
        ret = dump_read_stream(ctx, &b, 1);
        if (ret != 1)
            return MMBAK_RET_FAILED;

        v |= (int64_t)(b & 0x7F) << shift;
        shift += 7;
    } while (b & 0x80);

    if (minus)
        v = ~v;
    *val = v;
    return 0;
}

#define READ_STREAM_FAIL(tag, len, ret)                                        \
    LOG_AND_FAIL("Read stream failed. [tag: %s, length: %d, ret: %d]",         \
                 tag2text((tag)), (len), (ret))
#define MEMORY_ALLOC_FAIL(tag, len)                                            \
    LOG_AND_FAIL("Memory allocation failed. [tag: %s, length: %d]",            \
                 tag2text((tag)), (len))
#define SQLITE_FAIL_ERRMSG(tag, sql, errmsg)                                   \
    LOG_AND_FAIL("SQL execution failed. [tag: %s, err: %s, sql: %s]",          \
                 tag2text((tag)), (errmsg), (sql))
#define SQLITE_FAIL_GETMSG(tag, sql, db)                                       \
    LOG_AND_FAIL("SQL execution failed. [tag: %s, err: %s, sql: %s]",          \
                 tag2text((tag)), sqlite3_errmsg((db)), (sql))
#define CUSTOM_FAIL(tag, msg)                                                  \
    LOG_AND_FAIL("%s [tag: %s]", (msg), tag2text((tag)))

#define SQLITE_FAIL_ERRMSG_WARN(tag, sql, errmsg)                              \
    LOGE("SQL execution failed. [tag: %s, err: %s, sql: %s]", tag2text((tag)), \
         (errmsg), (sql))
#define SQLITE_FAIL_GETMSG_WARN(tag, sql, db)                                  \
    LOGE("SQL execution failed. [tag: %s, err: %s, sql: %s]", tag2text((tag)), \
         sqlite3_errmsg((db)), (sql))

static int dump_read_and_run_tag(mm_recover_ctx *ctx, int fatal)
{
    int ret;
    uint8_t tag;
    uint8_t length_short;
    uint16_t length;
    char *data = NULL;
    char *p_data = NULL;
    sqlite3_stmt *stmt = NULL;
    int bind_idx;
    char *errmsg = NULL;
    char databuf[8192];

    while (!ctx->canceled) {
        ret = dump_read_stream(ctx, &tag, 1);
        if (ret == 0)
            return MMBAK_RET_OK;
        else if (ret != 1)
            return MMBAK_RET_FAILED;

        switch (tag) {
            case MMBAK_TAG_LARGE_DATA:
                if (data != NULL)
                    CUSTOM_FAIL(tag, "Internal error.");
                if ((ret = dump_read_stream(ctx, &length, 2)) != 2)
                    READ_STREAM_FAIL(tag, 2, ret);

                data = (char *) malloc((length + 1) * 65536);
                if (!data)
                    MEMORY_ALLOC_FAIL(tag, (length + 1) * 65536);

                if ((ret = dump_read_stream(ctx, data, length * 65536)) !=
                    length * 65536)
                    READ_STREAM_FAIL(tag, length * 65536, ret);

                p_data = data;
                //LOGD("[%s] len=%u * 65536", "LARGE_DATA", length);
                break;

            case MMBAK_TAG_SQL_ONESHOT:
                if ((ret = dump_read_stream(ctx, &length, 2)) != 2)
                    READ_STREAM_FAIL(tag, 2, ret);

                if (!p_data) {
                    if (length < sizeof(databuf))
                        data = databuf;
                    else {
                        data = (char *) malloc(length + 1);
                        if (!data)
                            MEMORY_ALLOC_FAIL(tag, length + 1);
                    }
                    p_data = data;
                }

                if ((ret = dump_read_stream(ctx, p_data, length)) != length)
                    READ_STREAM_FAIL(tag, length, ret);
                p_data[length] = '\0';

                ret = sqlite3_exec(ctx->db, data, 0, 0, &errmsg);
                if (ret != SQLITE_OK) {
                    SQLITE_FAIL_ERRMSG_WARN(tag, data, errmsg);
                    if (fatal)
                        goto bail;

                    sqlite3_free(errmsg);
                    ctx->fail_count++;
                } else
                    ctx->succ_count++;

                //LOGD("[%s] sql=%s", "SQL_ONESHOT", data);
                if (data != databuf)
                    free(data);
                p_data = data = NULL;
                break;

            case MMBAK_TAG_SQL_REPEATED:
                if ((ret = dump_read_stream(ctx, &length, 2)) != 2)
                    READ_STREAM_FAIL(tag, 2, ret);

                if (!p_data) {
                    if (length < sizeof(databuf))
                        data = databuf;
                    else {
                        data = (char *) malloc(length + 1);
                        if (!data)
                            MEMORY_ALLOC_FAIL(tag, length + 1);
                    }
                    p_data = data;
                }
                if ((ret = dump_read_stream(ctx, p_data, length)) != length)
                    READ_STREAM_FAIL(tag, length, ret);
                p_data[length] = '\0';

                ret = sqlite3_prepare_v2(ctx->db, data,
                                         (p_data - data) + length, &stmt, NULL);
                if (ret != SQLITE_OK) {
                    SQLITE_FAIL_GETMSG_WARN(tag, data, ctx->db);
                    if (fatal)
                        goto bail;

                    stmt = NULL;
                }

                //LOGD("[%s] sql=%s", "SQL_REPEATED", data);
                if (data != databuf)
                    free(data);
                p_data = data = NULL;

                bind_idx = 0;
                do {
                    // check exit status.
                    if (ctx->canceled) {
                        if (data && data != databuf)
                            free(data);
                        if (stmt)
                            sqlite3_finalize(stmt);
                        return MMBAK_RET_CANCELED;
                    }

                    if ((ret = dump_read_stream(ctx, &tag, 1)) != 1)
                        READ_STREAM_FAIL(tag, 1, ret);

                    switch (tag) {
                        union {
                            int64_t i;
                            double f;
                        } value;

                        case MMBAK_TAG_LARGE_DATA:
                            if (data != NULL)
                                CUSTOM_FAIL(tag, "Internal error.");
                            if ((ret = dump_read_stream(ctx, &length, 2)) != 2)
                                READ_STREAM_FAIL(tag, 2, ret);

                            data = (char *) malloc((length + 1) * 65536);
                            if (!data)
                                MEMORY_ALLOC_FAIL(tag, (length + 1) * 65536);

                            if ((ret = dump_read_stream(ctx, data,
                                                        length * 65536)) !=
                                length * 65536)
                                READ_STREAM_FAIL(tag, length * 65536, ret);

                            p_data = data;
                            //LOGD(" > [%s] len=%u * 65536", "LARGE_DATA", length);
                            break;

                        case MMBAK_TAG_END_ROW:
                            if (!stmt)
                                ctx->fail_count++;
                            else {
                                while (sqlite3_step(stmt) == SQLITE_ROW) {
                                }
                                ret = sqlite3_reset(stmt);
                                if (ret != SQLITE_OK) {
                                    SQLITE_FAIL_GETMSG_WARN(
                                        tag, sqlite3_sql(stmt), ctx->db);
                                    if (fatal)
                                        goto bail;

                                    ctx->fail_count++;
                                } else {
                                    ctx->succ_count++;
                                    if (ctx->succ_count % 256 == 0) {
                                        // We have run 256 insertions, do a transaction commit.
                                        ret = sqlite3_exec(ctx->db,
                                                           "COMMIT; BEGIN;", 0,
                                                           0, &errmsg);
                                        if (ret != SQLITE_OK) {
                                            SQLITE_FAIL_ERRMSG_WARN(tag, data,
                                                                    errmsg);
                                            if (fatal)
                                                goto bail;

                                            sqlite3_free(errmsg);
                                            ctx->fail_count++;
                                        }
                                    }
                                }
                            }

                            bind_idx = 0;
                            //LOGD(" > [%s]", "END_ROW");
                            break;

                        case MMBAK_TAG_END_SQL:
                            //LOGD(" > [%s]", "END_SQL");
                            break; // end of loop

                        case MMBAK_TAG_BIND_NULL:
                            if (stmt)
                                sqlite3_bind_null(stmt, ++bind_idx);
                            //LOGD(" > [%s]", "BIND_NULL");
                            break;

                        case MMBAK_TAG_BIND_VARINT:
                        case MMBAK_TAG_BIND_VARINT_MINUS:
                            if (dump_read_varint(
                                    ctx, &value.i,
                                    tag == MMBAK_TAG_BIND_VARINT_MINUS) != 0)
                                CUSTOM_FAIL(tag, "Read varint failed.");
                            if (stmt)
                                sqlite3_bind_int64(stmt, ++bind_idx, value.i);
                            //LOGD(" > [%s] value=%lld", tag2text(tag), value.i);
                            break;

                        case MMBAK_TAG_BIND_FLOAT:
                            if ((ret = dump_read_stream(ctx, &value, 8)) != 8)
                                READ_STREAM_FAIL(tag, 8, ret);
                            if (stmt)
                                sqlite3_bind_double(stmt, ++bind_idx, value.f);
                            //LOGD(" > [%s] value=%f", "BIND_FLOAT", value.f);
                            break;

                        case MMBAK_TAG_BIND_TEXT:
                        case MMBAK_TAG_BIND_BLOB:
                            if ((ret = dump_read_stream(ctx, &length, 2)) != 2)
                                READ_STREAM_FAIL(tag, 2, ret);
                            if (!p_data) {
                                data = (char *) malloc(length);
                                if (!data)
                                    MEMORY_ALLOC_FAIL(tag, length);
                                p_data = data;
                            }
                            if ((ret = dump_read_stream(ctx, p_data, length)) !=
                                length)
                                READ_STREAM_FAIL(tag, length, ret);

                            //LOGD(" > [%s] value=%s", tag2text(tag), tag == MMBAK_TAG_BIND_TEXT ? data : "(BLOB)");
                            if (stmt) {
                                if (tag == MMBAK_TAG_BIND_TEXT)
                                    sqlite3_bind_text(stmt, ++bind_idx, data,
                                                      (p_data - data) + length,
                                                      free);
                                else
                                    sqlite3_bind_blob(stmt, ++bind_idx, data,
                                                      (p_data - data) + length,
                                                      free);
                            } else
                                free(data);

                            p_data = data = NULL;
                            break;

                        case MMBAK_TAG_BIND_TEXT_SHORT:
                        case MMBAK_TAG_BIND_BLOB_SHORT:
                            if ((ret = dump_read_stream(ctx, &length_short,
                                                        1)) != 1)
                                READ_STREAM_FAIL(tag, 1, ret);
                            length = length_short;
                            if (!p_data) {
                                data = (char *) malloc(length);
                                if (!data)
                                    MEMORY_ALLOC_FAIL(tag, length);
                                p_data = data;
                            }
                            if ((ret = dump_read_stream(ctx, p_data, length)) !=
                                length)
                                READ_STREAM_FAIL(tag, length, ret);

                            //LOGD(" > [%s] value=%s", tag2text(tag), tag == MMBAK_TAG_BIND_TEXT_SHORT ? data : "(BLOB)");
                            if (stmt) {
                                if (tag == MMBAK_TAG_BIND_TEXT_SHORT)
                                    sqlite3_bind_text(stmt, ++bind_idx, data,
                                                      (p_data - data) + length,
                                                      free);
                                else
                                    sqlite3_bind_blob(stmt, ++bind_idx, data,
                                                      (p_data - data) + length,
                                                      free);
                            } else
                                free(data);

                            p_data = data = NULL;
                            break;

                        default:
                            LOG_AND_FAIL("Unrecognized tag: %d", tag);
                    }
                } while (tag != MMBAK_TAG_END_SQL);

                if (stmt && sqlite3_finalize(stmt) != SQLITE_OK)
                    SQLITE_FAIL_GETMSG(tag, "(FINALIZE)", ctx->db);
                break;

            default:
                LOG_AND_FAIL("Unrecognized tag: %d", tag);
        }
    }
    return MMBAK_RET_CANCELED;

bail:
    if (data && data != databuf)
        free(data);
    if (errmsg)
        sqlite3_free(errmsg);
    if (stmt)
        sqlite3_finalize(stmt);
    LOGE("Fatal Offset [in: %lu, out: %lu]", ctx->zstrm.total_in,
         ctx->zstrm.total_out);
    return MMBAK_RET_FAILED;
}

mm_recover_ctx *mm_recover_init(const char *in_path,
                                const unsigned char *key,
                                int key_len,
                                mm_logfunc logfunc)
{
    if (!logfunc)
        logfunc = dummy_log;

    mm_recover_ctx *ctx = (mm_recover_ctx *) malloc(sizeof(mm_recover_ctx));
    if (!ctx) {
        logfunc(MMBAK_LOG_ERROR, "Memory allocation failed.");
        return NULL;
    }
    memset(ctx, 0, sizeof(mm_recover_ctx));
    ctx->log = logfunc;

    if (key && key_len > 0)
        RC4_set_key(&ctx->rc4_template, key_len, key);
    else
        ctx->flags |= MMBAK_FLAG_NO_CIPHER;

    ctx->fp = fopen(in_path, "rb");
    if (!ctx->fp)
        LOG_AND_FAIL("Cannot open file '%s' for reading: %s", in_path,
                     strerror(errno));

    // read header from dump file.
    if (read_header(ctx) != 0)
        goto bail;

    LOGI("Database recover context initialized. [input: %s]", in_path);
    return ctx;

bail:
    if (ctx) {
        inflateEnd(&ctx->zstrm);
        if (ctx->fp)
            fclose(ctx->fp);
        free(ctx);
    }
    return NULL;
}

int mm_recover_run(mm_recover_ctx *ctx, sqlite3 *db, int fatal)
{
    mm_backup_section section;
    int ret = MMBAK_RET_FAILED;
    char *errmsg = NULL;
    ctx->db = db;

    const char *dbfile = sqlite3_db_filename(db, "main");
    LOGI("Database recover started. [db: %s]",
         dbfile ? dbfile : "(temp or memory)");

    // run startup SQLs.
    if (sqlite3_exec(db, "PRAGMA foreign_keys=OFF; BEGIN TRANSACTION;", 0, 0,
                     &errmsg) != SQLITE_OK)
        LOG_AND_FAIL("Cannot execute startup SQL: %s",
                     errmsg ? errmsg : "Unknown");

    ctx->succ_count = 0;
    ctx->fail_count = 0;
    ctx->section_count = 0;

    while ((ret = fread(&section, sizeof(section), 1, ctx->fp)) == 1) {
        ctx->flags = section.flags;
        ctx->sec_size = section.sec_size;
        ctx->section_count++;

        // re-initialize inflate and key context.
        if (Z_OK != inflateInit(&ctx->zstrm))
            LOG_AND_FAIL("Failed to initialize inflate context.");
        ctx->zstrm.next_in = ctx->in_buf;
        ctx->zstrm.avail_in = 0;

        if (!(ctx->flags & MMBAK_FLAG_NO_CIPHER))
            memcpy(&ctx->rc4_key, &ctx->rc4_template, sizeof(RC4_KEY));

        // read and run dump tags.
        ret = dump_read_and_run_tag(ctx, fatal);
        if (ret != MMBAK_RET_OK)
            break;
    }

    // run endup SQLs.
    if (sqlite3_exec(db, "COMMIT;", 0, 0, &errmsg) != SQLITE_OK)
        LOG_AND_FAIL("Cannot execute 'COMMIT': %s",
                     errmsg ? errmsg : "Unknown");

    LOGI("Database recover %s. [sections: %u, succeeded: %u, failed: %u]",
         ret == MMBAK_RET_OK
             ? "finished"
             : (ret == MMBAK_RET_CANCELED) ? "canceled" : "failed",
         ctx->section_count, ctx->succ_count, ctx->fail_count);
    return ret;

bail:
    return MMBAK_RET_FAILED;
}

void mm_recover_cancel(mm_recover_ctx *ctx)
{
    ctx->canceled = 1;
}

void mm_recover_finish(mm_recover_ctx *ctx)
{
    if (ctx->fp)
        fclose(ctx->fp);
    inflateEnd(&ctx->zstrm);
    free(ctx);
}

void mm_recover_statistics(mm_recover_ctx *ctx,
                           unsigned int *succeed,
                           unsigned int *failed)
{
    if (succeed)
        *succeed = ctx->succ_count;
    if (failed)
        *failed = ctx->fail_count;
}

const char *mm_recover_last_error(mm_recover_ctx *ctx)
{
    return ctx->errmsg;
}

void mm_print_log(mm_logfunc logfunc, int prio, const char *fmt, ...)
{
    char buf[2048];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    logfunc(prio, buf);
}

void mm_print_err(mm_logfunc logfunc, char *errmsg, const char *fmt, ...)
{
    // errmsg should be 2048 bytes long.
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(errmsg, 2048, fmt, ap);
    va_end(ap);

    logfunc(MMBAK_LOG_ERROR, errmsg);
}

void dummy_log(int prio, const char *msg)
{
    // do nothing.
}
