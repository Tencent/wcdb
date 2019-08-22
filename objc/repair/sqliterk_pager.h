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

#ifndef sqliterk_pager_h
#define sqliterk_pager_h

#include "sqliterk_crypto.h"
#include <stdio.h>

// A legal state transfer is:
// unchecked --> checking
// checking --> invalid
// checking --> damaged
// checking --> checked
// checking --> discarded
// default status is unchecked
typedef enum {
    sqliterk_status_invalid = -1,
    sqliterk_status_unchecked = 0,
    sqliterk_status_checking = 1,
    sqliterk_status_damaged = 2,
    sqliterk_status_discarded = 3,
    sqliterk_status_checked = 4,
} sqliterk_status;

// We are only concerned about the listed types. See https://www.sqlite.org/fileformat2.html#btree
typedef enum {
    sqliterk_page_type_interior_index = 2,
    sqliterk_page_type_interior_table = 5,
    sqliterk_page_type_leaf_index = 10,
    sqliterk_page_type_leaf_table = 13,
    sqliterk_page_type_overflow = 1,
    sqliterk_page_type_unknown = -1,
} sqliterk_page_type;

// sqliterk_pager is the management class for all page in single db file
typedef struct sqliterk_pager sqliterk_pager;
// sqliterk_page is single page in db file, including its page data. But sometime, page data will be ahead release to save memory.
typedef struct sqliterk_page sqliterk_page;

typedef struct sqliterk_file sqliterk_file;
typedef struct sqliterk_cipher_conf sqliterk_cipher_conf;
struct sqliterk_pager {
    sqliterk_file *file;
    sqliterk_status *pagesStatus;
    int pagesize;
    int freepagecount;
    int reservedBytes;
    int pagecount;
    int usableSize;         // pagesize-reservedBytes
    unsigned int integrity; // integrity flags.

    sqliterk_codec *codec; // Codec context, implemented in SQLCipher library.
};

int sqliterkPagerOpen(const char *path,
                      const sqliterk_cipher_conf *cipher,
                      sqliterk_pager **pager);
int sqliterkPagerClose(sqliterk_pager *pager);
int sqliterkPagerGetPageCount(sqliterk_pager *pager);
int sqliterkPagerIsPagenoValid(sqliterk_pager *pager, int pageno);
int sqliterkPagerGetSize(sqliterk_pager *pager);
int sqliterkPagerGetUsableSize(sqliterk_pager *pager);

void sqliterkPagerSetStatus(sqliterk_pager *pager,
                            int pageno,
                            sqliterk_status status);
sqliterk_status sqliterkPagerGetStatus(sqliterk_pager *pager, int pageno);
int sqliterkPagerGetParsedPageCount(sqliterk_pager *pager);
int sqliterkPagerGetValidPageCount(sqliterk_pager *pager);
unsigned int sqliterkPagerGetIntegrity(sqliterk_pager *pager);

int sqliterkPageAcquire(sqliterk_pager *pager,
                        int pageno,
                        sqliterk_page **page);
int sqliterkPageAcquireOverflow(sqliterk_pager *pager,
                                int pageno,
                                sqliterk_page **page);
int sqliterkPageAcquireType(sqliterk_pager *pager,
                            int pageno,
                            sqliterk_page_type *type);
int sqliterkPageClearData(sqliterk_page *page);
int sqliterkPageRelease(sqliterk_page *page);

int sqliterkPagenoHeaderOffset(int pageno);
int sqliterkPageHeaderOffset(sqliterk_page *page);

const unsigned char *sqliterkPageGetData(sqliterk_page *page);

int sqliterkPageGetPageno(sqliterk_page *page);
sqliterk_page_type sqliterkPageGetType(sqliterk_page *page);

const char *sqliterkPageGetTypeName(sqliterk_page_type type);

#endif /* sqliterk_pager_h */
