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

#if defined(__ANDROID__) && !defined(WCDB_ICU_DIRECT_LINKING)

#define ICUCOMPAT_IMPL

#include "icucompat.h"
#include <dirent.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

static void *s_libicuuc_so = NULL;
static void *s_libicui18n_so = NULL;
icu_compat_t __g_icucompat_iface__ = {0};

static void *
load_function(void *lib_handle, const char *func_name, const char *suffix)
{
    char buf[128];
    size_t len = strlcpy(buf, func_name, sizeof(buf));
    strlcpy(buf + len, suffix, sizeof(buf) - len);

    return dlsym(lib_handle, buf);
}

static void load_functions(const char *suffix)
{
#define ICUCOMPAT_UC_FUNC(ret_type, func_name, arg_list)                       \
    (__g_icucompat_iface__.func_name##_) =                                     \
        (ret_type(*) arg_list) load_function(s_libicuuc_so, #func_name,        \
                                             suffix);
#define ICUCOMPAT_I18N_FUNC(ret_type, func_name, arg_list)                     \
    (__g_icucompat_iface__.func_name##_) =                                     \
        (ret_type(*) arg_list) load_function(s_libicui18n_so, #func_name,      \
                                             suffix);

#include "icuprototype.h"

#undef ICUCOMPAT_UC_FUNC
#undef ICUCOMPAT_I18N_FUNC
}

static const char *find_icu_suffix(void *lib_handle, char *buf, size_t buflen)
{
    DIR *icudir = opendir("/system/usr/icu");
    if (!icudir)
        return NULL;

    struct dirent *dp;
    while ((dp = readdir(icudir)) != NULL) {
        if (dp->d_type == DT_REG) {
            // Find /system/usr/icu/icudt??l.dat
            const char *name = dp->d_name;
            if (strlen(name) == 12 && !strncmp("icudt", name, 5) &&
                name[5] >= '0' && name[5] <= '9' && name[6] >= '0' &&
                name[6] <= '9' && !strncmp("l.dat", &name[7], 6)) {
                snprintf(buf, buflen, "_%c%c", name[5], name[6]);

                // Try loading with suffix.
                if (load_function(lib_handle, "u_getVersion", buf)) {
                    closedir(icudir);
                    return buf;
                }
            }
        }
    }
    closedir(icudir);

    // Failed, try plain names without suffix.
    if (load_function(lib_handle, "u_getVersion", "")) {
        buf[0] = 0;
        return buf;
    }

    return NULL;
}

int init_icucompat()
{
    char suffix[16];
    if (s_libicuuc_so || s_libicui18n_so)
        return 0;

    s_libicuuc_so = dlopen("libicuuc.so", RTLD_LAZY);
    s_libicui18n_so = dlopen("libicui18n.so", RTLD_LAZY);
    if (!s_libicuuc_so || !s_libicui18n_so)
        goto bail;

    if (!find_icu_suffix(s_libicuuc_so, suffix, sizeof(suffix)))
        goto bail;

    load_functions(suffix);
    return 0;

bail:
    if (s_libicuuc_so)
        dlclose(s_libicuuc_so);
    if (s_libicui18n_so)
        dlclose(s_libicui18n_so);
    s_libicuuc_so = NULL;
    s_libicui18n_so = NULL;
    return -1;
}

void destroy_icucompat()
{
    if (s_libicuuc_so)
        dlclose(s_libicuuc_so);
    if (s_libicui18n_so)
        dlclose(s_libicui18n_so);
    s_libicuuc_so = NULL;
    s_libicui18n_so = NULL;
}

#endif // defined(__ANDROID__) && !defined(WCDB_ICU_DIRECT_LINKING)
