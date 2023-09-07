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

#include "QualifiedTableBridge.h"
#include "ObjectBridge.hpp"
#include "QualifiedTable.hpp"
#include "Schema.hpp"
#include "WinqBridge.hpp"

CPPQualifiedTable WCDBQualifiedTableCreateWithTable(const char* _Nullable name)
{
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPQualifiedTable, WCDB::QualifiedTable, name);
}

void WCDBQualifiedTableConfigSchema2(CPPQualifiedTable table, CPPCommonValue schema)
{
    WCDBGetObjectOrReturn(table, WCDB::QualifiedTable, cppTable);
    cppTable->schema(WCDBCreateSchemaFromCommonValue(schema));
}

void WCDBQualifiedTableConfigSchema(CPPQualifiedTable table, CPPSchema schema)
{
    WCDBGetObjectOrReturn(table, WCDB::QualifiedTable, cppTable);
    WCDBGetObjectOrReturn(schema, WCDB::Schema, cppSchema);
    cppTable->schema(*cppSchema);
}

void WCDBQualifiedTableConfigAliasName(CPPQualifiedTable table, const char* _Nullable alias)
{
    WCDBGetObjectOrReturn(table, WCDB::QualifiedTable, cppTable);
    cppTable->as(alias);
}

void WCDBQualifiedTableConfigIndexName(CPPQualifiedTable table, const char* _Nullable indexName)
{
    WCDBGetObjectOrReturn(table, WCDB::QualifiedTable, cppTable);
    cppTable->indexed(indexName);
}

void WCDBQualifiedTableConfigNoIndexed(CPPQualifiedTable table)
{
    WCDBGetObjectOrReturn(table, WCDB::QualifiedTable, cppTable);
    cppTable->notIndexed();
}
