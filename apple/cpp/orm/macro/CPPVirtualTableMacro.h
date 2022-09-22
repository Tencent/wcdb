//
// Created by 陈秋文 on 2022/8/28.
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

#define __WCDB_CPP_VIRTUAL_TABLE_BEGIN()                                       \
    {                                                                          \
        WCDB::StatementCreateVirtualTable& statement = g_binding->statementVirtualTable;

#define __WCDB_CPP_VIRTUAL_TABLE_END() }

#define __WCDB_CPP_VIRTUAL_TABLE_ARGUMENT_IMP(argument_)                       \
    __WCDB_CPP_VIRTUAL_TABLE_BEGIN()                                           \
    statement.argument(argument_);                                             \
    __WCDB_CPP_VIRTUAL_TABLE_END()

#define __WCDB_CPP_VIRTUAL_TABLE_MODULE_IMP(moduleName)                        \
    __WCDB_CPP_VIRTUAL_TABLE_BEGIN()                                           \
    statement.usingModule(moduleName);                                         \
    __WCDB_CPP_VIRTUAL_TABLE_END()
