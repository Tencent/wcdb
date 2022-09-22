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

#define WCDB_ORM_TYPE_INDEX index

#define __WCDB_INDEX_BEGIN(indexSuffixName)                                    \
    +(void) WCDB_ORM_UNIQUE(WCDB_ORM_TYPE_INDEX)                               \
    {                                                                          \
        WCDB::BaseBinding::Index &index = binding.getOrCreateIndex(indexSuffixName);

#define __WCDB_INDEX_END(indexSuffixName) }

#define __WCDB_ORDERED_INDEX_IMP(indexSuffixName, propertyName, order_, isUnique) \
    __WCDB_INDEX_BEGIN(indexSuffixName)                                           \
    WCDB_STATIC_ASSERT_EXISTS(self.propertyName);                                 \
    WCDB_IF(isUnique, index.statement.unique();)                                  \
    index.statement.indexed(self.propertyName.asIndex().order(order_));           \
    __WCDB_INDEX_END(indexSuffixName)

#define __WCDB_INDEX_IMP(indexSuffixName, propertyName, isUnique)              \
    __WCDB_INDEX_BEGIN(indexSuffixName)                                        \
    WCDB_STATIC_ASSERT_EXISTS(self.propertyName);                              \
    WCDB_IF(isUnique, index.statement.unique();)                               \
    index.statement.indexed(self.propertyName.asIndex());                      \
    __WCDB_INDEX_END(indexSuffixName)

#define __WCDB_INDEX_FOR_NEWLY_CREATED_TABLE_ONLY_IMP(indexSuffixName)               \
    __WCDB_INDEX_BEGIN(indexSuffixName)                                              \
    index.action = WCDB::BaseBinding::Index::Action::CreateForNewlyCreatedTableOnly; \
    __WCDB_INDEX_END(indexSuffixName)

#define __WCDB_INDEX_TO_BE_DROPPED(indexSuffixName)                            \
    __WCDB_INDEX_BEGIN(indexSuffixName)                                        \
    index.action = WCDB::BaseBinding::Index::Action::Drop;                     \
    __WCDB_INDEX_END(indexSuffixName)
