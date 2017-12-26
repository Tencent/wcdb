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

#include <SQLite-Bridging.h>
#ifdef WCDB_COCOAPODS
#include "sqlite3.h"
#else //WCDB_COCOAPODS
#include <sqlcipher/sqlite3.h>
#endif //WCDB_COCOAPODS

int sqlite3_bind_text_transient(sqlite3_stmt *a, int b, const char *c, int d)
{
    return sqlite3_bind_text(a, b, c, d, SQLITE_TRANSIENT);
}

int sqlite3_bind_blob_transient(sqlite3_stmt *a, int b, const void *c, int n)
{
    return sqlite3_bind_blob(a, b, c, n, SQLITE_TRANSIENT);
}

int sqlite3_config_multithread(void)
{
    return sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
}

int sqlite3_config_memstatus(int a)
{
    return sqlite3_config(SQLITE_CONFIG_MEMSTATUS, a);
}

int sqlite3_config_log(sqlite3_global_log a, void *b)
{
    return sqlite3_config(SQLITE_CONFIG_LOG, a, b);
}
