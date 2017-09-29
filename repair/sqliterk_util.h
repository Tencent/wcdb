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

#ifndef sqliterk_util_h
#define sqliterk_util_h

#include <stdio.h>

int sqliterkParseInt(const unsigned char *data,
                     int offset,
                     int length,
                     int *value);
int sqliterkParseInt64(const unsigned char *data,
                       int offset,
                       int length,
                       int64_t *value);
int sqliterkParseVarint(const unsigned char *data,
                        int offset,
                        int *length,
                        int *value);
int sqliterkParseVarint64(const unsigned char *data,
                          int offset,
                          int *length,
                          int64_t *value);
int sqliterkParseNumber(const unsigned char *data, int offset, double *value);
int sqliterkGetMaxVarintLength(void);
const char *sqliterkGetResultCodeDescription(int result);

#endif /* sqliterk_util_h */
