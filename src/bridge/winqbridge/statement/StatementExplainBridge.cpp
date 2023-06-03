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

#include "StatementExplainBridge.h"
#include "Statement.hpp"
#include "StatementExplain.hpp"
#include "WinqBridge.hpp"

CPPStatementExplain WCDBStatementExplainCreate(void)
{
    return WCDBCreateCPPBridgedObject(CPPStatementExplain, WCDB::StatementExplain);
}

void WCDBStatementExplain(CPPStatementExplain explain, CPPObject* statement, bool queryPlan)
{
    WCDBGetObjectOrReturn(explain, WCDB::StatementExplain, cppExplain);
    WCDB::Statement* cppStatement
    = (WCDB::Statement*) WCDB::ObjectBridge::extractOriginalCPPObject(statement);
    cppExplain->syntax().queryPlan = queryPlan;
    cppExplain->syntax().stmt = cppStatement->syntax();
}
