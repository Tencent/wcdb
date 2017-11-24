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

#ifndef sqliterk_btree_h
#define sqliterk_btree_h

// About sqliterk_btree, see https://www.sqlite.org/fileformat2.html#btree
typedef struct sqliterk_btree sqliterk_btree;
typedef struct sqliterk_pager sqliterk_pager;
typedef struct sqliterk_values sqliterk_values;
typedef struct sqliterk_column sqliterk_column;
typedef struct sqliterk_page sqliterk_page;
typedef struct sqliterk sqliterk;
typedef struct sqliterk_btree_notify sqliterk_btree_notify;

typedef enum {
    sqliterk_btree_type_index = -2,
    sqliterk_btree_type_table = -1,
    sqliterk_btree_type_unknown = 0,
    // About SQLite reserved btree, see [Storage Of The SQL Database Schema]
    // chapter at https://www.sqlite.org/fileformat2.html#Schema
    sqliterk_btree_type_system_begin = 1,
    sqliterk_btree_type_sequence = 1,
    sqliterk_btree_type_autoindex = 2,
    sqliterk_btree_type_stat = 3,
    sqliterk_btree_type_master = 4,
    sqliterk_btree_type_system_end = 5,
} sqliterk_btree_type;

struct sqliterk_btree_notify {
    void (*onBeginParseBtree)(sqliterk *rk, sqliterk_btree *btree);
    void (*onEndParseBtree)(sqliterk *rk, sqliterk_btree *btree, int result);
    int (*onParseColumn)(sqliterk *rk,
                         sqliterk_btree *btree,
                         sqliterk_page *page,
                         sqliterk_column *column);

    // return SQLITE_OK to continue parsing the page. All other return
    // value will skip the parsing phase of this page.
    int (*onBeginParsePage)(sqliterk *rk, sqliterk_btree *btree, int pageno);

    void (*onEndParsePage)(sqliterk *rk,
                           sqliterk_btree *btree,
                           int pageno,
                           int result);
};

int sqliterkBtreeOpen(sqliterk *rk,
                      sqliterk_pager *pager,
                      int rootPageno,
                      sqliterk_btree **btree);
int sqliterkBtreeParse(sqliterk_btree *btree);
int sqliterkBtreeClose(sqliterk_btree *btree);

int sqliterkBtreeSetMeta(sqliterk_btree *btree,
                         const char *name,
                         sqliterk_btree_type type);
const char *sqliterkBtreeGetName(sqliterk_btree *btree);
sqliterk_btree_type sqliterkBtreeGetType(sqliterk_btree *btree);
int sqliterkBtreeIsSystemType(sqliterk_btree_type type);
sqliterk_page *sqliterkBtreeGetRootPage(sqliterk_btree *btree);

void sqliterkBtreeSetNotify(sqliterk_btree *btree,
                            sqliterk_btree_notify *notify);
void sqliterkBtreeSetUserInfo(sqliterk_btree *btree, void *userInfo);
void *sqliterkBtreeGetUserInfo(sqliterk_btree *btree);
const char *sqliterkBtreeGetTypeName(sqliterk_btree_type type);

#endif /* sqliterk_btree_h */
