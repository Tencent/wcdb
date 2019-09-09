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

#define WCDB_ORM_TYPE_VIRTUAL_TABLE virtual_table

#define __WCDB_VIRTUAL_TABLE_BEGIN()                                           \
    +(void) WCDB_ORM_UNIQUE(WCDB_ORM_TYPE_VIRTUAL_TABLE)                       \
    {                                                                          \
        WCDB::StatementCreateVirtualTable& statement = binding.statementVirtualTable;

#define __WCDB_VIRTUAL_TABLE_END() }

#define __WCDB_VIRTUAL_TABLE_ARGUMENT_IMP(argument_)                           \
    __WCDB_VIRTUAL_TABLE_BEGIN()                                               \
    statement.argument(argument_);                                             \
    __WCDB_VIRTUAL_TABLE_END()

#define __WCDB_VIRTUAL_TABLE_MODULE_IMP(moduleName)                            \
    __WCDB_VIRTUAL_TABLE_BEGIN()                                               \
    statement.usingModule(moduleName);                                         \
    __WCDB_VIRTUAL_TABLE_END()
