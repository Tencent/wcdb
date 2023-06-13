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

#include "ColumnDefBridge.h"
#include "ColumnConstraint.hpp"
#include "ColumnDef.hpp"
#include "ObjectBridge.hpp"
#include "WinqBridge.hpp"

CPPColumnDef WCDBColumnDefCreateWithoutType2(CPPCommonValue column)
{
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPColumnDef, WCDB::ColumnDef, WCDBCreateColumnFromCommonValue(column));
}

CPPColumnDef WCDBColumnDefCreateWithType2(CPPCommonValue column, enum WCDBSyntaxColumnType type)
{
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPColumnDef,
    WCDB::ColumnDef,
    WCDBCreateColumnFromCommonValue(column),
    WCDB::WinqBridge::changeColumnType(type));
}

CPPColumnDef WCDBColumnDefCreateWithoutType(CPPColumn column)
{
    WCDBGetObjectOrReturnValue(column, WCDB::Column, cppColumn, CPPColumnDef());
    return WCDBCreateCPPBridgedObjectWithParameters(CPPColumnDef, WCDB::ColumnDef, *cppColumn);
}

CPPColumnDef WCDBColumnDefCreateWithType(CPPColumn column, enum WCDBSyntaxColumnType type)
{
    WCDBGetObjectOrReturnValue(column, WCDB::Column, cppColumn, CPPColumnDef());
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPColumnDef, WCDB::ColumnDef, *cppColumn, WCDB::WinqBridge::changeColumnType(type));
}

void WCDBColumnDefConfigConstraint(CPPColumnDef columnDef, CPPColumnConstraint constraint)
{
    WCDBGetObjectOrReturn(columnDef, WCDB::ColumnDef, cppColumnDef);
    WCDBGetObjectOrReturn(constraint, WCDB::ColumnConstraint, cppColumnConstraint);
    cppColumnDef->constraint(*cppColumnConstraint);
}
