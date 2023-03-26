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

#ifdef __cplusplus
#if __cplusplus < 201402L
#error `CLANG_CXX_LANGUAGE_STANDARD = gnu++14` and `CLANG_CXX_LIBRARY = libc++` build settings are required.
#endif
#else // __cplusplus
#error Objective-C++ environment is required. (By changing suffix of those files including <WCDB/WCDBObjc.h> from `.m` to `.mm`.
#endif // __cplusplus

// WCDB may fail to compile since there are few frameworks that define `offset` as a macro.
#ifdef offset
#error Symbol `offset` should not be defined as a macro since it's a common used word.
#endif

#import "Interface.h"
#import "WCTBuiltin.h"
#import "WINQ.h"

#import "WCTDatabase+Test.h"
