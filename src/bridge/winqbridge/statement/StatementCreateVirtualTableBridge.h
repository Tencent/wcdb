//
// Created by qiuwenchen on 2022/7/3.
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

#include "WinqBridge.h"

WCDB_EXTERN_C_BEGIN

CPPStatementCreateVirtualTable WCDBStatementCreateVirtualTableCreate(void);
void WCDBStatementCreateVirtualTableConfigTable(CPPStatementCreateVirtualTable createTable,
                                                const char* _Nullable name);
void WCDBStatementCreateVirtualTableConfigSchema(CPPStatementCreateVirtualTable createTable,
                                                 CPPSchema schema);
void WCDBStatementCreateVirtualTableConfigSchema2(CPPStatementCreateVirtualTable createTable,
                                                  CPPCommonValue schema);
void WCDBStatementCreateVirtualTableConfigIfNotExist(CPPStatementCreateVirtualTable createTable);
void WCDBStatementCreateVirtualTableConfigModule(CPPStatementCreateVirtualTable createTable,
                                                 const char* _Nullable module);
void WCDBStatementCreateVirtualTableConfigArguments(CPPStatementCreateVirtualTable createTable,
                                                    const char* const _Nullable* _Nullable arguments,
                                                    int argNum);

WCDB_EXTERN_C_END
