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

#define WCDB_ORM_PREFIX __wcdb

#define WCDB_ORM_CONCAT(a, b) WCDB_CONCAT(a, WCDB_CONCAT(_, b))

#define __WCDB_ORM_TYPE_PREFIX(ORMType)                                        \
    WCDB_ORM_CONCAT(WCDB_ORM_PREFIX, ORMType)

#define __WCDB_ORM(ORMType, suffix)                                            \
    WCDB_ORM_CONCAT(__WCDB_ORM_TYPE_PREFIX(ORMType), suffix)

#define WCDB_ORM_UNIQUE(ORMType)                                               \
    __WCDB_ORM(ORMType, WCDB_UNIQUE_ID) : (WCTBinding &) binding
