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

#include "sqliterk_btree.h"
#include "SQLiteRepairKit.h"
#include "sqliterk_column.h"
#include "sqliterk_os.h"
#include "sqliterk_pager.h"
#include "sqliterk_util.h"
#include "sqliterk_values.h"
#include <string.h>

// Declarations
static int sqliterkBtreeParsePage(sqliterk_btree *btree, int pageno);
static int sqliterkBtreeParseCell(sqliterk_btree *btree,
                                  sqliterk_page *page,
                                  const int *cellPointerArray,
                                  const int cellsCount);
static int sqliterkBtreeParsePayload(sqliterk_btree *btree,
                                     sqliterk_page *page,
                                     int offset,
                                     int payloadSize,
                                     sqliterk_column *column);
static int sqliterkBtreeGetLengthForSerialType(int serialType);

struct sqliterk_btree {
    sqliterk *rk;
    char *name;
    sqliterk_btree_type type;
    sqliterk_pager *pager;
    sqliterk_page *rootpage;
    // For leaf-table. See https://www.sqlite.org/fileformat2.html#btree
    int maxLocal;
    int minLocal;
    int maxLeaf;
    int minLeaf;

    sqliterk_btree_notify notify;
    void *userInfo;
};

int sqliterkBtreeOpen(sqliterk *rk,
                      sqliterk_pager *pager,
                      int rootPageno,
                      sqliterk_btree **btree)
{
    if (!pager || !btree) {
        return SQLITERK_MISUSE;
    }
    int rc = SQLITERK_OK;
    sqliterk_btree *theBtree = sqliterkOSMalloc(sizeof(sqliterk_btree));
    if (!theBtree) {
        rc = SQLITERK_NOMEM;
        goto sqliterkBtreeOpen_Failed;
    }
    theBtree->pager = pager;

    rc = sqliterkPageAcquire(theBtree->pager, rootPageno, &theBtree->rootpage);
    if (rc != SQLITERK_OK) {
        goto sqliterkBtreeOpen_Failed;
    }
    if (rootPageno == 1) {
        rc = sqliterkBtreeSetMeta(theBtree, "sqlite_master",
                                  sqliterk_btree_type_master);
        if (rc != SQLITERK_OK) {
            goto sqliterkBtreeOpen_Failed;
        }
    } else {
        switch (sqliterkPageGetType(theBtree->rootpage)) {
            case sqliterk_page_type_interior_index:
            case sqliterk_page_type_leaf_index:
                theBtree->type = sqliterk_btree_type_index;
                break;
            case sqliterk_page_type_interior_table:
            case sqliterk_page_type_leaf_table:
                theBtree->type = sqliterk_btree_type_table;
                break;
            default:
                rc = SQLITERK_DAMAGED;
                goto sqliterkBtreeOpen_Failed;
        }
    }
    // Save memory
    sqliterkPageClearData(theBtree->rootpage);

    theBtree->maxLocal =
        (sqliterkPagerGetUsableSize(theBtree->pager) - 12) * 64 / 255 - 23;
    theBtree->minLocal =
        (sqliterkPagerGetUsableSize(theBtree->pager) - 12) * 32 / 255 - 23;
    theBtree->maxLeaf = sqliterkPagerGetUsableSize(theBtree->pager) - 35;
    theBtree->minLeaf =
        (sqliterkPagerGetUsableSize(theBtree->pager) - 12) * 32 / 255 - 23;

    theBtree->rk = rk;

    *btree = theBtree;
    return SQLITERK_OK;

sqliterkBtreeOpen_Failed:
    if (theBtree) {
        sqliterkBtreeClose(theBtree);
    }
    *btree = NULL;
    return rc;
}

int sqliterkBtreeParse(sqliterk_btree *btree)
{
    if (!btree) {
        return SQLITERK_MISUSE;
    }
    if (btree->notify.onBeginParseBtree) {
        btree->notify.onBeginParseBtree(btree->rk, btree);
    }
    int rc =
        sqliterkBtreeParsePage(btree, sqliterkPageGetPageno(btree->rootpage));
    if (btree->notify.onEndParseBtree) {
        btree->notify.onEndParseBtree(btree->rk, btree, rc);
    }
    return rc;
}

