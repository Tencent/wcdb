//
// Created by qiuwenchen on 2022/5/30.
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

#include "WindowDefBridge.h"
#include "Expression.hpp"
#include "ObjectBridge.hpp"
#include "OrderingTerm.hpp"
#include "WindowDef.hpp"
#include "WinqBridge.hpp"

CPPWindowDef WCDBWindowDefCreate(void)
{
    return WCDBCreateCPPBridgedObject(CPPWindowDef, WCDB::WindowDef);
}

void WCDBWindowDefConfigPartition(CPPWindowDef def, const CPPExpression* partitions, int partitionNum)
{
    WCDBGetObjectOrReturn(def, WCDB::WindowDef, cppDef);
    WCDBGetCPPSyntaxListOrReturn(WCDB::Expression, cppPartitions, partitions, partitionNum);
    cppDef->partition(cppPartitions);
}

void WCDBWindowDefConfigOrder(CPPWindowDef def,
                              const CPPOrderingTerm* _Nullable orderingTerms,
                              int orderingTermNum)
{
    WCDBGetObjectOrReturn(def, WCDB::WindowDef, cppDef);
    WCDBGetCPPSyntaxListOrReturn(WCDB::OrderingTerm, cppTerms, orderingTerms, orderingTermNum);
    cppDef->order(cppTerms);
}

void WCDBWindowDefConfigFrameSpec(CPPWindowDef def, CPPFrameSpec frameSpec)
{
    WCDBGetObjectOrReturn(def, WCDB::WindowDef, cppDef);
    WCDBGetObjectOrReturn(frameSpec, WCDB::FrameSpec, cppFrameSpec);
    cppDef->framespec(*cppFrameSpec);
}
