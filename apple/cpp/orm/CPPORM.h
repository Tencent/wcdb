//
// Created by qiuwenchen on 2022/8/26.
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

#define WCDB_CPP_ORM_STATIC_ASSERT_FOR_OBJECT_TYPE                                             \
    static_assert(ObjectType::isObjectRelationMapped, "This class has no ORM configuration."); \
    static_assert(std::is_constructible<ObjectType>::value,                                    \
                  "This class must have default constructor.");                                \
    static_assert(std::is_copy_assignable<ObjectType>::value                                   \
                  && std::is_copy_constructible<ObjectType>::value,                            \
                  "This class must be copy constructible and copy assignable.");

#include <WCDB/Binding.hpp>
#include <WCDB/FTSTokenizerUtil.hpp>
#include <WCDB/Field.hpp>
#include <WCDB/MemberPointer.hpp>
#include <WCDB/ResultField.hpp>
#include <WCDB/RunTimeAccessor.hpp>
