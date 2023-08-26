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
#include "Column.hpp"
#include "Expression.hpp"
#include "LiteralValue.hpp"
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

void WCDBWindowDefConfigPartition2(CPPWindowDef def, CPPMultiTypeArray partitions)
{
    WCDBGetObjectOrReturn(def, WCDB::WindowDef, cppDef);
    WCDB::Expressions cppPartitions;
    int intIndex = 0;
    int stringIndex = 0;
    for (int i = 0; i < partitions.totalLength; i++) {
        switch (partitions.types[i]) {
        case WCDBBridgedType_String: {
            cppPartitions.emplace_back(WCDB::Column(
            WCDB::UnsafeStringView(partitions.stringValues[stringIndex])));
            stringIndex++;
        } break;
        case WCDBBridgedType_Column: {
            cppPartitions.emplace_back(
            WCDBGetMultiTypeArrayObject(WCDB::Column, partitions, intIndex));
            intIndex++;
        } break;
        case WCDBBridgedType_LiteralValue: {
            cppPartitions.emplace_back(
            WCDBGetMultiTypeArrayObject(WCDB::LiteralValue, partitions, i));
            intIndex++;
        } break;
        case WCDBBridgedType_Expression: {
            cppPartitions.emplace_back(
            WCDBGetMultiTypeArrayObject(WCDB::Expression, partitions, i));
            intIndex++;
        } break;
        default:
            assert(partitions.types[i] == WCDBBridgedType_Invalid);
            break;
        }
    }
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
