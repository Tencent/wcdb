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

#ifndef SQLiteRepairKit_h
#define SQLiteRepairKit_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

typedef struct sqliterk sqliterk;
typedef struct sqliterk_table sqliterk_table;
typedef struct sqliterk_column sqliterk_column;

typedef struct sqliterk_notify sqliterk_notify;
struct sqliterk_notify {
    void (*onBeginParseTable)(sqliterk *rk, sqliterk_table *table);
    // Only a column that make sense will trigger this callback,
    // which is the column in a non-system table or the "sqlite_master"
    // return SQLITERK_OK to tell sqliterk that you already know that
    // meaning of this column
    int (*onParseColumn)(sqliterk *rk,
                         sqliterk_table *table,
                         sqliterk_column *column);
    void (*onEndParseTable)(sqliterk *rk, sqliterk_table *table);
    void (*didParsePage)(sqliterk *rk, int pageno);
};
int sqliterk_register_notify(sqliterk *rk, sqliterk_notify notify);

typedef struct sqliterk_cipher_conf {
    const void *key;
    int key_len;
    const char *cipher_name;
    int page_size;
    int kdf_iter;
    int use_hmac;
    const unsigned char *kdf_salt;
} sqliterk_cipher_conf;

typedef struct sqlite3 sqlite3;
typedef struct sqliterk_master_info sqliterk_master_info;

#define SQLITERK_OUTPUT_NO_CREATE_TABLES 0x0001
#define SQLITERK_OUTPUT_ALL_TABLES 0x0002

int sqliterk_open(const char *path,
                  const sqliterk_cipher_conf *cipher,
                  sqliterk **rk);
int sqliterk_parse(sqliterk *rk);
int sqliterk_parse_page(sqliterk *rk, int pageno);
int sqliterk_parse_master(sqliterk *rk);
int sqliterk_close(sqliterk *rk);
void *sqliterk_get_user_info(sqliterk *rk);
void sqliterk_set_user_info(sqliterk *rk, void *userInfo);
void sqliterk_set_recursive(sqliterk *rk, int recursive);

int sqliterk_output(sqliterk *rk,
                    sqlite3 *db,
                    sqliterk_master_info *master,
                    unsigned int flags);
int sqliterk_output_cb(sqliterk *rk,
                       sqlite3 *db,
                       sqliterk_master_info *master,
                       unsigned int flags,
                       int (*callback)(void *user,
                                       sqliterk *rk,
                                       sqliterk_table *table,
                                       sqliterk_column *column),
                       void *user);
void sqliterk_cancel(sqliterk *rk);
int sqliterk_make_master(const char **tables,
                         int num_tables,
                         sqliterk_master_info **out_master);
int sqliterk_save_master(sqlite3 *db,
                         const char *path,
                         const void *key,
                         int key_len);
int sqliterk_load_master(const char *path,
                         const void *key,
                         int key_len,
                         const char **tables,
                         int num_tables,
                         sqliterk_master_info **out_master,
                         unsigned char *out_kdf_salt);
void sqliterk_free_master(sqliterk_master_info *master);

// A database may have many kind of tables or indexes, such as a customized
// index or a system-level table and so on. But you should be only concern
// about the listed types below.
// Since the system-level tables or indexes is generated. And you do know
// the index of a certain table (you make this table).
typedef enum {
    sqliterk_type_index = -2,
    sqliterk_type_table = -1,
    sqliterk_type_unknown = 0,
    sqliterk_type_sequence = 1,
    sqliterk_type_autoindex = 2,
    sqliterk_type_stat = 3,
    sqliterk_type_master = 4,
} sqliterk_type;

// This method may return NULL since SQLiteRepairKir may not understand
// a corrupted b-tree.
const char *sqliterk_table_name(sqliterk_table *table);
sqliterk_type sqliterk_table_type(sqliterk_table *table);
int sqliterk_table_root(sqliterk_table *table);
void sqliterk_table_set_user_info(sqliterk_table *table, void *userInfo);
void *sqliterk_table_get_user_info(sqliterk_table *table);

typedef enum {
    sqliterk_value_type_null,
    sqliterk_value_type_integer,
    sqliterk_value_type_number,
    sqliterk_value_type_text,
    sqliterk_value_type_binary,
} sqliterk_value_type;

int sqliterk_column_count(sqliterk_column *column);
sqliterk_value_type sqliterk_column_type(sqliterk_column *column, int index);
int sqliterk_column_integer(sqliterk_column *column, int index);
int64_t sqliterk_column_integer64(sqliterk_column *column, int index);
double sqliterk_column_number(sqliterk_column *column, int index);
const char *sqliterk_column_text(sqliterk_column *column, int index);
const void *sqliterk_column_binary(sqliterk_column *column, int index);
int sqliterk_column_bytes(sqliterk_column *column, int index);
int64_t sqliterk_column_rowid(sqliterk_column *column);

#define SQLITERK_INTEGRITY_HEADER 0x0001
#define SQLITERK_INTEGRITY_DATA 0x0002
#define SQLITERK_INTEGRITY_KDF_SALT 0x0004

int sqliterk_parsed_page_count(sqliterk *rk);
int sqliterk_valid_page_count(sqliterk *rk);
int sqliterk_page_count(sqliterk *rk);
unsigned int sqliterk_integrity(sqliterk *rk);

typedef enum {
    sqliterk_loglevel_debug,
    sqliterk_loglevel_warning,
    sqliterk_loglevel_error,
    sqliterk_loglevel_info,
} sqliterk_loglevel;

typedef struct sqliterk_os sqliterk_os;
struct sqliterk_os {
    void (*xLog)(sqliterk_loglevel level, int result, const char *msg);
    //TODO
};
int sqliterk_register(sqliterk_os os);

#define SQLITERK_OK 0
#define SQLITERK_CANTOPEN 1
#define SQLITERK_MISUSE 2
#define SQLITERK_IOERR 3
#define SQLITERK_NOMEM 4
#define SQLITERK_SHORT_READ 5
#define SQLITERK_DAMAGED 6
#define SQLITERK_DISCARD 7
#define SQLITERK_CANCELLED 8
#define SQLITERK_IGNORE 100
const char *sqliterk_description(int result);

#ifndef SQLITRK_CONFIG_DEFAULT_PAGESIZE
#define SQLITRK_CONFIG_DEFAULT_PAGESIZE 4096
#endif

#ifdef __cplusplus
}
#endif

#endif /* SQLiteRepairKit_h */
