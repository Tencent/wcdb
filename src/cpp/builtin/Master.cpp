//
// Created by qiuwenchen on 2022/10/27.
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

#include "Master.hpp"

namespace WCDB {

const StringView& Master::tableName = Syntax::masterTable;

WCDB_CPP_ORM_IMPLEMENTATION_BEGIN(Master)

WCDB_CPP_SYNTHESIZE(type)
WCDB_CPP_SYNTHESIZE(name)
WCDB_CPP_SYNTHESIZE_COLUMN(tblName, "tbl_name")
WCDB_CPP_SYNTHESIZE(rootpage)
WCDB_CPP_SYNTHESIZE(sql)

WCDB_CPP_ORM_IMPLEMENTATION_END

} //namespace WCDB
