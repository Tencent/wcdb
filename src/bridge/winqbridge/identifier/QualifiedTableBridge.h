//
// Created by qiuwenchen on 2022/6/11.
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

#include "WinqBridge.h"

WCDB_EXTERN_C_BEGIN

CPPQualifiedTable WCDBQualifiedTableCreateWithTable(const char* _Nullable name);
void WCDBQualifiedTableConfigSchema2(CPPQualifiedTable table, CPPCommonValue schema);
void WCDBQualifiedTableConfigSchema(CPPQualifiedTable table, CPPSchema schema);
void WCDBQualifiedTableConfigAliasName(CPPQualifiedTable table, const char* _Nullable alias);
void WCDBQualifiedTableConfigIndexName(CPPQualifiedTable table, const char* _Nullable indexName);
void WCDBQualifiedTableConfigNoIndexed(CPPQualifiedTable table);

WCDB_EXTERN_C_END