// If the page is an interior-btree, no matter is an interior-table btree
// or an interior-index btree, this function will recursively parse the page
// until it find the leaf page or any error occur.
// A leaf-index btree will only be found but not parse, since its data make
// no sense.
static int sqliterkBtreeParsePage(sqliterk_btree *btree, int pageno)
{
    int i;

    if (!btree || pageno > sqliterkPagerGetPageCount(btree->pager)) {
        return SQLITERK_MISUSE;
    }

    int rc;

    if (btree->notify.onBeginParsePage) {
        rc = btree->notify.onBeginParsePage(btree->rk, btree, pageno);
        if (rc != SQLITERK_OK) {
            return rc;
        }
    }

    int *cellPointerArray = NULL;
    sqliterk_page *page = NULL;

    // ahead checking type to fast up parsing
    rc = sqliterkPageAcquire(btree->pager, pageno, &page);
    if (rc != SQLITERK_OK) {
        goto sqliterkBtreeParsePage_End;
    }
    sqliterk_page_type type = sqliterkPageGetType(page);
    if (type != sqliterk_page_type_interior_index &&
        type != sqliterk_page_type_interior_table &&
        type != sqliterk_page_type_leaf_index &&
        type != sqliterk_page_type_leaf_table) {
        rc = sqliterkOSWarning(SQLITERK_DAMAGED, "Page %d has invalid type",
                               pageno);
        goto sqliterkBtreeParsePage_End;
    }

    //sqliterkOSDebug(SQLITERK_OK, "Page %d is %s", pageno, sqliterkPageGetTypeName(type));

    // Parse cell pointer array. For further information, see [cell pointer]
    // at https://www.sqlite.org/fileformat2.html#btree
    const unsigned char *pagedata = sqliterkPageGetData(page);
    int offsetCellPointerArray =
        (type == sqliterk_page_type_interior_table) ? 12 : 8;
    int cellsCount;
    sqliterkParseInt(pagedata, 3 + sqliterkPageHeaderOffset(page), 2,
                     &cellsCount);
    if (cellsCount <= 0 || cellsCount * 2 + offsetCellPointerArray >
                               sqliterkPagerGetSize(btree->pager)) {
        rc = SQLITERK_DAMAGED;
        goto sqliterkBtreeParsePage_End;
    }
    cellPointerArray = sqliterkOSMalloc(sizeof(int) * (cellsCount + 1));
    if (!cellPointerArray) {
        rc = SQLITERK_NOMEM;
        goto sqliterkBtreeParsePage_End;
    }

    for (i = 0; i < cellsCount; i++) {
        int cellPointer;
        sqliterkParseInt(pagedata,
                         sqliterkPageHeaderOffset(page) +
                             offsetCellPointerArray + i * 2,
                         2, &cellPointer);
        cellPointerArray[i] = cellPointer;
    }

    switch (type) {
        case sqliterk_page_type_interior_table:
        case sqliterk_page_type_interior_index: {
            int hasRightMostPageno =
                (type == sqliterk_page_type_interior_table);
            int pagenosCount = cellsCount + hasRightMostPageno;
            int *pagenos = sqliterkOSMalloc(sizeof(int) * (pagenosCount + 1));
            if (!pagenos) {
                rc = SQLITERK_NOMEM;
                goto sqliterkBtreeParsePage_End;
            }
            for (i = 0; i < cellsCount; i++) {
                sqliterkParseInt(pagedata, cellPointerArray[i], 4, pagenos + i);
            }
            if (hasRightMostPageno) {
                sqliterkParseInt(pagedata, 8, 4, pagenos + cellsCount);
            }
            // All done for page data. Ahead release the page data to avoid memory overflow
            sqliterkOSFree(cellPointerArray);
            cellPointerArray = NULL;
            sqliterkPageClearData(page);
            // Recursively decode the page
            for (i = 0; i < pagenosCount; i++) {
                if (sqliterkBtreeParsePage(btree, pagenos[i]) ==
                    SQLITERK_CANCELLED)
                    break;
            }
            sqliterkOSFree(pagenos);
            break;
        }
        case sqliterk_page_type_leaf_table:
            if (sqliterkBtreeIsSystemType(sqliterkBtreeGetType(btree)) &&
                btree->type != sqliterk_btree_type_master) {
                //skip a non-master system table, since its column is generated.
                goto sqliterkBtreeParsePage_End;
            }
            rc = sqliterkBtreeParseCell(btree, page, cellPointerArray,
                                        cellsCount);
            break;
        case sqliterk_page_type_leaf_index:
            // Just skip it since the column in leaf index make no sense.
            break;
        default:
            break;
    }

sqliterkBtreeParsePage_End:
    if (cellPointerArray) {
        sqliterkOSFree(cellPointerArray);
    }
    if (btree->notify.onEndParsePage) {
        btree->notify.onEndParsePage(btree->rk, btree, pageno, rc);
    }
    if (page) {
        sqliterkPageRelease(page);
    }
    if (rc != SQLITERK_OK && rc != SQLITERK_CANCELLED) {
        sqliterkOSDebug(rc, "Failed to parse page %d.", pageno);
    }
    return rc;
}

