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

#ifndef sqliterk_os_h
#define sqliterk_os_h

#include "SQLiteRepairKit.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct sqliterk_file sqliterk_file;
// sqliterk_os is the virtual layer to fit different os or platform.
// TODO
typedef struct sqliterk_os sqliterk_os;

// memory allocation and set the allocated memory to zero-values
void *sqliterkOSMalloc(size_t size);
void sqliterkOSFree(void *p);

int sqliterkOSLog(sqliterk_loglevel loglevel,
                  int result,
                  const char *format,
                  ...)
#ifdef __GNUC__
    __attribute__((format(printf, 3, 4)))
#endif
    ;
int sqliterkOSRegister(sqliterk_os os);

#define sqliterkOSDebug(result, ...)                                           \
    sqliterkOSLog(sqliterk_loglevel_debug, result, ##__VA_ARGS__)
#define sqliterkOSWarning(result, ...)                                         \
    sqliterkOSLog(sqliterk_loglevel_warning, result, ##__VA_ARGS__)
#define sqliterkOSError(result, ...)                                           \
    sqliterkOSLog(sqliterk_loglevel_error, result, ##__VA_ARGS__)
#define sqliterkOSInfo(result, ...)                                            \
    sqliterkOSLog(sqliterk_loglevel_info, result, ##__VA_ARGS__)

int sqliterkOSReadOnlyOpen(const char *path, sqliterk_file **file);
int sqliterkOSClose(sqliterk_file *file);
int sqliterkOSRead(sqliterk_file *file,
                   off_t offset,
                   unsigned char *data,
                   size_t *size);
int sqliterkOSFileSize(sqliterk_file *file, size_t *filesize);
const char *sqliterkOSGetFilePath(sqliterk_file *file);

#endif /* sqliterk_os_h */
