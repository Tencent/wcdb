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

#ifndef __ICU_COMPAT_H__
#define __ICU_COMPAT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Disable ICU export renaming. It will be replaced with icucompat macros. */
#define U_DISABLE_RENAMING 1
#include <unicode/ubrk.h>
#include <unicode/ucnv.h>
#include <unicode/ucol.h>
#include <unicode/uiter.h>
#include <unicode/umachine.h>
#include <unicode/unorm.h>
#include <unicode/ustring.h>
#include <unicode/utypes.h>
#include <unicode/uversion.h>

typedef struct icu_compat_t {
#define ICUCOMPAT_UC_FUNC(ret_type, func_name, arg_list)                       \
    ret_type(U_EXPORT2 *func_name##_) arg_list;
#define ICUCOMPAT_I18N_FUNC(ret_type, func_name, arg_list)                     \
    ret_type(U_EXPORT2 *func_name##_) arg_list;

#include "icuprototype.h"

#undef ICUCOMPAT_UC_FUNC
#undef ICUCOMPAT_I18N_FUNC
} icu_compat_t;
extern icu_compat_t __g_icucompat_iface__;

int init_icucompat();
void destroy_icucompat();

#ifndef ICUCOMPAT_IMPL
#define ICUCOMPAT_DEFINE_SYMBOL(symbol) (__g_icucompat_iface__.symbol##_)
#include "icuprototype.h"
#endif

#ifdef __cplusplus
}
#endif

#endif
