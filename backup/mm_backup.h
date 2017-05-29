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

#ifndef __MM_BACKUP_H__
#define __MM_BACKUP_H__

#include <sqlite3.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MMBAK_RET_FAILED (-1)
#define MMBAK_RET_OK 0
#define MMBAK_RET_CANCELED 1

#define MMBAK_FLAG_NO_CIPHER 0x0001
#define MMBAK_FLAG_NO_COMPRESS 0x0002
#define MMBAK_FLAG_FIX_CORRUPTION 0x0004
#define MMBAK_FLAG_NO_CREATE_TABLE 0x0008
#define MMBAK_FLAG_INCREMENTAL 0x0010

#define MMBAK_LOG_DEBUG 0
#define MMBAK_LOG_INFO 1
#define MMBAK_LOG_ERROR 2

typedef struct mm_backup_ctx mm_backup_ctx;
typedef struct mm_recover_ctx mm_recover_ctx;

typedef void (*mm_logfunc)(int prio, const char *msg);

typedef struct mm_backup_tabdesc {
    const char *table;
    const char *condition;
} mm_backup_tabdesc;

mm_backup_ctx *mm_backup_init(const unsigned char *key,
                              int key_len,
                              const char *out_path,
                              unsigned int flags,
                              mm_logfunc logfunc);
int mm_backup_run(mm_backup_ctx *ctx,
                  sqlite3 *db,
                  const mm_backup_tabdesc *desc,
                  int num_desc);
void mm_backup_statistics(mm_backup_ctx *ctx, unsigned int *stmt_count);
void mm_backup_cancel(mm_backup_ctx *ctx);
void mm_backup_finish(mm_backup_ctx *ctx);
const char *mm_backup_last_error(mm_backup_ctx *ctx);

mm_recover_ctx *mm_recover_init(const char *in_path,
                                const unsigned char *key,
                                int key_len,
                                mm_logfunc logfunc);
int mm_recover_run(mm_recover_ctx *ctx, sqlite3 *db, int fatal);
void mm_recover_statistics(mm_recover_ctx *ctx,
                           unsigned int *succeeded,
                           unsigned int *failed);
void mm_recover_cancel(mm_recover_ctx *ctx);
void mm_recover_finish(mm_recover_ctx *ctx);
const char *mm_recover_last_error(mm_recover_ctx *ctx);

#if defined(MM_BACKUP_INCLUDE_INTERNAL)

#define MMBAK_TAG_LARGE_DATA 0x80
#define MMBAK_TAG_SQL_ONESHOT 0x81
#define MMBAK_TAG_SQL_REPEATED 0x82
#define MMBAK_TAG_END_ROW 0x83
#define MMBAK_TAG_END_SQL 0x84

#define MMBAK_TAG_BIND_NULL 0x00
#define MMBAK_TAG_BIND_VARINT 0x01
#define MMBAK_TAG_BIND_VARINT_MINUS 0x02
#define MMBAK_TAG_BIND_FLOAT 0x03
#define MMBAK_TAG_BIND_TEXT 0x04
#define MMBAK_TAG_BIND_TEXT_SHORT 0x05
#define MMBAK_TAG_BIND_BLOB 0x06
#define MMBAK_TAG_BIND_BLOB_SHORT 0x07

#define MMBAK_MAGIC "\0dBbAk"
#define MMBAK_VERSION 0x0002

typedef struct mm_backup_header {
    char magic[6];    // file format magic, always MMBAK_MAGIC
    uint16_t version; // file format version, current version is MMBAK_VERSION
} mm_backup_header;

typedef struct mm_backup_section {
    uint32_t flags;    // flags used in mm_backup_init, see MMBAK_FLAG_*
    uint32_t sec_size; // section sizes
} mm_backup_section;

void mm_print_log(mm_logfunc logfunc, int prio, const char *fmt, ...)
#if defined(__GNUC__)
    __attribute__((format(printf, 3, 4)))
#endif
    ;
void mm_print_err(mm_logfunc logfunc, char *errmsg, const char *fmt, ...)
#if defined(__GNUC__)
    __attribute__((format(printf, 3, 4)))
#endif
    ;
void dummy_log(int prio, const char *msg);

#define LOGD(fmt, ...)                                                         \
    mm_print_log(ctx->log, MMBAK_LOG_DEBUG, (fmt), __VA_ARGS__)
#define LOGI(fmt, ...)                                                         \
    mm_print_log(ctx->log, MMBAK_LOG_INFO, (fmt), __VA_ARGS__)
#define LOGE(fmt, ...)                                                         \
    mm_print_log(ctx->log, MMBAK_LOG_ERROR, (fmt), __VA_ARGS__)
#define LOGD_(msg) ctx->log(MMBAK_LOG_DEBUG, (msg));
#define LOGI_(msg) ctx->log(MMBAK_LOG_INFO, (msg));
#define LOGE_(msg) ctx->log(MMBAK_LOG_ERROR, (msg));

#define LOG_AND_FAIL(fmt, args...)                                             \
    do {                                                                       \
        mm_print_err(ctx->log, ctx->errmsg, (fmt), ##args);                    \
        goto bail;                                                             \
    } while (0)

#endif

#ifdef __cplusplus
}
#endif

#endif
