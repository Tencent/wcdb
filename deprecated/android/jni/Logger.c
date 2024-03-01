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

#include "Logger.h"
#include <stdio.h>

static wcdb_logfunc_t g_logfunc = __android_log_write;

static int dummy_log(int prio, const char *tag, const char *msg)
{
    return 0;
}

wcdb_logfunc_t wcdb_get_log_function()
{
    return g_logfunc;
}

void wcdb_set_log_function(wcdb_logfunc_t func)
{
    if (!func)
        func = dummy_log;
    g_logfunc = func;
}

int wcdb_log_vprint(int prio, const char *tag, const char *fmt, va_list ap)
{
    char buf[1024];
    vsnprintf(buf, sizeof(buf), fmt, ap);
    return g_logfunc(prio, tag, buf);
}

int wcdb_log_print(int prio, const char *tag, const char *fmt, ...)
{
    va_list ap;
    char buf[1024];

    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    return g_logfunc(prio, tag, buf);
}
