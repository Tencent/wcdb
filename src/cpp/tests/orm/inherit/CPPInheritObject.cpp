//
// Created by qiuwenchen on 2023/10/29.
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

#include "CPPInheritObject.hpp"

WCDB_CPP_ORM_IMPLEMENTATION_BEGIN(CPPInheritBase1);
WCDB_CPP_SYNTHESIZE(value1)
WCDB_CPP_SYNTHESIZE(value2)
WCDB_CPP_PRIMARY(value1)
WCDB_CPP_INDEX("_value2", value2)
WCDB_CPP_ORM_IMPLEMENTATION_END

WCDB_CPP_ORM_IMPLEMENTATION_BEGIN(CPPInheritBase2)
WCDB_CPP_INHERIT_ORM(CPPInheritBase1)
WCDB_CPP_SYNTHESIZE(value3)
WCDB_CPP_SYNTHESIZE(value4)
WCDB_CPP_INDEX("_value3_value4", value3)
WCDB_CPP_INDEX("_value3_value4", value4)
WCDB_CPP_ORM_IMPLEMENTATION_END

WCDB_CPP_ORM_IMPLEMENTATION_BEGIN(CPPInheritObject)
WCDB_CPP_INHERIT_ORM(CPPInheritBase2)
WCDB_CPP_SYNTHESIZE(value5)
WCDB_CPP_UNIQUE(value5)
WCDB_CPP_ORM_IMPLEMENTATION_END
