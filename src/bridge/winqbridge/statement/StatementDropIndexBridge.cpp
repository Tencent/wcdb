//
// Created by qiuwenchen on 2022/7/12.
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

#include "StatementDropIndexBridge.h"
#include "Schema.hpp"
#include "StatementDropIndex.hpp"
#include "WinqBridge.hpp"

CPPStatementDropIndex WCDBStatementDropIndexCreate(void)
{
    return WCDBCreateCPPBridgedObject(CPPStatementDropIndex, WCDB::StatementDropIndex);
}

void WCDBStatementDropIndexConfigSchema(CPPStatementDropIndex dropIndexStatement, CPPSchema schema)
{
    WCDBGetObjectOrReturn(dropIndexStatement, WCDB::StatementDropIndex, cppDropIndexStatement);
    WCDBGetObjectOrReturn(schema, WCDB::Schema, cppSchema);
    cppDropIndexStatement->schema(*cppSchema);
}

void WCDBStatementDropIndexConfigSchema2(CPPStatementDropIndex dropIndexStatement,
                                         CPPCommonValue schema)
{
    WCDBGetObjectOrReturn(dropIndexStatement, WCDB::StatementDropIndex, cppDropIndexStatement);
    cppDropIndexStatement->schema(WCDBCreateSchemaFromCommonValue(schema));
}

void WCDBStatementDropIndexConfigIndex(CPPStatementDropIndex dropIndexStatement,
                                       const char* _Nullable name)
{
    WCDBGetObjectOrReturn(dropIndexStatement, WCDB::StatementDropIndex, cppDropIndexStatement);
    cppDropIndexStatement->dropIndex(name);
}

void WCDBStatementDropIndexConfigIfExists(CPPStatementDropIndex dropIndexStatement)
{
    WCDBGetObjectOrReturn(dropIndexStatement, WCDB::StatementDropIndex, cppDropIndexStatement);
    cppDropIndexStatement->ifExists();
}
