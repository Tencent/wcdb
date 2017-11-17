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

#include "sqliterk_column.h"
#include "sqliterk_os.h"

struct sqliterk_column {
    int64_t rowid;
    sqliterk_values *values;
    sqliterk_values *overflowPages;
};

int sqliterkColumnAlloc(sqliterk_column **column)
{
    if (!column) {
        return SQLITERK_MISUSE;
    }
    int rc = SQLITERK_OK;
    sqliterk_column *theColumn = sqliterkOSMalloc(sizeof(sqliterk_column));
    if (!theColumn) {
        rc = SQLITERK_NOMEM;
        goto sqliterkColumnAlloc_Failed;
    }
    rc = sqliterkValuesAlloc(&theColumn->values);
    if (rc != SQLITERK_OK) {
        goto sqliterkColumnAlloc_Failed;
    }
    rc = sqliterkValuesAlloc(&theColumn->overflowPages);
    if (rc != SQLITERK_OK) {
        goto sqliterkColumnAlloc_Failed;
    }
    *column = theColumn;
    return SQLITERK_OK;

sqliterkColumnAlloc_Failed:
    if (theColumn) {
        sqliterkColumnFree(theColumn);
    }
    *column = NULL;
    return rc;
}

int sqliterkColumnFree(sqliterk_column *column)
{
    if (!column) {
        return SQLITERK_MISUSE;
    }
    if (column->overflowPages) {
        sqliterkValuesFree(column->overflowPages);
    }
    if (column->values) {
        sqliterkValuesFree(column->values);
    }
    sqliterkOSFree(column);
    return SQLITERK_OK;
}

sqliterk_values *sqliterkColumnGetValues(sqliterk_column *column)
{
    if (!column) {
        return NULL;
    }
    return column->values;
}

void sqliterkColumnSetRowId(sqliterk_column *column, int64_t rowid)
{
    if (column) {
        column->rowid = rowid;
    }
}

int64_t sqliterkColumnGetRowId(sqliterk_column *column)
{
    if (!column) {
        return 0;
    }
    return column->rowid;
}

sqliterk_values *sqliterkColumnGetOverflowPages(sqliterk_column *column)
{
    if (!column) {
        return NULL;
    }
    return column->overflowPages;
}

int sqliterkColumnClear(sqliterk_column *column)
{
    if (!column) {
        return SQLITERK_MISUSE;
    }
    column->rowid = 0;
    sqliterkValuesClear(column->overflowPages);
    sqliterkValuesClear(column->values);
    return SQLITERK_OK;
}