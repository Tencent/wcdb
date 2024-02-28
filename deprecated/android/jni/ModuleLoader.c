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

#include "ModuleLoader.h"
#include "Logger.h"
#include <jni.h>
#include <stdint.h>
#include <stdlib.h>

#define LOG_TAG "WCDB"

typedef int (*jni_init_func_t)(JavaVM *vm, JNIEnv *env);

typedef struct wcdb_moddef_t {
    const char *name;
    void *func;
} wcdb_moddef_t;

static wcdb_moddef_t *g_moddef[WCDB_INITTAG_COUNT] = {0};
static size_t g_moddef_num[WCDB_INITTAG_COUNT] = {0};
static size_t g_moddef_capacity[WCDB_INITTAG_COUNT] = {0};

void register_module_func(const char *name, void *func, unsigned tag)
{
    if (g_moddef_num[tag] == g_moddef_capacity[tag]) {
        size_t new_size =
            g_moddef_capacity[tag] ? g_moddef_capacity[tag] * 2 : 16;
        void *ptr = realloc(g_moddef[tag], new_size * sizeof(wcdb_moddef_t));
        if (!ptr)
            *(char *) (0xdeadbeaf) = 0;

        g_moddef[tag] = (wcdb_moddef_t *) ptr;
        g_moddef_capacity[tag] = new_size;
    }

    wcdb_moddef_t *mod = g_moddef[tag] + (g_moddef_num[tag]++);
    mod->name = name;
    mod->func = func;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv *env;
    if ((*vm)->GetEnv(vm, (void **) (&env), JNI_VERSION_1_6) != JNI_OK)
        return -1;

    wcdb_moddef_t *mod = g_moddef[WCDB_INITTAG_JNI_INIT];
    size_t num_mod = g_moddef_num[WCDB_INITTAG_JNI_INIT];

    int i;
    for (i = 0; i < num_mod; i++) {
        LOGI(LOG_TAG, "Initialize JNI module (%d/%d) %s...", i + 1,
             (int) num_mod, mod->name);
        int ret = ((jni_init_func_t)(mod->func))(vm, env);
        if (ret != 0)
            return ret;
        mod++;
    }

    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
    JNIEnv *env;
    if ((*vm)->GetEnv(vm, (void **) (&env), JNI_VERSION_1_6) != JNI_OK)
        return;

    wcdb_moddef_t *mod = g_moddef[WCDB_INITTAG_JNI_FINI];
    size_t num_mod = g_moddef_num[WCDB_INITTAG_JNI_FINI];
    while (num_mod--) {
        LOGI(LOG_TAG, "Finalize JNI module '%s'...", mod->name);
        ((jni_init_func_t)(mod->func))(vm, env);
        mod++;
    }
}

static void __attribute__((destructor)) wcdb_init_destroy()
{
    int i;
    for (i = 0; i < WCDB_INITTAG_COUNT; i++)
        free(g_moddef[i]);
}
