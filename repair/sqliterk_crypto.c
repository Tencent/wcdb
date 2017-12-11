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

#include "sqliterk_crypto.h"
#include "SQLiteRepairKit.h"
#include "sqliterk_os.h"
#include "sqliterk_pager.h"
#ifdef WCDB_BUILTIN_SQLCIPHER
#ifdef WCDB_COCOAPODS
#include "sqlite3.h"
#else //WCDB_COCOAPODS
#include <sqlcipher/sqlite3.h>
#endif //WCDB_COCOAPODS
#else  //WCDB_BUILTIN_SQLCIPHER
#include <sqlite3.h>
#endif //WCDB_BUILTIN_SQLCIPHER
#include <string.h>

// Declarations by SQLCipher.
#define CIPHER_DECRYPT 0
#define CIPHER_ENCRYPT 1

#define CIPHER_READ_CTX 0
#define CIPHER_WRITE_CTX 1
#define CIPHER_READWRITE_CTX 2

/* Extensions defined in crypto_impl.c */
typedef struct codec_ctx codec_ctx;

/* Activation and initialization */
void sqlcipher_activate(void);
void sqlcipher_deactivate(void);
int sqlcipher_codec_ctx_init(
    codec_ctx **, void *, void *, void *, const void *, int);
void sqlcipher_codec_ctx_free(codec_ctx **);
int sqlcipher_codec_key_derive(codec_ctx *);
int sqlcipher_codec_key_copy(codec_ctx *, int);

/* Page cipher implementation */
int sqlcipher_page_cipher(
    codec_ctx *, int, int, int, int, unsigned char *, unsigned char *);

/* Context setters & getters */
//void sqlcipher_codec_ctx_set_error(codec_ctx *, int);

int sqlcipher_codec_ctx_set_pass(codec_ctx *, const void *, int, int);
void sqlcipher_codec_get_keyspec(codec_ctx *, void **zKey, int *nKey);

int sqlcipher_codec_ctx_set_pagesize(codec_ctx *, int);
int sqlcipher_codec_ctx_get_pagesize(codec_ctx *);
int sqlcipher_codec_ctx_get_reservesize(codec_ctx *);

void sqlcipher_set_default_pagesize(int page_size);
int sqlcipher_get_default_pagesize(void);

void sqlcipher_set_default_kdf_iter(int iter);
int sqlcipher_get_default_kdf_iter(void);

int sqlcipher_codec_ctx_set_kdf_iter(codec_ctx *, int, int);
int sqlcipher_codec_ctx_get_kdf_iter(codec_ctx *ctx, int);

void *sqlcipher_codec_ctx_get_kdf_salt(codec_ctx *ctx);

int sqlcipher_codec_ctx_set_fast_kdf_iter(codec_ctx *, int, int);
int sqlcipher_codec_ctx_get_fast_kdf_iter(codec_ctx *, int);

int sqlcipher_codec_ctx_set_cipher(codec_ctx *, const char *, int);
const char *sqlcipher_codec_ctx_get_cipher(codec_ctx *ctx, int for_ctx);

void *sqlcipher_codec_ctx_get_data(codec_ctx *);

//void sqlcipher_exportFunc(sqlite3_context *, int, sqlite3_value **);

void sqlcipher_set_default_use_hmac(int use);
int sqlcipher_get_default_use_hmac(void);

void sqlcipher_set_hmac_salt_mask(unsigned char mask);
unsigned char sqlcipher_get_hmac_salt_mask(void);

int sqlcipher_codec_ctx_set_use_hmac(codec_ctx *ctx, int use);
int sqlcipher_codec_ctx_get_use_hmac(codec_ctx *ctx, int for_ctx);

int sqlcipher_codec_ctx_set_flag(codec_ctx *ctx, unsigned int flag);
int sqlcipher_codec_ctx_unset_flag(codec_ctx *ctx, unsigned int flag);
int sqlcipher_codec_ctx_get_flag(codec_ctx *ctx,
                                 unsigned int flag,
                                 int for_ctx);

const char *sqlcipher_codec_get_cipher_provider(codec_ctx *ctx);
//int sqlcipher_codec_ctx_migrate(codec_ctx *ctx);
int sqlcipher_codec_add_random(codec_ctx *ctx, const char *data, int random_sz);
int sqlcipher_cipher_profile(sqlite3 *db, const char *destination);
//static void sqlcipher_profile_callback(void *file, const char *sql, sqlite3_uint64 run_time);
//static int sqlcipher_codec_get_store_pass(codec_ctx *ctx);
//static void sqlcipher_codec_get_pass(codec_ctx *ctx, void **zKey, int *nKey);
//static void sqlcipher_codec_set_store_pass(codec_ctx *ctx, int value);
int sqlcipher_codec_fips_status(codec_ctx *ctx);
const char *sqlcipher_codec_get_provider_version(codec_ctx *ctx);

// sqlite3_file redirector
typedef struct {
    const struct sqlite3_io_methods *pMethods;
    sqliterk_file *fd;
    const unsigned char *kdf_salt;
} sqlite3_file_rkredir;

