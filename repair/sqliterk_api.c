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

#include "SQLiteRepairKit.h"
#include "sqliterk.h"
#include "sqliterk_btree.h"
#include "sqliterk_column.h"
#include "sqliterk_os.h"
#include "sqliterk_pager.h"
#include "sqliterk_util.h"
#include "sqliterk_values.h"

int sqliterk_register_notify(sqliterk *rk, sqliterk_notify notify)
{
    return sqliterkSetNotify(rk, notify);
}

int sqliterk_open(const char *path,
                  const sqliterk_cipher_conf *cipher,
                  sqliterk **rk)
{
    return sqliterkOpen(path, cipher, rk);
}

void *sqliterk_user_info(sqliterk *rk)
{
    return sqliterkGetUserInfo(rk);
}

int sqliterk_parse(sqliterk *rk)
{
    return sqliterkParse(rk);
}

int sqliterk_parse_page(sqliterk *rk, int pageno)
{
    return sqliterkParsePage(rk, pageno);
}

int sqliterk_parse_master(sqliterk *rk)
{
    return sqliterkParseMaster(rk);
}

int sqliterk_close(sqliterk *rk)
{
    return sqliterkClose(rk);
}

void *sqliterk_get_user_info(sqliterk *rk)
{
    return sqliterkGetUserInfo(rk);
}

void sqliterk_set_user_info(sqliterk *rk, void *userInfo)
{
    sqliterkSetUserInfo(rk, userInfo);
}

int sqliterk_column_count(sqliterk_column *column)
{
    return sqliterkValuesGetCount(sqliterkColumnGetValues(column));
}

sqliterk_value_type sqliterk_column_type(sqliterk_column *column, int index)
{
    return sqliterkValuesGetType(sqliterkColumnGetValues(column), index);
}

int sqliterk_column_integer(sqliterk_column *column, int index)
{
    return sqliterkValuesGetInteger(sqliterkColumnGetValues(column), index);
}

int64_t sqliterk_column_integer64(sqliterk_column *column, int index)
{
    return sqliterkValuesGetInteger64(sqliterkColumnGetValues(column), index);
}

double sqliterk_column_number(sqliterk_column *column, int index)
{
    return sqliterkValuesGetNumber(sqliterkColumnGetValues(column), index);
}

const char *sqliterk_column_text(sqliterk_column *column, int index)
{
    return sqliterkValuesGetText(sqliterkColumnGetValues(column), index);
}

const void *sqliterk_column_binary(sqliterk_column *column, int index)
{
    return sqliterkValuesGetBinary(sqliterkColumnGetValues(column), index);
}

int sqliterk_column_bytes(sqliterk_column *column, int index)
{
    return sqliterkValuesGetBytes(sqliterkColumnGetValues(column), index);
}

int64_t sqliterk_column_rowid(sqliterk_column *column)
{
    return sqliterkColumnGetRowId(column);
}

const char *sqliterk_table_name(sqliterk_table *table)
{
    return sqliterkBtreeGetName((sqliterk_btree *) table);
}

void sqliterk_table_set_user_info(sqliterk_table *table, void *userInfo)
{
    sqliterkBtreeSetUserInfo((sqliterk_btree *) table, userInfo);
}

void *sqliterk_table_get_user_info(sqliterk_table *table)
{
    return sqliterkBtreeGetUserInfo((sqliterk_btree *) table);
}

sqliterk_type sqliterk_table_type(sqliterk_table *table)
{
    return (sqliterk_type) sqliterkBtreeGetType((sqliterk_btree *) table);
}

int sqliterk_register(sqliterk_os os)
{
    return sqliterkOSRegister(os);
}

int sqliterk_table_root(sqliterk_table *table)
{
    sqliterk_page *page = sqliterkBtreeGetRootPage((sqliterk_btree *) table);
    return sqliterkPageGetPageno(page);
}

const char *sqliterk_description(int result)
{
    return sqliterkGetResultCodeDescription(result);
}

int sqliterk_parsed_page_count(sqliterk *rk)
{
    return sqliterkGetParsedPageCount(rk);
}

int sqliterk_valid_page_count(sqliterk *rk)
{
    return sqliterkGetValidPageCount(rk);
}

int sqliterk_page_count(sqliterk *rk)
{
    return sqliterkGetPageCount(rk);
}

unsigned int sqliterk_integrity(sqliterk *rk)
{
    return sqliterkGetIntegrity(rk);
}
