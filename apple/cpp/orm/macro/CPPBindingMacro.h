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

#define __WCDB_CPP_ORM_COMMON_DECLARATION(className)                               \
    std::shared_ptr<long long> lastInsertedRowID = std::make_shared<long long>(0); \
    bool isAutoIncrement;                                                          \
    using WCDBORMType = className;                                                 \
    constexpr static bool isObjectRelationMapped = true;

#define __WCDB_CPP_ORM_ALL_PROPERTIES_IMPLEMENTATION                           \
    {                                                                          \
        return getObjectRelationBinding().allFields();                         \
    }

#define __WCDB_CPP_ORM_GET_BINDING_IMPLEMENTATIONP_START                       \
    {                                                                          \
        static WCDB::Binding* g_binding = new WCDB::Binding();                 \
        static std::once_flag g_flag;                                          \
    std::call_once(g_flag, [&](){
#define __WCDB_CPP_ORM_IMPLEMENTATION_END                                      \
    });                                                                        \
    return *g_binding;                                                         \
    }
