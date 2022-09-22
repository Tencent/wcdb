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

#include <WCDB/Expression.hpp>
#include <WCDB/ObjectBridge.hpp>
#include <WCDB/Schema.hpp>
#include <WCDB/StatementAttach.hpp>
#include <WCDB/StatementAttachBridge.h>

CPPStatementAttach WCDBStatementAttachCreate()
{
    return WCDBCreateCPPBridgedObject(CPPStatementAttach, new WCDB::StatementAttach());
}

void WCDBStatementAttachConfigPath(CPPStatementAttach attach, CPPExpression path)
{
    WCDBGetObjectOrReturn(attach, WCDB::StatementAttach, cppAttach);
    WCDBGetObjectOrReturn(path, WCDB::Expression, cppPath);
    cppAttach->attach(*cppPath);
}

void WCDBStatementAttachConfigSchema(CPPStatementAttach attach, CPPSchema schema)
{
    WCDBGetObjectOrReturn(attach, WCDB::StatementAttach, cppAttach);
    WCDBGetObjectOrReturn(schema, WCDB::Schema, cppSchema);
    cppAttach->as(*cppSchema);
}
