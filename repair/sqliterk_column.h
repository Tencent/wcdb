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

#ifndef sqliterk_column_h
#define sqliterk_column_h

#include "sqliterk_values.h"

typedef struct sqliterk_column sqliterk_column;

int sqliterkColumnAlloc(sqliterk_column **column);
int sqliterkColumnFree(sqliterk_column *column);

sqliterk_values *sqliterkColumnGetValues(sqliterk_column *column);
void sqliterkColumnSetRowId(sqliterk_column *column, int64_t rowid);
int64_t sqliterkColumnGetRowId(sqliterk_column *column);
sqliterk_values *sqliterkColumnGetOverflowPages(sqliterk_column *column);
int sqliterkColumnClear(sqliterk_column *column);

#endif /* sqliterk_column_h */