// Parse the payload data. see [B-tree Cell Format]
// at https://www.sqlite.org/fileformat2.html#btree
static int sqliterkBtreeParseCell(sqliterk_btree *btree,
                                  sqliterk_page *page,
                                  const int *cellPointerArray,
                                  const int cellsCount)
{
    if (!btree || !page || !cellPointerArray || cellsCount < 0) {
        return SQLITERK_MISUSE;
    }
    const unsigned char *pagedata = sqliterkPageGetData(page);
    int rc = SQLITERK_OK;
    sqliterk_column *column;
    rc = sqliterkColumnAlloc(&column);
    if (rc != SQLITERK_OK) {
        goto sqliterkBtreeParsePayload_End;
    }

    int i;
    for (i = 0; i < cellsCount; i++) {
        sqliterkColumnClear(column);
        int offset = cellPointerArray[i];
        // Find payload
        int payloadSizeLength;
        int payloadSize;
        rc = sqliterkParseVarint(pagedata, offset, &payloadSizeLength,
                                 &payloadSize);
        if (rc != SQLITERK_OK) {
            goto sqliterkBtreeParsePayload_End;
        }
        offset += payloadSizeLength;

        int rowidLength;
        int64_t rowid;
        rc = sqliterkParseVarint64(pagedata, offset, &rowidLength, &rowid);
        if (rc != SQLITERK_OK) {
            goto sqliterkBtreeParsePayload_End;
        }
        offset += rowidLength;
        sqliterkColumnSetRowId(column, rowid);

        rc =
            sqliterkBtreeParsePayload(btree, page, offset, payloadSize, column);
        if (rc != SQLITERK_OK) {
            goto sqliterkBtreeParsePayload_End;
        }
    }
sqliterkBtreeParsePayload_End:
    if (column) {
        sqliterkColumnFree(column);
    }
    if (rc != SQLITERK_OK && rc != SQLITERK_CANCELLED) {
        sqliterkOSDebug(rc, "Failed to parse payload.");
    }
    return rc;
}

