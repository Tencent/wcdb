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

#define __WCDB_CPP_SYNTHESIZE_IMP(fieldName, columnName)                                                        \
    auto _mp_##fieldName = &WCDBORMType::fieldName;                                                             \
    constexpr bool _has_synthesized_##fieldName = true;                                                         \
    WCDB_UNUSED(_has_synthesized_##fieldName);                                                                  \
    auto& _field_##fieldName = g_binding->registerField(                                                        \
    WCDB::castMemberPointer(_mp_##fieldName),                                                                   \
    columnName,                                                                                                 \
    std::make_shared<WCDB::RuntimeAccessor<WCDBORMType, WCDB::getMemberType<decltype(_mp_##fieldName)>::type>>( \
    _mp_##fieldName));                                                                                          \
    WCDB_UNUSED(_field_##fieldName);
