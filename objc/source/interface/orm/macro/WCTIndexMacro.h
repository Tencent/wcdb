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

#define __WCDB_INDEX_BEGIN(className, indexSubfixName, propertyName)                     \
    +(void) WCDB_ORM(className, index)                                                   \
    {                                                                                    \
        WCDB_STATIC_ASSERT_EXISTS(className.propertyName);                               \
        const WCTProperty &property = binding.getProperty(WCDB_STRINGIFY(propertyName)); \
        WCDB::StatementCreateIndex &index = binding.getOrCreateIndex(indexSubfixName);

#define __WCDB_INDEX_END(className, indexSubfixName, propertyName) }

#define __WCDB_ORDERED_INDEX_IMP(className, indexSubfixName, propertyName, order_, isUnique) \
    __WCDB_INDEX_BEGIN(className, indexSubfixName, propertyName)                             \
    WCDB_IF(isUnique, index.unique();)                                                       \
    index.indexed(property.asIndex().order(order_));                                         \
    __WCDB_INDEX_END(className, indexSubfixName, propertyName)

#define __WCDB_INDEX_IMP(className, indexSubfixName, propertyName, isUnique)   \
    __WCDB_INDEX_BEGIN(className, indexSubfixName, propertyName)               \
    WCDB_IF(isUnique, index.unique();)                                         \
    index.indexed(property.asIndex());                                         \
    __WCDB_INDEX_END(className, indexSubfixName, propertyName)