// Parse the payload for leaf-table page only. We don't implement the parse
// method for index page, since we are not concerned about the data in an
// index page. See [Record Format] at https://www.sqlite.org/fileformat2.html
static int sqliterkBtreeParsePayload(sqliterk_btree *btree,
                                     sqliterk_page *page,
                                     int offset,
                                     int payloadSize,
                                     sqliterk_column *column)
{
    if (!btree || payloadSize <= 0 || !column) {
        return SQLITERK_MISUSE;
    }
    int rc = SQLITERK_OK;
    unsigned char *payloadData = sqliterkOSMalloc(payloadSize);
    if (!payloadData) {
        rc = SQLITERK_NOMEM;
        goto sqliterkBtreeParseColumn_End;
    }

    // Check overflow
    int local = 0;
    if (payloadSize <= btree->maxLeaf) {
        local = payloadSize;
    } else {
        // Since it is a leaf-table page, the max local should be equal to max leaf
        int maxPageLocal = btree->maxLeaf;
        int minPageLocal = btree->minLocal;
        int surplus =
            minPageLocal + (payloadSize - minPageLocal) %
                               (sqliterkPagerGetUsableSize(btree->pager) - 4);
        if (surplus <= maxPageLocal) {
            local = surplus;
        } else {
            local = minPageLocal;
        }
    }

    // Read data
    int payloadPointer = 0;
    const unsigned char *pagedata = sqliterkPageGetData(page);
    if (offset + local > sqliterkPagerGetSize(btree->pager)) {
        rc = SQLITERK_DAMAGED;
        goto sqliterkBtreeParseColumn_End;
    }
    memcpy(payloadData, pagedata + offset, local);
    payloadPointer += local;

    if (payloadPointer < payloadSize) {
        sqliterk_values *overflowPages = sqliterkColumnGetOverflowPages(column);
        int overflowPageno;
        const unsigned char *pagedata = sqliterkPageGetData(page);
        sqliterkParseInt(pagedata, offset + local, 4, &overflowPageno);
        while (sqliterkPagerIsPagenoValid(btree->pager, overflowPageno) ==
               SQLITERK_OK) {
            sqliterkValuesAddInteger(overflowPages, overflowPageno);
            if (btree->notify.onBeginParsePage) {
                btree->notify.onBeginParsePage(btree->rk, btree,
                                               overflowPageno);
            }
            sqliterk_page *page;
            rc = sqliterkPageAcquireOverflow(btree->pager, overflowPageno,
                                             &page);
            if (btree->notify.onEndParsePage) {
                btree->notify.onEndParsePage(btree->rk, btree, overflowPageno,
                                             rc);
            }
            if (rc != SQLITERK_OK) {
                break;
            }
            // Read data
            int overflowSize = payloadSize - payloadPointer;
            int maxSize = sqliterkPagerGetUsableSize(btree->pager) - 4;
            if (overflowSize > maxSize) {
                overflowSize = maxSize;
            }

            const unsigned char *pageData = sqliterkPageGetData(page);
            memcpy(payloadData + payloadPointer, pageData + 4, overflowSize);
            payloadPointer += overflowSize;
            // Iterate
            sqliterkParseInt(pageData, 0, 4, &overflowPageno);
            // Clear
            sqliterkPageRelease(page);
        }
    }

    int columnOffsetValue = 0;
    int columnOffsetValueLength = 0;
    rc = sqliterkParseVarint(payloadData, 0, &columnOffsetValueLength,
                             &columnOffsetValue);
    if (rc != SQLITERK_OK) {
        goto sqliterkBtreeParseColumn_End;
    }

    int offsetSerialType = columnOffsetValueLength;
    int offsetValue = columnOffsetValue;
    const int endSerialType = offsetValue;
    const int endValue = payloadSize;

    int serialTypeLength = 0;
    int serialType = 0;
    int valueLength = 0;

    sqliterk_values *values = sqliterkColumnGetValues(column);
    while (offsetValue < endValue || offsetSerialType < endSerialType) {
        rc = sqliterkParseVarint(payloadData, offsetSerialType,
                                 &serialTypeLength, &serialType);
        if (rc != SQLITERK_OK) {
            goto sqliterkBtreeParseColumn_End;
        }
        valueLength = sqliterkBtreeGetLengthForSerialType(serialType);
        if (serialType == 0) {
            rc = sqliterkValuesAddNull(values);
        } else if (serialType < 7) {
            int64_t value;
            sqliterkParseInt64(payloadData, offsetValue, valueLength, &value);
            rc = sqliterkValuesAddInteger64(values, value);
        } else if (serialType == 7) {
            double value;
            sqliterkParseNumber(payloadData, offsetValue, &value);
            rc = sqliterkValuesAddNumber(values, value);
        } else if (serialType == 8) {
            rc = sqliterkValuesAddInteger(values, 0);
        } else if (serialType == 9) {
            rc = sqliterkValuesAddInteger(values, 1);
        } else if (serialType >= 12) {
            if (serialType % 2 == 0) {
                rc = sqliterkValuesAddBinary(values, payloadData + offsetValue,
                                             valueLength);
            } else {
                rc = sqliterkValuesAddNoTerminatorText(
                    values, (const char *) payloadData + offsetValue,
                    valueLength);
            }
        } else {
            rc = SQLITERK_DAMAGED;
        }
        if (rc != SQLITERK_OK) {
            goto sqliterkBtreeParseColumn_End;
        }
        offsetValue += valueLength;
        offsetSerialType += serialTypeLength;
    }
    if (offsetSerialType != endSerialType || offsetValue != endValue) {
        rc = SQLITERK_DAMAGED;
        goto sqliterkBtreeParseColumn_End;
    }

sqliterkBtreeParseColumn_End:
    if (rc == SQLITERK_OK && btree->notify.onParseColumn) {
        rc = btree->notify.onParseColumn(btree->rk, btree, page, column);
    }
    if (payloadData) {
        sqliterkOSFree(payloadData);
    }
    return rc;
}

int sqliterkBtreeClose(sqliterk_btree *btree)
{
    if (!btree) {
        return SQLITERK_MISUSE;
    }
    if (btree->name) {
        sqliterkOSFree(btree->name);
        btree->name = NULL;
    }
    if (btree->rootpage) {
        sqliterkPageRelease(btree->rootpage);
        btree->rootpage = NULL;
    }
    btree->pager = NULL;
    btree->userInfo = NULL;
    btree->rk = NULL;
    btree->type = 0;
    sqliterkOSFree(btree);
    return SQLITERK_OK;
}

