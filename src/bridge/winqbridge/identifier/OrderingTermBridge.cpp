//
// Created by qiuwenchen on 2022/5/29.
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

#include "OrderingTermBridge.h"
#include "Expression.hpp"
#include "ObjectBridge.hpp"
#include "OrderingTerm.hpp"
#include "WinqBridge.hpp"

CPPOrderingTerm WCDBOrderingTermCreate(CPPExpression expression)
{
    WCDBGetObjectOrReturnValue(
    expression, WCDB::Expression, cppExpression, CPPOrderingTerm());
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPOrderingTerm, WCDB::OrderingTerm, *cppExpression);
}

CPPOrderingTerm WCDBOrderingTermCreate2(CPPCommonValue expression)
{
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPOrderingTerm, WCDB::OrderingTerm, WCDBCreateExpressionFromCommonValue(expression));
}

void WCDBOrderingTermConfigCollation(CPPOrderingTerm orderingTerm, const char* _Nullable collation)
{
    WCDBGetObjectOrReturn(orderingTerm, WCDB::OrderingTerm, cppOrderingTerm);
    cppOrderingTerm->collate(collation);
}
void WCDBOrderingTermConfigOrder(CPPOrderingTerm orderingTerm, enum WCDBSyntaxOrder order)
{
    WCDBGetObjectOrReturn(orderingTerm, WCDB::OrderingTerm, cppOrderingTerm);
    cppOrderingTerm->order(WCDB::WinqBridge::changeOrder(order));
}
