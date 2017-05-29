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

#include "mm_fts.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static char g_errmsg_buffer[1024];

void sqlite3_mm_set_last_error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vsnprintf(g_errmsg_buffer, sizeof(g_errmsg_buffer), fmt, args);
    g_errmsg_buffer[sizeof(g_errmsg_buffer) - 1] = 0;

    va_end(args);
}

void sqlite3_mm_clear_error()
{
    g_errmsg_buffer[0] = 0;
}

static void mm_last_error(sqlite3_context *db, int argc, sqlite3_value **argv)
{
    sqlite3_result_text(db, g_errmsg_buffer, -1, SQLITE_STATIC);
}

int sqlite3_register_mm_utils(sqlite3 *db)
{
    return sqlite3_create_function_v2(db, "mm_last_error", 0, SQLITE_ANY, NULL,
                                      mm_last_error, NULL, NULL, NULL);
}