int sqliterkBtreeSetMeta(sqliterk_btree *btree,
                         const char *name,
                         sqliterk_btree_type type)
{
    if (!btree) {
        return SQLITERK_MISUSE;
    }
    if (btree->name) {
        sqliterkOSFree(btree->name);
        btree->name = NULL;
    }
    if (name) {
        size_t length = strlen(name);
        btree->name = sqliterkOSMalloc(sizeof(char) * (length + 1));
        if (!btree->name) {
            return SQLITERK_NOMEM;
        }
        strncpy(btree->name, name, length);
        // If it's a system btree name, then setup its b-tree type.
        sqliterk_btree_type i;
        for (i = sqliterk_btree_type_system_begin;
             i < sqliterk_btree_type_system_end; i++) {
            const char *typename = sqliterkBtreeGetTypeName(i);
            if (strncmp(btree->name, typename, strlen(typename)) == 0) {
                btree->type = i;
                break;
            }
        }
    } else {
        btree->name = NULL;
    }
    if (!sqliterkBtreeIsSystemType(btree->type) &&
        type != sqliterk_btree_type_unknown) {
        btree->type = type;
    }
    return SQLITERK_OK;
}

const char *sqliterkBtreeGetName(sqliterk_btree *btree)
{
    if (!btree) {
        return NULL;
    }
    return btree->name;
}

sqliterk_btree_type sqliterkBtreeGetType(sqliterk_btree *btree)
{
    if (!btree) {
        return sqliterk_btree_type_unknown;
    }
    return btree->type;
}

int sqliterkBtreeSetType(sqliterk_btree *btree, sqliterk_btree_type type)
{
    if (!btree) {
        return SQLITERK_MISUSE;
    }
    if (sqliterkBtreeIsSystemType(btree->type)) {
        // You can only set the type manually when the type is not a system type
        return SQLITERK_MISUSE;
    }
    btree->type = type;
    return SQLITERK_OK;
}

int sqliterkBtreeIsSystemType(sqliterk_btree_type type)
{
    if (type >= sqliterk_btree_type_system_begin &&
        type < sqliterk_btree_type_system_end) {
        return 1;
    }
    return 0;
}

void sqliterkBtreeSetNotify(sqliterk_btree *btree,
                            sqliterk_btree_notify *notify)
{
    if (!btree || !notify) {
        return;
    }
    btree->notify = *notify;
}

void sqliterkBtreeSetUserInfo(sqliterk_btree *btree, void *userInfo)
{
    if (!btree) {
        return;
    }
    btree->userInfo = userInfo;
}

void *sqliterkBtreeGetUserInfo(sqliterk_btree *btree)
{
    if (!btree) {
        return NULL;
    }
    return btree->userInfo;
}

sqliterk_page *sqliterkBtreeGetRootPage(sqliterk_btree *btree)
{
    if (!btree) {
        return NULL;
    }
    return btree->rootpage;
}

const char *sqliterkBtreeGetTypeName(sqliterk_btree_type type)
{
    char *name;
    switch (type) {
        case sqliterk_btree_type_autoindex:
            name = "sqlite_autoindex";
            break;
        case sqliterk_btree_type_sequence:
            name = "sqlite_sequence";
            break;
        case sqliterk_btree_type_stat:
            name = "sqlite_stat";
            break;
        case sqliterk_btree_type_master:
            name = "sqlite_master";
            break;
        case sqliterk_btree_type_table:
            name = "table";
            break;
        case sqliterk_btree_type_index:
            name = "index";
            break;
        default:
            name = "unknown";
            break;
    }
    return name;
}

// See [Serial Type Codes Of The Record Format]
// at https://www.sqlite.org/fileformat2.html
static int sqliterkBtreeGetLengthForSerialType(int serialType)
{
    if (serialType < 0) {
        return 0;
    }
    static int sqliterk_btree_serialtype_fixlengths[12] = {0, 1, 2, 3, 4, 6,
                                                           8, 8, 0, 0, 0, 0};
    if (serialType < 12) {
        return sqliterk_btree_serialtype_fixlengths[serialType];
    }
    return (serialType - 12 - serialType % 2) / 2;
}