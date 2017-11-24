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

#include "sqliterk.h"
#include "SQLiteRepairKit.h"
#include "sqliterk_btree.h"
#include "sqliterk_column.h"
#include "sqliterk_os.h"
#include "sqliterk_pager.h"
#include "sqliterk_util.h"
#include "sqliterk_values.h"
#include <string.h>

struct sqliterk {
    sqliterk_pager *pager;
    sqliterk_btree_notify listen;
    sqliterk_notify notify;
    void *userInfo;
    char recursive;
};

//declaration
static void sqliterkNotify_onBeginParseBtree(sqliterk *rk,
                                             sqliterk_btree *btree);
static void
sqliterkNotify_onEndParseBtree(sqliterk *rk, sqliterk_btree *btree, int result);
static int sqliterkNotify_onParseColumn(sqliterk *rk,
                                        sqliterk_btree *btree,
                                        sqliterk_page *page,
                                        sqliterk_column *column);
static int sqliterkNotify_onBeginParsePage(sqliterk *rk,
                                           sqliterk_btree *btree,
                                           int pageno);
static void sqliterkNotify_onEndParsePage(sqliterk *rk,
                                          sqliterk_btree *btree,
                                          int pageno,
                                          int result);
static int sqliterkParseBtree(sqliterk *rk, sqliterk_btree *btree);

int sqliterkOpen(const char *path,
                 const sqliterk_cipher_conf *cipher,
                 sqliterk **rk)
{
    if (!rk) {
        return SQLITERK_MISUSE;
    }
    int rc = SQLITERK_OK;
    sqliterk *therk = sqliterkOSMalloc(sizeof(sqliterk));
    if (!therk) {
        rc = SQLITERK_NOMEM;
        sqliterkOSError(rc, "Not enough memory, required: %zu bytes",
                        sizeof(sqliterk));
        goto sqliterkOpen_Failed;
    }

    rc = sqliterkPagerOpen(path, cipher, &therk->pager);
    if (rc != SQLITERK_OK) {
        goto sqliterkOpen_Failed;
    }

    therk->listen.onBeginParsePage = sqliterkNotify_onBeginParsePage;
    therk->listen.onEndParsePage = sqliterkNotify_onEndParsePage;
    therk->listen.onBeginParseBtree = sqliterkNotify_onBeginParseBtree;
    therk->listen.onEndParseBtree = sqliterkNotify_onEndParseBtree;
    therk->listen.onParseColumn = sqliterkNotify_onParseColumn;
    therk->recursive = 1;

    *rk = therk;
    sqliterkOSInfo(SQLITERK_OK, "RepairKit on '%s' opened, %s.", path,
                   cipher ? "encrypted" : "plain-text");
    return SQLITERK_OK;

sqliterkOpen_Failed:
    if (therk) {
        sqliterkClose(therk);
    }
    *rk = NULL;
    return rc;
}

void sqliterk_set_recursive(sqliterk *rk, int recursive)
{
    rk->recursive = (char) recursive;
}

int sqliterkParse(sqliterk *rk)
{
    if (!rk) {
        return SQLITERK_MISUSE;
    }

    int i;
    for (i = 0; i < sqliterkPagerGetPageCount(rk->pager); i++) {
        int pageno = i + 1;
        sqliterkParsePage(rk, pageno);
    }
    return SQLITERK_OK;
}

int sqliterkParsePage(sqliterk *rk, int pageno)
{
    if (!rk) {
        return SQLITERK_MISUSE;
    }
    if (sqliterkPagerGetStatus(rk->pager, pageno) !=
        sqliterk_status_unchecked) {
        return SQLITERK_OK;
    }
    int rc = SQLITERK_OK;
    sqliterk_btree *btree = NULL;
    rc = sqliterkBtreeOpen(rk, rk->pager, pageno, &btree);
    if (rc != SQLITERK_OK) {
        goto sqliterkParsePage_End;
    }
    rc = sqliterkParseBtree(rk, btree);
sqliterkParsePage_End:
    if (btree) {
        sqliterkBtreeClose(btree);
    }
    return rc;
}

