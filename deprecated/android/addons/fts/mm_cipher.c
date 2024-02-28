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

#include "mm_fts.h"
#include <icucompat.h>
#include <openssl/rc4.h>
#include <stdlib.h>
#include <string.h>
#include <unicode/ucnv.h>

typedef struct mm_cipher_context_t {
    int ref_count;

    UConverter *cnv;
    uint8_t key[16];
} mm_cipher_context_t;

static void do_rc4(mm_cipher_context_t *ctx, void *buf, int buf_len)
{
    RC4_KEY rc4_key;

    RC4_set_key(&rc4_key, 16, ctx->key);
    RC4(&rc4_key, (size_t) buf_len, (const unsigned char *) buf,
        (unsigned char *) buf);
}

static void mmenc_func(sqlite3_context *db, int argc, sqlite3_value **argv)
{
    mm_cipher_context_t *ctx;
    const UChar *src;
    int32_t src_len;
    char buf[1024];
    char *dst = buf;
    int32_t dst_len;
    UErrorCode status = U_ZERO_ERROR;
    int arg_type;

    // only accept 1 argument.
    if (argc != 1)
        goto error_misuse;

    // encoding BLOB data type is not supported.
    arg_type = sqlite3_value_type(argv[0]);
    if (arg_type == SQLITE_BLOB)
        goto error_misuse;

    // for data types other than TEXT, just return them.
    if (arg_type != SQLITE_TEXT) {
        sqlite3_result_value(db, argv[0]);
        return;
    }

    ctx = (mm_cipher_context_t *) sqlite3_user_data(db);
    src_len = sqlite3_value_bytes16(argv[0]) / 2;
    src = (const UChar *) sqlite3_value_text16(argv[0]);

    // transform input string to BOCU-1 encoding.
    // try stack buffer first, if it doesn't fit, malloc a new buffer.
    dst_len =
        ucnv_fromUChars(ctx->cnv, dst, sizeof(buf), src, src_len, &status);
    if (status == U_BUFFER_OVERFLOW_ERROR) {
        status = U_ZERO_ERROR;
        dst = (char *) sqlite3_malloc(dst_len);
        dst_len =
            ucnv_fromUChars(ctx->cnv, dst, dst_len, src, src_len, &status);
    }
    if (U_FAILURE(status) && status != U_STRING_NOT_TERMINATED_WARNING) {
        sqlite3_mm_set_last_error(
            "Failed transforming text to internal encoding.");
        goto error_error;
    }

    // encrypt transformed BOCU-1 string.
    do_rc4(ctx, dst, dst_len);

    // return
    sqlite3_result_blob(db, dst, dst_len, SQLITE_TRANSIENT);
    if (dst != buf)
        sqlite3_free(dst);
    return;

error_error:
    if (dst != buf)
        sqlite3_free(dst);
    sqlite3_result_error_code(db, SQLITE_ERROR);
    return;

error_misuse:
    if (dst != buf)
        sqlite3_free(dst);
    sqlite3_result_error_code(db, SQLITE_MISUSE);
    return;
}

static void mmdec_func(sqlite3_context *db, int argc, sqlite3_value **argv)
{
    mm_cipher_context_t *ctx;
    char src_buf[1024];
    char *src = src_buf;
    int32_t src_len;
    UChar buf[512];
    UChar *dst = buf;
    int32_t dst_len;
    UErrorCode status = U_ZERO_ERROR;
    int arg_type;

    // only accept 1 argument.
    if (argc != 1)
        goto error_misuse;

    arg_type = sqlite3_value_type(argv[0]);

    // for data types other than BLOB, just return them.
    if (arg_type != SQLITE_BLOB) {
        sqlite3_result_value(db, argv[0]);
        return;
    }

    ctx = (mm_cipher_context_t *) sqlite3_user_data(db);
    src_len = sqlite3_value_bytes(argv[0]);
    if (src_len > sizeof(src_buf)) {
        src = (char *) sqlite3_malloc(src_len);
        if (!src)
            goto error_error;
    }
    memcpy(src, sqlite3_value_blob(argv[0]), src_len);

    // decrypt transformed BOCU-1 string.
    do_rc4(ctx, src, src_len);

    // transform input string from BOCU-1 encoding.
    // try stack buffer first, if it doesn't fit, malloc a new buffer.
    dst_len = ucnv_toUChars(ctx->cnv, dst, sizeof(buf), src, src_len, &status);
    if (status == U_BUFFER_OVERFLOW_ERROR) {
        status = U_ZERO_ERROR;
        dst = (UChar *) sqlite3_malloc(dst_len * sizeof(UChar));
        dst_len = ucnv_toUChars(ctx->cnv, dst, dst_len, src, src_len, &status);
    }
    if (U_FAILURE(status) && status != U_STRING_NOT_TERMINATED_WARNING) {
        sqlite3_mm_set_last_error(
            "Failed transforming text from internal encoding.");
        goto error_error;
    }

    // return
    sqlite3_result_text16(db, dst, dst_len * sizeof(UChar), SQLITE_TRANSIENT);
    if (src != src_buf)
        sqlite3_free(src);
    if (dst != buf)
        sqlite3_free(dst);
    return;

error_error:
    if (src != src_buf)
        sqlite3_free(src);
    if (dst != buf)
        sqlite3_free(dst);
    sqlite3_result_error_code(db, SQLITE_ERROR);
    return;

error_misuse:
    if (src != src_buf)
        sqlite3_free(src);
    if (dst != buf)
        sqlite3_free(dst);
    sqlite3_result_error_code(db, SQLITE_MISUSE);
    return;
}

