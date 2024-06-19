//
// Created by sanhuazhang on 2019/05/02
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

#ifndef _WIN32
#define WCDB_API __attribute__((visibility("default")))
#else
#if defined(WCDB_DLL)
#if defined(WCDB_BUILD_DLL)
#define WCDB_API __declspec(dllexport)
#else
#define WCDB_API __declspec(dllimport)
#endif //WCDB_BUILD_DLL
#else
#define WCDB_API
#endif //WCDB_DLL
#endif //_WIN32

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

#define _WCDB_CONCAT(a, b) a##b
#define WCDB_CONCAT(a, b) _WCDB_CONCAT(a, b)

#define WCDB_UNIQUE_ID __COUNTER__
#define WCDB_ONCE(task)                                                        \
    static auto WCDB_CONCAT(__once_, WCDB_UNIQUE_ID) WCDB_USED = task;

#define WCDB_STATIC_ASSERT_EXISTS(type_or_expression)                          \
    static_assert(sizeof(type_or_expression) > 0,                              \
                  WCDB_STRINGIFY(type_or_expression) "does not exist.")

#define WCDB_NO_ESCAPE __attribute__((noescape))
#define WCDB_ESCAPE

#if !defined(_WIN32)
#define WCDB_NO_INLINE __attribute__((noinline))
#else
#define WCDB_NO_INLINE
#endif

#if !defined(_WIN32)
#define WCDB_USED __attribute__((__used__))
#else
#define WCDB_USED
#endif

#ifndef WCDB_EXTERN
#if defined(__cplusplus)
#define WCDB_EXTERN extern "C" WCDB_API
#define WCDB_EXTERN_C_BEGIN extern "C" {
#define WCDB_EXTERN_C_END }
#else
#define WCDB_EXTERN extern WCDB_API
#define WCDB_EXTERN_C_BEGIN
#define WCDB_EXTERN_C_END
#endif
#endif

#define WCDB_FIRST_ARG(N, ...) N
#define WCDB_NON_FIRST_ARGS(N, ...) __VA_ARGS__

#if !defined(_WIN32)
#define WCDB_UNUSED(variable) _Pragma(WCDB_STRINGIFY(unused(variable)))
#else
#define WCDB_UNUSED(variable) (void) (variable)
#endif

#ifdef __clang__
#define WCDB_NO_DESTROY [[clang::no_destroy]]
#define WCDB_STATIC_VARIABLE WCDB_NO_DESTROY static
#else
#define WCDB_NO_DESTROY
#define WCDB_STATIC_VARIABLE
#endif

#define WCDBLiteralStringDefine(name, value)                                   \
    static constexpr const char* k_##name = value;                             \
    WCDB_EXTERN const StringView name;

#define WCDBLiteralStringImplement(name)                                       \
    WCDB_NO_DESTROY const StringView name = StringView::makeConstant(k_##name)
