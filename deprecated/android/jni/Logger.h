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

#ifndef __WCDB_ANDROID_LOG_H__
#define __WCDB_ANDROID_LOG_H__

#include <android/log.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*wcdb_logfunc_t)(int prio, const char *tag, const char *msg);

wcdb_logfunc_t wcdb_get_log_function();
void wcdb_set_log_function(wcdb_logfunc_t func);

int wcdb_log_print(int prio, const char *tag, const char *fmt, ...)
#ifdef __GNUC__
    __attribute__((format(printf, 3, 4)))
#endif
    ;

int wcdb_log_vprint(int prio, const char *tag, const char *fmt, va_list ap);

inline static int wcdb_log_write(int prio, const char *tag, const char *msg)
{
    return wcdb_get_log_function()(prio, tag, msg);
}

#ifndef MMDB_NO_LOGX_MACRO

#define LOGV(tag, fmt, args...)                                                \
    wcdb_log_print(ANDROID_LOG_VERBOSE, (tag), (fmt), ##args)
#define LOGD(tag, fmt, args...)                                                \
    wcdb_log_print(ANDROID_LOG_DEBUG, (tag), (fmt), ##args)
#define LOGI(tag, fmt, args...)                                                \
    wcdb_log_print(ANDROID_LOG_INFO, (tag), (fmt), ##args)
#define LOGW(tag, fmt, args...)                                                \
    wcdb_log_print(ANDROID_LOG_WARN, (tag), (fmt), ##args)
#define LOGE(tag, fmt, args...)                                                \
    wcdb_log_print(ANDROID_LOG_ERROR, (tag), (fmt), ##args)

#define ALOG(priority, tag, ...)                                               \
    wcdb_log_print(ANDROID_##priority, (tag), __VA_ARGS__)
#define ALOGV(...) ((void) LOGV(LOG_TAG, __VA_ARGS__))
#define ALOGD(...) ((void) LOGD(LOG_TAG, __VA_ARGS__))
#define ALOGI(...) ((void) LOGI(LOG_TAG, __VA_ARGS__))
#define ALOGW(...) ((void) LOGW(LOG_TAG, __VA_ARGS__))
#define ALOGE(...) ((void) LOGE(LOG_TAG, __VA_ARGS__))

#define IF_ALOG(priority, tag) if (1)
#define LOG_FATAL_IF(cond, ...) // do nothing
#define ALOG_ASSERT(cond, ...) LOG_FATAL_IF(!(cond), ##__VA_ARGS__)

#endif

#ifdef __cplusplus
}
#endif

#endif
