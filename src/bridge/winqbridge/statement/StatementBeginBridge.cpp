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

#include "StatementBeginBridge.h"
#include "StatementBegin.hpp"
#include "WinqBridge.hpp"

CPPStatementBegin WCDBStatementBeginCreate()
{
    CPPStatementBegin begin
    = WCDBCreateCPPBridgedObject(CPPStatementBegin, WCDB::StatementBegin);
    WCDBStatementBeginConfigType(begin, WCDBSyntaxTransactionType_Deferred);
    return begin;
}

void WCDBStatementBeginConfigType(CPPStatementBegin begin, enum WCDBSyntaxTransactionType type)
{
    WCDBGetObjectOrReturn(begin, WCDB::StatementBegin, cppBegin);
    switch (type) {
    case WCDBSyntaxTransactionType_Deferred:
        cppBegin->beginDeferred();
        break;
    case WCDBSyntaxTransactionType_Immediate:
        cppBegin->beginImmediate();
        break;
    case WCDBSyntaxTransactionType_Exclusive:
        cppBegin->beginExclusive();
        break;
    }
}
