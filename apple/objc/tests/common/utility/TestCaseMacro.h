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

#define weakify(variable)                                                                   \
    __attribute__((objc_ownership(weak))) __typeof__(variable) WCDB_CONCAT(weak_, variable) \
    = (variable);

#define strongify(variable)                                                           \
    _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wshadow\"") \
    __attribute__((objc_ownership(strong))) __typeof__(variable) self                 \
    = WCDB_CONCAT(weak_, variable);                                                   \
    _Pragma("clang diagnostic pop")

#define strongify_or_return(variable)                                          \
    strongify(variable) if (variable == nil) { return; }
