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

#define WCDB_BOOL_1 1
#define WCDB_BOOL_0 0
#define WCDB_BOOL_true 1
#define WCDB_BOOL_false 0
#define WCDB_BOOL_TRUE 1
#define WCDB_BOOL_FALSE 0
#define WCDB_BOOL_YES 1
#define WCDB_BOOL_NO 0
#define WCDB_BOOL___objc_yes 1
#define WCDB_BOOL___objc_no 0
#define WCDB_BOOL(var) _WCDB_BOOL(var)
#define _WCDB_BOOL(var) WCDB_BOOL_##var

#define WCDB_IF_ELSE_0(t, f) f
#define WCDB_IF_ELSE_1(t, f) t
#define __WCDB_IF_ELSE(bit, t, f) WCDB_IF_ELSE_##bit(t, f)
#define _WCDB_IF_ELSE(bit, t, f) __WCDB_IF_ELSE(bit, t, f)
#define WCDB_IF_ELSE(cond, t, f) _WCDB_IF_ELSE(WCDB_BOOL(cond), t, f)

#define WCDB_IF_0(t)
#define WCDB_IF_1(t) t
#define __WCDB_IF(bit, t) WCDB_IF_##bit(t)
#define _WCDB_IF(bit, t) __WCDB_IF(bit, t)
#define WCDB_IF(cond, t) _WCDB_IF(WCDB_BOOL(cond), t)

#define WCDB_IF_NOT_0(f) f
#define WCDB_IF_NOT_1(f)
#define __WCDB_IF_NOT(bit, f) WCDB_IF_NOT_##bit(f)
#define _WCDB_IF_NOT(bit, f) __WCDB_IF_NOT(bit, f)
#define WCDB_IF_NOT(cond, f) _WCDB_IF_NOT(WCDB_BOOL(cond), f)

#define _WCDB_STRINGIFY(str) #str
#define WCDB_STRINGIFY(str) _WCDB_STRINGIFY(str)

#ifdef __cplusplus
#define WCDB_EXTERN extern "C" __attribute__((visibility("default")))
#else
#define WCDB_EXTERN extern __attribute__((visibility("default")))
#endif
