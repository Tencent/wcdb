//
// Created by 陈秋文 on 2022/8/27.
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

#include <WCDB/Macro.h>

#define __WCDB_CPP_INDEX_BEGIN(indexSuffixName)                                \
    {                                                                          \
        WCDB::BaseBinding::Index &index = g_binding->getOrCreateIndex(indexSuffixName);

#define __WCDB_CPP_INDEX_BEGIN_WITH_FIELD_NAME(indexSuffixName, fieldName)              \
    {                                                                                   \
        static_assert(has_synthesized_##fieldName,                                      \
                      "You need to synthesize this field before you use it");           \
        WCDB::BaseBinding::Index &index = g_binding->getOrCreateIndex(indexSuffixName); \
        auto temp_memory_point = &WCDBORMType::fieldName;                               \
        WCDB_UNUSED(temp_memory_point)

#define __WCDB_CPP_INDEX_END }

#define __WCDB_CPP_ORDERED_INDEX_IMP(indexSuffixName, fieldName, order_, isUnique) \
    __WCDB_CPP_INDEX_BEGIN_WITH_FIELD_NAME(indexSuffixName, fieldName)             \
    WCDB_IF(isUnique, index.statement.unique();)                                   \
    index.statement.indexed(_field_##fieldName.asIndex().order(order_));           \
    __WCDB_CPP_INDEX_END

#define __WCDB_CPP_INDEX_IMP(indexSuffixName, fieldName, isUnique)             \
    __WCDB_CPP_INDEX_BEGIN_WITH_FIELD_NAME(indexSuffixName, fieldName)         \
    WCDB_IF(isUnique, index.statement.unique();)                               \
    index.statement.indexed(_field_##fieldName.asIndex());                     \
    __WCDB_CPP_INDEX_END

#define __WCDB_CPP_INDEX_FOR_NEWLY_CREATED_TABLE_ONLY_IMP(indexSuffixName)           \
    __WCDB_CPP_INDEX_BEGIN(indexSuffixName)                                          \
    index.action = WCDB::BaseBinding::Index::Action::CreateForNewlyCreatedTableOnly; \
    __WCDB_CPP_INDEX_END

#define __WCDB_CPP_INDEX_TO_BE_DROPPED(indexSuffixName)                        \
    __WCDB_CPP_INDEX_BEGIN(indexSuffixName)                                    \
    index.action = WCDB::BaseBinding::Index::Action::Drop;                     \
    __WCDB_CPP_INDEX_END
