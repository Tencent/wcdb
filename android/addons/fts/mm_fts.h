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

#ifndef __MM_FTS_H__
#define __MM_FTS_H__

#include <sqlite3ext.h>
SQLITE_EXTENSION_INIT3


#ifdef __cplusplus
extern "C" {
#endif

int sqlite3_mmftsext_init(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi);

int sqlite3_register_mm_tokenizer(sqlite3 *db);
int sqlite3_register_mm_cipher(sqlite3 *db, const unsigned char *key);
int sqlite3_register_mm_utils(sqlite3 *db);
int sqlite3_mm_set_cipher_key(sqlite3 *db, const void *key, int key_len);
void sqlite3_mm_set_last_error(const char *fmt, ...);
void sqlite3_mm_clear_error();


#ifdef __cplusplus
}
#endif

#endif	// __MM_FTS_H__
