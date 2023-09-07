//
// Created by qiuwenchen on 2022/7/17.
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

#include "StatementPragmaBridge.h"
#include "LiteralValue.hpp"
#include "Pragma.hpp"
#include "Schema.hpp"
#include "StatementPragma.hpp"
#include "WinqBridge.hpp"

CPPStatementPragma WCDBStatementPragmaCreate(void)
{
    return WCDBCreateCPPBridgedObject(CPPStatementPragma, WCDB::StatementPragma);
}

void WCDBStatementPragmaConfigPragma(CPPStatementPragma statment, CPPPragma pragma)
{
    WCDBGetObjectOrReturn(statment, WCDB::StatementPragma, cppStatement);
    WCDBGetObjectOrReturn(pragma, WCDB::Pragma, cppPragma);
    cppStatement->pragma(*cppPragma);
}

void WCDBStatementPragmaConfigSchema(CPPStatementPragma statment, CPPSchema schema)
{
    WCDBGetObjectOrReturn(statment, WCDB::StatementPragma, cppStatement);
    WCDBGetObjectOrReturn(schema, WCDB::Schema, cppSchema);
    cppStatement->schema(*cppSchema);
}

void WCDBStatementPragmaConfigSchema2(CPPStatementPragma statment, CPPCommonValue schema)
{
    WCDBGetObjectOrReturn(statment, WCDB::StatementPragma, cppStatement);
    cppStatement->schema(WCDBCreateSchemaFromCommonValue(schema));
}

void WCDBStatementPragmaConfigToValue(CPPStatementPragma statment, CPPLiteralValue value)
{
    WCDBGetObjectOrReturn(statment, WCDB::StatementPragma, cppStatement);
    WCDBGetObjectOrReturn(value, WCDB::LiteralValue, cppValue);
    cppStatement->to(*cppValue);
}

void WCDBStatementPragmaConfigWithValue(CPPStatementPragma statment, CPPLiteralValue value)
{
    WCDBGetObjectOrReturn(statment, WCDB::StatementPragma, cppStatement);
    WCDBGetObjectOrReturn(value, WCDB::LiteralValue, cppValue);
    cppStatement->with(*cppValue);
}

void WCDBStatementPragmaConfigToValue2(CPPStatementPragma statment, CPPCommonValue value)
{
    WCDBGetObjectOrReturn(statment, WCDB::StatementPragma, cppStatement);
    cppStatement->to(WCDBCreateLiteralValueFromCommonValue(value));
}

void WCDBStatementPragmaConfigWithValue2(CPPStatementPragma statment, CPPCommonValue value)
{
    WCDBGetObjectOrReturn(statment, WCDB::StatementPragma, cppStatement);
    cppStatement->with(WCDBCreateLiteralValueFromCommonValue(value));
}
