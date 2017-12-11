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

#ifndef SQLite_Bridging_h
#define SQLite_Bridging_h

#ifdef WCDB_COCOAPODS
#import "fts3_tokenizer.h"
#import "sqlite3.h"
#else //WCDB_COCOAPODS
#import <sqlcipher/fts3_tokenizer.h>
#import <sqlcipher/sqlite3.h>
#endif //WCDB_COCOAPODS

struct Tokenizer {
    sqlite3_tokenizer base;
    void *info;
};
typedef struct Tokenizer Tokenizer;

struct Cursor {
    sqlite3_tokenizer_cursor base;
    void *info;
};
typedef struct Cursor Cursor;

typedef struct sqlite3_stmt sqlite3_stmt;

int sqlite3_bind_text_transient(sqlite3_stmt *, int, const char *, int);

int sqlite3_bind_blob_transient(sqlite3_stmt *, int, const void *, int n);

int sqlite3_config_multithread(void);

int sqlite3_config_memstatus(int);

typedef void (*sqlite3_global_log)(void *, int, const char *);

int sqlite3_config_log(sqlite3_global_log, void *);

#endif /* SQLite_Bridging_h */