static int sqliterkParseBtree(sqliterk *rk, sqliterk_btree *btree)
{
    if (!rk) {
        return SQLITERK_MISUSE;
    }
    sqliterk_page *page = sqliterkBtreeGetRootPage(btree);
    int pageno = sqliterkPageGetPageno(page);
    if (!page || sqliterkPagerIsPagenoValid(rk->pager, pageno) != SQLITERK_OK) {
        return SQLITERK_MISUSE;
    }
    int rc = SQLITERK_OK;
    sqliterkBtreeSetNotify(btree, &rk->listen);
    rc = sqliterkBtreeParse(btree);
    return rc;
}

int sqliterkParseMaster(sqliterk *rk)
{
    // The page 1 is always sqlite_master. See [B-tree Pages] at
    // https://www.sqlite.org/fileformat2.html
    return sqliterkParsePage(rk, 1);
}

int sqliterkClose(sqliterk *rk)
{
    if (!rk) {
        return SQLITERK_MISUSE;
    }
    if (rk->pager) {
        sqliterkPagerClose(rk->pager);
        rk->pager = NULL;
    }
    sqliterkOSFree(rk);
    return SQLITERK_OK;
}

static void sqliterkNotify_onBeginParseBtree(sqliterk *rk,
                                             sqliterk_btree *btree)
{
    if (rk->notify.onBeginParseTable) {
        rk->notify.onBeginParseTable(rk, (sqliterk_table *) btree);
    }
    sqliterk_page *rootpage = sqliterkBtreeGetRootPage(btree);
    sqliterkOSDebug(
        SQLITERK_OK, "Parsing B-tree -> [root: %d, name: %s, type: %s]",
        sqliterkPageGetPageno(rootpage), sqliterkBtreeGetName(btree),
        sqliterkBtreeGetTypeName(sqliterkBtreeGetType(btree)));
}

static void
sqliterkNotify_onEndParseBtree(sqliterk *rk, sqliterk_btree *btree, int result)
{
    if (rk->notify.onEndParseTable) {
        rk->notify.onEndParseTable(rk, (sqliterk_table *) btree);
    }
}

static int sqliterkNotify_onParseColumn(sqliterk *rk,
                                        sqliterk_btree *btree,
                                        sqliterk_page *page,
                                        sqliterk_column *column)
{
    if (!rk) {
        return SQLITERK_MISUSE;
    }

    int result;
    if (rk->notify.onParseColumn) {
        result = rk->notify.onParseColumn(rk, (sqliterk_table *) btree, column);
        if (result == SQLITERK_CANCELLED) {
            return result;
        } else if (result != SQLITERK_OK) {
            int pageno = sqliterkPageGetPageno(page);
            sqliterkPagerSetStatus(rk->pager, pageno,
                                   sqliterk_status_discarded);
            sqliterk_values *overflowPages =
                sqliterkColumnGetOverflowPages(column);

            int i;
            for (i = 0; i < sqliterkValuesGetCount(overflowPages); i++) {
                sqliterkPagerSetStatus(
                    rk->pager, sqliterkValuesGetInteger(overflowPages, i),
                    sqliterk_status_discarded);
            }
        }
    } else {
        result = SQLITERK_OK;
    }

    if (sqliterkBtreeGetType(btree) == sqliterk_btree_type_master &&
        rk->recursive) {
        // Recursively decode the page since the mapping of [table]->[rootPageno] is known
        sqliterk_values *values = sqliterkColumnGetValues(column);
        const char *type = sqliterkValuesGetText(values, 0);
        const char *name = sqliterkValuesGetText(values, 1);
        int rootPageno = sqliterkValuesGetInteger(values, 3);
        int rc = SQLITERK_OK;
        if (type && name) {
            sqliterk_btree *subbtree;
            rc = sqliterkBtreeOpen(rk, rk->pager, rootPageno, &subbtree);
            if (rc == SQLITERK_OK) {
                if (memcmp("table", type, 5) == 0) {
                    sqliterkBtreeSetMeta(subbtree, name,
                                         sqliterk_btree_type_table);
                } else if (memcmp("index", type, 5) == 0) {
                    sqliterkBtreeSetMeta(subbtree, name,
                                         sqliterk_btree_type_index);
                } else {
                    sqliterkBtreeSetMeta(subbtree, name,
                                         sqliterk_btree_type_unknown);
                }
                rc = sqliterkParseBtree(rk, subbtree);
            }
            if (rc != SQLITERK_OK) {
                sqliterk_page *rootpage = sqliterkBtreeGetRootPage(subbtree);
                sqliterkOSError(
                    rc,
                    "sqliterkNotify_onParseColumn: failed to parse known "
                    "table with root page no. %d, name %s, type %s",
                    sqliterkPageGetPageno(rootpage),
                    sqliterkBtreeGetName(subbtree),
                    sqliterkBtreeGetTypeName(sqliterkBtreeGetType(subbtree)));
            }
            if (subbtree) {
                sqliterkBtreeClose(subbtree);
            }
        }
    }

    return result;
}

