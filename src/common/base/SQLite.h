//
// Created by sanhuazhang on 2018/09/26
//

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

#pragma once

#if SQLITE_WCDB
#include <sqlcipher/sqlite3.h>
#include <sqlcipher/sqlite3_wcdb.h>

#ifdef __cplusplus
extern "C" {
#endif

extern char *sqlite3_temp_directory;

int sqlcipher_set_default_hmac_algorithm(int algorithm);
int sqlcipher_set_default_kdf_algorithm(int algorithm);
void sqlcipher_set_default_kdf_iter(int iter);
void sqlcipher_set_default_use_hmac(int use);

#ifdef __cplusplus
}
#endif

#else
#include <sqlite3.h>
#endif
