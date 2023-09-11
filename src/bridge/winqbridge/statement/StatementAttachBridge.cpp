//
// Created by qiuwenchen on 2022/6/29.
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

#include "StatementAttachBridge.h"
#include "Expression.hpp"
#include "ObjectBridge.hpp"
#include "Schema.hpp"
#include "StatementAttach.hpp"
#include "WinqBridge.hpp"

CPPStatementAttach WCDBStatementAttachCreate()
{
    return WCDBCreateCPPBridgedObject(CPPStatementAttach, WCDB::StatementAttach);
}

void WCDBStatementAttachConfigPath(CPPStatementAttach attach, CPPExpression path)
{
    WCDBGetObjectOrReturn(attach, WCDB::StatementAttach, cppAttach);
    WCDBGetObjectOrReturn(path, WCDB::Expression, cppPath);
    cppAttach->attach(*cppPath);
}

void WCDBStatementAttachConfigPath2(CPPStatementAttach attach, CPPCommonValue path)
{
    WCDBGetObjectOrReturn(attach, WCDB::StatementAttach, cppAttach);
    cppAttach->attach(WCDBCreateExpressionFromCommonValue(path));
}

void WCDBStatementAttachConfigSchema(CPPStatementAttach attach, CPPSchema schema)
{
    WCDBGetObjectOrReturn(attach, WCDB::StatementAttach, cppAttach);
    WCDBGetObjectOrReturn(schema, WCDB::Schema, cppSchema);
    cppAttach->as(*cppSchema);
}

void WCDBStatementAttachConfigSchema2(CPPStatementAttach attach, CPPCommonValue schema)
{
    WCDBGetObjectOrReturn(attach, WCDB::StatementAttach, cppAttach);
    cppAttach->as(WCDBCreateSchemaFromCommonValue(schema));
}

void WCDBStatementAttachConfigKey(CPPStatementAttach attach, CPPExpression key)
{
    WCDBGetObjectOrReturn(attach, WCDB::StatementAttach, cppAttach);
    WCDBGetObjectOrReturn(key, WCDB::Expression, cppKey);
    cppAttach->key(*cppKey);
}

void WCDBStatementAttachConfigKey2(CPPStatementAttach attach, CPPCommonValue key)
{
    WCDBGetObjectOrReturn(attach, WCDB::StatementAttach, cppAttach);
    cppAttach->key(WCDBCreateExpressionFromCommonValue(key));
}
