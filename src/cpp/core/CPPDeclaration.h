//
// Created by 陈秋文 on 2022/8/31.
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

#include "WINQ.h"

#define WCDB_CPP_ORM_STATIC_ASSERT_FOR_OBJECT_TYPE                                             \
    static_assert(ObjectType::isObjectRelationMapped, "This class has no ORM configuration."); \
    static_assert(std::is_constructible<ObjectType>::value,                                    \
                  "This class must have default constructor.");                                \
    static_assert(std::is_copy_assignable<ObjectType>::value                                   \
                  && std::is_copy_constructible<ObjectType>::value,                            \
                  "This class must be copy constructible and copy assignable.");

namespace WCDB {

class BaseAccessor;
class Field;
class ResultField;
class Binding;

typedef SyntaxList<Field> Fields;
typedef SyntaxList<ResultField> ResultFields;

class HandleOperation;
class HandleORMOperation;
class StatementOperation;
class TableOperation;
template<class ObjectType>
class TableORMOperation;

class InnerDatabase;
class Database;
class InnerHandle;
class Handle;
class HandleStatement;
class PreparedStatement;

class BaseChainCall;

template<class ObjectType>
class Table;

class MultiObject;

template<class ObjectType>
class Insert;

template<class ObjectType>
class Update;

class Delete;

template<class ObjectType>
class Select;

class MultiSelect;

} //namespace WCDB
