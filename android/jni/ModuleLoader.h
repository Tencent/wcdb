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

#ifndef __WCDB_MODULE_LOADER_H__
#define __WCDB_MODULE_LOADER_H__

typedef struct sqlite3 sqlite3;

typedef enum wcdb_inittag_t {
    WCDB_INITTAG_INVALID = -1,

    WCDB_INITTAG_JNI_INIT,
    WCDB_INITTAG_JNI_FINI,
    WCDB_INITTAG_DBCONN_INIT,

    WCDB_INITTAG_COUNT
} wcdb_inittag_t;

// typedef int (*jni_init_func_t)(JavaVM *vm, JNIEnv *env);
// typedef int (*dbconn_init_func_t)(sqlite3 *db, char **errmsg, const void *);

#define WCDB_INITTAG_DEFINE(name, func, tag)                                   \
    static void __attribute__((constructor)) __WCDB__##tag##name()             \
    {                                                                          \
        register_module_func(#name, (void *) (func), (tag));                   \
    }

#define WCDB_JNI_INIT(name, func)                                              \
    WCDB_INITTAG_DEFINE(name, func, WCDB_INITTAG_JNI_INIT)
#define WCDB_JNI_FINI(name, func)                                              \
    WCDB_INITTAG_DEFINE(name, func, WCDB_INITTAG_JNI_FINI)
#define WCDB_DBCONN_INIT(name, func)                                           \
    WCDB_INITTAG_DEFINE(name, func, WCDB_INITTAG_DBCONN_INIT)

#ifdef __cplusplus
#define WECHAT_EXPORT extern "C"
#else
#define WECHAT_EXPORT
#endif

WECHAT_EXPORT void
register_module_func(const char *name, void *func, unsigned tag);
WECHAT_EXPORT int run_dbconn_initializers(sqlite3 *db, char **errmsg);

#endif
