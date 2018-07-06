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

#define WCDB_ORM_PREFIX __wcdb

#define __WCDB_ORM_CONCAT(a, b) WCDB_CONCAT(a, WCDB_CONCAT(_, b))

#define __WCDB_ORM_MAKE_UNIQUE(expr) __WCDB_ORM_CONCAT(expr, WCDB_UNIQUE_ID)

#define WCDB_ORM(className, ORMType)                                           \
    __WCDB_ORM_MAKE_UNIQUE(                                                    \
    __WCDB_ORM_CONCAT(__WCDB_ORM_CONCAT(WCDB_ORM_PREFIX, className), ORMType)) \
    : (WCTBinding &) binding

#if defined(__cplusplus)
#define WCDB_EXTERN extern "C"
#else
#define WCDB_EXTERN extern
#endif
