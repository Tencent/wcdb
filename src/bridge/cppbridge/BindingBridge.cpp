//
// Created by qiuwenchen on 2022/12/2.
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

#include "BindingBridge.h"
#include "Assertion.hpp"
#include "BaseBinding.hpp"
#include "ObjectBridge.hpp"

namespace WCDB {

class BridgedBinding final : public BaseBinding {
public:
    BridgedBinding() = default;
    ~BridgedBinding() override final = default;
    void addTableConstraint(const WCDB::TableConstraint& tableConstraint)
    {
        static int i = 0;
        i++;
        m_constraints.emplace(StringView::formatted("%d", i), tableConstraint);
    }
    void addColumnDef(const ColumnDef& columnDef)
    {
        m_columnDefs.emplace_back(
        std::make_pair(columnDef.syntax().column.value().name, columnDef));
    }
};

}; // namespace WCDB

CPPBinding WCDBBindingCreate()
{
    return WCDBCreateCPPBridgedObject(CPPBinding, WCDB::BridgedBinding);
}

void WCDBBindingAddColumnDef(CPPBinding binding, CPPColumnDef columnDef)
{
    WCDBGetObjectOrReturn(binding, WCDB::BridgedBinding, cppBinding);
    WCDBGetObjectOrReturn(columnDef, WCDB::ColumnDef, cppColumnDef);
    cppBinding->addColumnDef(*cppColumnDef);
}

void WCDBBindingEnableAutoIncrementForExistingTable(CPPBinding binding)
{
    WCDBGetObjectOrReturn(binding, WCDB::BridgedBinding, cppBinding);
    cppBinding->enableAutoIncrementForExistingTable();
}

void WCDBBindingAddColumnConstraint(CPPBinding binding,
                                    const char* _Nullable columnName,
                                    CPPColumnConstraint constraint)
{
    WCDBGetObjectOrReturn(binding, WCDB::BridgedBinding, cppBinding);
    WCDBGetObjectOrReturn(constraint, WCDB::ColumnConstraint, cppConstraint);
    WCDB::ColumnDef* columnDef = cppBinding->getColumnDef(columnName);
    WCTRemedialAssert(columnDef != nullptr, "columndef not found", return;);
    columnDef->constraint(*cppConstraint);
}

void WCDBBindingAddIndex(CPPBinding binding,
                         const char* _Nullable indexSuffix,
                         bool isFullName,
                         CPPStatementCreateIndex createIndex)
{
    WCDBGetObjectOrReturn(binding, WCDB::BridgedBinding, cppBinding);
    WCDBGetObjectOrReturn(createIndex, WCDB::StatementCreateIndex, cppCreateIndex);
    WCDB::BaseBinding::Index& index = cppBinding->getOrCreateIndex(indexSuffix, isFullName);
    index.action = WCDB::BaseBinding::Index::Action::Create;
    index.statement = *cppCreateIndex;
}

void WCDBBindingAddTableConstraint(CPPBinding binding, CPPTableConstraint tableConstraint)
{
    WCDBGetObjectOrReturn(binding, WCDB::BridgedBinding, cppBinding);
    WCDBGetObjectOrReturn(tableConstraint, WCDB::TableConstraint, cppTableConstraint);
    cppBinding->addTableConstraint(*cppTableConstraint);
}

void WCDBBindingConfigVirtualModule(CPPBinding binding, const char* _Nullable moduleName)
{
    WCDBGetObjectOrReturn(binding, WCDB::BridgedBinding, cppBinding);
    cppBinding->statementVirtualTable.usingModule(moduleName);
}
void WCDBBindingConfigVirtualModuleArgument(CPPBinding binding, const char* _Nullable argument)
{
    WCDBGetObjectOrReturn(binding, WCDB::BridgedBinding, cppBinding);
    cppBinding->statementVirtualTable.argument(argument);
}

bool WCDBBindingIsPrimaryKey(CPPBinding binding, const char* _Nullable columnName)
{
    WCDBGetObjectOrReturnValue(binding, WCDB::BridgedBinding, cppBinding, false);
    WCDB::ColumnDef* columnDef = cppBinding->getColumnDef(columnName);
    if (!columnDef) {
        return false;
    }
    return columnDef->syntax().isAutoIncrement();
}

bool WCDBBindingCreateTable(CPPBinding binding, const char* _Nullable tableName, CPPHandle handle)
{
    WCDBGetObjectOrReturnValue(binding, WCDB::BridgedBinding, cppBinding, false);
    WCDBGetObjectOrReturnValue(handle, WCDB::InnerHandle, cppHandle, false);
    return cppBinding->createTable(tableName, cppHandle);
}

bool WCDBBindingCreateVirtualTable(CPPBinding binding, const char* _Nullable tableName, CPPHandle handle)
{
    WCDBGetObjectOrReturnValue(binding, WCDB::BridgedBinding, cppBinding, false);
    WCDBGetObjectOrReturnValue(handle, WCDB::InnerHandle, cppHandle, false);
    return cppBinding->createVirtualTable(tableName, cppHandle);
}

void WCDBBindingConfigWithoutRowId(CPPBinding binding)
{
    WCDBGetObjectOrReturn(binding, WCDB::BridgedBinding, cppBinding);
    cppBinding->statementTable.withoutRowID();
}

const void* _Nullable WCDBBindingGetBaseBinding(CPPBinding binding)
{
    WCDBGetObjectOrReturnValue(binding, WCDB::BridgedBinding, cppBinding, nullptr);
    WCTAssert(dynamic_cast<const WCDB::BaseBinding*>(cppBinding) != nullptr);
    return static_cast<const WCDB::BaseBinding*>(cppBinding);
}