static void
mm_cipher_key_func(sqlite3_context *db, int argc, sqlite3_value **argv)
{
    mm_cipher_context_t *ctx;

    // only accept 1 BLOB argument.
    if (argc != 1)
        goto error_misuse;
    if (sqlite3_value_type(argv[0]) != SQLITE_BLOB)
        goto error_misuse;
    if (sqlite3_value_bytes(argv[0]) != 16)
        goto error_misuse;

    ctx = (mm_cipher_context_t *) sqlite3_user_data(db);
    memcpy(ctx->key, sqlite3_value_blob(argv[0]), 16);

    sqlite3_result_null(db);
    return;

error_misuse:
    sqlite3_result_error_code(db, SQLITE_MISUSE);
    return;
}

static void mm_func_destroy(void *ptr)
{
    mm_cipher_context_t *ctx = (mm_cipher_context_t *) ptr;
    if (--ctx->ref_count == 0) {
        ucnv_close(ctx->cnv);
        sqlite3_free(ctx);
    }
}

int sqlite3_register_mm_cipher(sqlite3 *db, const unsigned char *key)
{
    UErrorCode status = U_ZERO_ERROR;
    int sqlite_err;
    mm_cipher_context_t *ctx;

    // XXX: if we failed to load ICU functions, just skip initializing.
    if (!ucnv_openCCSID)
        return SQLITE_OK;

    ctx = sqlite3_malloc(sizeof(mm_cipher_context_t));
    if (!ctx)
        return SQLITE_NOMEM;

    sqlite_err = SQLITE_ERROR;
    // open converter using CCSID instead of converter name to hide "BOCU-1" string.
    ctx->cnv = ucnv_openCCSID(1214, UCNV_IBM, &status);
    if (!ctx->cnv)
        goto error;

    memcpy(ctx->key, key, 16);
    ctx->ref_count = 3;

    sqlite_err =
        sqlite3_create_function_v2(db, "mmenc", 1, SQLITE_ANY, ctx, mmenc_func,
                                   NULL, NULL, mm_func_destroy);
    if (sqlite_err != SQLITE_OK)
        goto error;

    sqlite_err =
        sqlite3_create_function_v2(db, "mmdec", 1, SQLITE_ANY, ctx, mmdec_func,
                                   NULL, NULL, mm_func_destroy);
    if (sqlite_err != SQLITE_OK)
        goto error;

    sqlite_err = sqlite3_create_function_v2(db, "mm_cipher_key", 1, SQLITE_ANY,
                                            ctx, mm_cipher_key_func, NULL, NULL,
                                            mm_func_destroy);
    if (sqlite_err != SQLITE_OK)
        goto error;

    return SQLITE_OK;

error:
    if (ctx)
        sqlite3_free(ctx);
    return sqlite_err;
}

int sqlite3_mm_set_cipher_key(sqlite3 *db, const void *key, int key_len)
{
    sqlite3_stmt *stmt;
    int ret =
        sqlite3_prepare_v2(db, "SELECT mm_cipher_key(?)", -1, &stmt, NULL);
    if (ret != SQLITE_OK)
        return ret;

    sqlite3_bind_blob(stmt, 1, key, key_len, SQLITE_STATIC);
    sqlite3_step(stmt);

    return sqlite3_finalize(stmt);
}
