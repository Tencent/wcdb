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

#ifndef sqliterk_values_h
#define sqliterk_values_h

#include "SQLiteRepairKit.h"
#include <stdint.h>

// sqliterk_values is a kind of [Any] array.
typedef struct sqliterk_values sqliterk_values;
typedef struct sqliterk_value sqliterk_value;

int sqliterkValuesAlloc(sqliterk_values **values);
int sqliterkValuesFree(sqliterk_values *values);
int sqliterkValuesClear(sqliterk_values *values);

int sqliterkValuesAddInteger(sqliterk_values *values, int i);
int sqliterkValuesAddInteger64(sqliterk_values *values, int64_t i);
int sqliterkValuesAddNumber(sqliterk_values *values, double d);
int sqliterkValuesAddText(sqliterk_values *values, const char *t);
int sqliterkValuesAddNoTerminatorText(sqliterk_values *values,
                                      const char *t,
                                      const int s);
int sqliterkValuesAddBinary(sqliterk_values *values,
                            const void *b,
                            const int s);
int sqliterkValuesAddNull(sqliterk_values *values);

int sqliterkValuesGetCount(sqliterk_values *values);
sqliterk_value_type sqliterkValuesGetType(sqliterk_values *values, int index);
int sqliterkValuesGetInteger(sqliterk_values *values, int index);
int64_t sqliterkValuesGetInteger64(sqliterk_values *values, int index);
double sqliterkValuesGetNumber(sqliterk_values *values, int index);
const char *sqliterkValuesGetText(sqliterk_values *values, int index);
const void *sqliterkValuesGetBinary(sqliterk_values *values, int index);
int sqliterkValuesGetBytes(sqliterk_values *values, int index);

int sqliterkValueClear(sqliterk_value *value);

#endif /* sqliterk_values_h */