static int
sqliterkNotify_onBeginParsePage(sqliterk *rk, sqliterk_btree *btree, int pageno)
{
    //sqliterkOSDebug(SQLITERK_OK, "sqliterkNotify_onBeginParsePage: %d", pageno);
    if (sqliterkPagerGetStatus(rk->pager, pageno) == sqliterk_status_checking) {
        return SQLITERK_MISUSE;
    }
    sqliterkPagerSetStatus(rk->pager, pageno, sqliterk_status_checking);
    return SQLITERK_OK;
}

static void sqliterkNotify_onEndParsePage(sqliterk *rk,
                                          sqliterk_btree *btree,
                                          int pageno,
                                          int result)
{
    if (!rk) {
        return;
    }
    switch (result) {
        case SQLITERK_OK:
            sqliterkPagerSetStatus(rk->pager, pageno, sqliterk_status_checked);
            break;
        case SQLITERK_DAMAGED:
            sqliterkPagerSetStatus(rk->pager, pageno, sqliterk_status_damaged);
            break;
        case SQLITERK_CANCELLED:
            sqliterkOSDebug(SQLITERK_CANCELLED, "Cancelled parsing page %d.",
                            pageno);
            break;
        default:
            sqliterkOSWarning(SQLITERK_MISUSE,
                              "Cannot parse page %d. Invalid type.", pageno);
            sqliterkPagerSetStatus(rk->pager, pageno, sqliterk_status_invalid);
            break;
    }
    //sqliterkOSDebug(result, "sqliterkNotify_onEndParsePage: %d", pageno);
    if (rk->notify.didParsePage) {
        rk->notify.didParsePage(rk, pageno);
    }
}

int sqliterkSetNotify(sqliterk *rk, sqliterk_notify notify)
{
    if (!rk) {
        return SQLITERK_MISUSE;
    }
    rk->notify = notify;
    return SQLITERK_OK;
}

int sqliterkSetUserInfo(sqliterk *rk, void *userInfo)
{
    if (!rk) {
        return SQLITERK_MISUSE;
    }
    rk->userInfo = userInfo;
    return SQLITERK_OK;
}

void *sqliterkGetUserInfo(sqliterk *rk)
{
    if (!rk) {
        return NULL;
    }
    return rk->userInfo;
}

int sqliterkGetParsedPageCount(sqliterk *rk)
{
    if (!rk) {
        return 0;
    }
    return sqliterkPagerGetParsedPageCount(rk->pager);
}

int sqliterkGetValidPageCount(sqliterk *rk)
{
    if (!rk) {
        return 0;
    }
    return sqliterkPagerGetValidPageCount(rk->pager);
}

int sqliterkGetPageCount(sqliterk *rk)
{
    if (!rk) {
        return 0;
    }
    return sqliterkPagerGetPageCount(rk->pager);
}

unsigned int sqliterkGetIntegrity(sqliterk *rk)
{
    if (!rk) {
        return 0;
    }
    return sqliterkPagerGetIntegrity(rk->pager);
}