int sqliterkRead(sqlite3_file *fd, void *data, int iAmt, sqlite3_int64 iOfst)
{
    sqlite3_file_rkredir *rkos = (sqlite3_file_rkredir *) fd;
    if (rkos->kdf_salt) {
        memcpy(data, rkos->kdf_salt, (iAmt > 16) ? 16 : iAmt);
        return SQLITE_OK;
    } else {
        sqliterk_file *f = rkos->fd;
        size_t size = iAmt;
        return sqliterkOSRead(f, (off_t) iOfst, data, &size);
    }
}

int sqliterkCryptoSetCipher(sqliterk_pager *pager,
                            sqliterk_file *fd,
                            const sqliterk_cipher_conf *conf)
{
    codec_ctx *codec = NULL;
    int rc;

    if (conf) {
        // Check arguments.
        if (!conf->key || conf->key_len <= 0)
            return SQLITERK_MISUSE;

        // SQLite library must be initialized before calling sqlcipher_activate(),
        // or it will cause a deadlock.
        sqlite3_initialize();
        sqlcipher_activate();

        // XXX: fake BTree structure passed to sqlcipher_codec_ctx_init.
        // Member of such structure is assigned but never used by repair kit.
        int fake_db[8];

        sqlite3_file_rkredir file;
        struct sqlite3_io_methods methods = {0};
        methods.xRead = sqliterkRead;
        file.pMethods = &methods;
        file.fd = fd;
        file.kdf_salt = conf->kdf_salt;

        // Initialize codec context.
        rc = sqlcipher_codec_ctx_init(&codec, fake_db, NULL, &file, conf->key,
                                      conf->key_len);
        if (rc != SQLITE_OK)
            goto bail_sqlite_errstr;

        // Set cipher.
        if (conf->cipher_name) {
            rc = sqlcipher_codec_ctx_set_cipher(codec, conf->cipher_name,
                                                CIPHER_READWRITE_CTX);
            if (rc != SQLITE_OK)
                goto bail_sqlite_errstr;
        }

        // Set page size.
        if (conf->page_size > 0) {
            rc = sqlcipher_codec_ctx_set_pagesize(codec, conf->page_size);
            if (rc != SQLITE_OK)
                goto bail_sqlite_errstr;
        }

        // Set HMAC usage.
        if (conf->use_hmac >= 0) {
            rc = sqlcipher_codec_ctx_set_use_hmac(codec, conf->use_hmac);
            if (rc != SQLITE_OK)
                goto bail_sqlite_errstr;
        }

        // Set KDF Iteration.
        if (conf->kdf_iter > 0) {
            rc = sqlcipher_codec_ctx_set_kdf_iter(codec, conf->kdf_iter,
                                                  CIPHER_READWRITE_CTX);
            if (rc != SQLITE_OK)
                goto bail;
        }

        // Update pager page size.
        int page_sz = sqlcipher_codec_ctx_get_pagesize(codec);
        int reserve_sz = sqlcipher_codec_ctx_get_reservesize(codec);

        pager->pagesize = page_sz;
        pager->reservedBytes = reserve_sz;
    }

    if (pager->codec) {
        sqlcipher_codec_ctx_free(&pager->codec);
        sqlcipher_deactivate();
    }

    pager->codec = codec;
    return SQLITERK_OK;

bail_sqlite_errstr:
    sqliterkOSError(SQLITERK_CANTOPEN,
                    "Failed to initialize cipher context: %s",
                    sqlite3_errstr(rc));
    rc = SQLITERK_CANTOPEN;
bail:
    if (codec)
        sqlcipher_codec_ctx_free(&codec);
    sqlcipher_deactivate();
    return rc;
}

void sqliterkCryptoFreeCodec(sqliterk_pager *pager)
{
    if (!pager->codec)
        return;
    sqlcipher_codec_ctx_free(&pager->codec);
    sqlcipher_deactivate();
}

int sqliterkCryptoDecode(sqliterk_codec *codec, int pgno, void *data)
{
    int rc;
    int offset = 0;
    unsigned char *pdata = (unsigned char *) data;

    int page_sz = sqlcipher_codec_ctx_get_pagesize(codec);
    unsigned char *buffer =
        (unsigned char *) sqlcipher_codec_ctx_get_data(codec);

    rc = sqlcipher_codec_key_derive(codec);
    if (rc != SQLITE_OK)
        return rc;

    if (pgno == 1) {
        offset = 16; // FILE_HEADER_SZ
        memcpy(buffer, "SQLite format 3", 16);
    }
    rc = sqlcipher_page_cipher(codec, CIPHER_READ_CTX, pgno, CIPHER_DECRYPT,
                               page_sz - offset, pdata + offset,
                               buffer + offset);
    if (rc != SQLITE_OK)
        goto bail;
    memcpy(pdata, buffer, page_sz);

    return SQLITERK_OK;

bail:
    sqliterkOSError(SQLITERK_DAMAGED, "Failed to decode page %d: %s", pgno,
                    sqlite3_errstr(rc));
    return rc;
}
