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

#include "ForeignKeyBridge.h"
#include "Column.hpp"
#include "ForeignKey.hpp"
#include "ObjectBridge.hpp"
#include "WinqBridge.hpp"

CPPForeignKey WCDBForeignKeyCreate()
{
    return WCDBCreateCPPBridgedObject(CPPForeignKey, WCDB::ForeignKey);
}
void WCDBForeignKeyConfigReferencesTable(CPPForeignKey foreignKey, const char* _Nullable table)
{
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppObj);
    cppObj->references(table);
}

void WCDBForeignKeyAddColumn(CPPForeignKey foreignKey, CPPColumn column)
{
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppObj);
    WCDBGetObjectOrReturn(column, WCDB::Column, cppColumn);
    cppObj->column(*cppColumn);
}

void WCDBForeignKeyAddColumn2(CPPForeignKey foreignKey, CPPCommonArray columns)
{
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppObj);
    for (int i = 0; i < columns.length; i++) {
        switch (columns.type) {
        case WCDBBridgedType_String:
            cppObj->column(WCDB::StringView(
            WCDBGetCommonArrayLiteralValue(const char*, columns, i)));
            break;
        case WCDBBridgedType_Column:
            cppObj->column(WCDBGetCommonArrayObject(WCDB::Column, columns, i));
            break;
        default:
            assert(0);
            break;
        }
    }
}

void WCDBForeignKeyConfigOnDeleteSetNull(CPPForeignKey foreignKey)
{
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppObj);
    cppObj->onDeleteSetNull();
}

void WCDBForeignKeyConfigOnDeleteSetDefault(CPPForeignKey foreignKey)
{
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppObj);
    cppObj->onDeleteSetDefault();
}
void WCDBForeignKeyConfigOnDeleteCascade(CPPForeignKey foreignKey)
{
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppObj);
    cppObj->onDeleteCascade();
}

void WCDBForeignKeyConfigOnDeleteRestrict(CPPForeignKey foreignKey)
{
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppObj);
    cppObj->onDeleteRestrict();
}

void WCDBForeignKeyConfigOnDeleteNoAction(CPPForeignKey foreignKey)
{
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppObj);
    cppObj->onDeleteNoAction();
}

void WCDBForeignKeyConfigOnUpdateSetNull(CPPForeignKey foreignKey)
{
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppObj);
    cppObj->onUpdateSetNull();
}

void WCDBForeignKeyConfigOnUpdateSetDefault(CPPForeignKey foreignKey)
{
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppObj);
    cppObj->onUpdateSetDefault();
}

void WCDBForeignKeyConfigOnUpdateCascade(CPPForeignKey foreignKey)
{
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppObj);
    cppObj->onUpdateCascade();
}

void WCDBForeignKeyConfigOnUpdateRestrict(CPPForeignKey foreignKey)
{
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppObj);
    cppObj->onUpdateRestrict();
}

void WCDBForeignKeyConfigOnUpdateNoAction(CPPForeignKey foreignKey)
{
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppObj);
    cppObj->onUpdateNoAction();
}

void WCDBForeignKeyConfigMatch(CPPForeignKey foreignKey, WCDBSyntaxMatchType match)
{
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppObj);
    cppObj->match(WCDB::WinqBridge::changeMatchType(match));
}

void WCDBForeignKeyConfigDeferrable(CPPForeignKey foreignKey)
{
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppObj);
    cppObj->deferrable();
}

void WCDBForeignKeyConfigDeferrableInitiallyDeferred(CPPForeignKey foreignKey)
{
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppObj);
    cppObj->deferrableInitiallyDeferred();
}

void WCDBForeignKeyConfigDeferrableInitiallyImmediate(CPPForeignKey foreignKey)
{
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppObj);
    cppObj->deferrableInitiallyImmediate();
}

void WCDBForeignKeyConfigNotDeferrable(CPPForeignKey foreignKey)
{
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppObj);
    cppObj->notDeferrable();
}

void WCDBForeignKeyConfigNotDeferrableInitiallyDeferred(CPPForeignKey foreignKey)
{
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppObj);
    cppObj->notDeferrableInitiallyDeferred();
}

void WCDBForeignKeyConfigNotDeferrableInitiallyImmediate(CPPForeignKey foreignKey)
{
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppObj);
    cppObj->notDeferrableInitiallyImmediate();
}
