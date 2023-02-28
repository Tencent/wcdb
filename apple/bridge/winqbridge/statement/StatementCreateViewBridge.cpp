//
// Created by 陈秋文 on 2022/7/13.
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

#include "StatementCreateViewBridge.h"
#include "Column.hpp"
#include "Schema.hpp"
#include "StatementCreateView.hpp"
#include "StatementSelect.hpp"
#include "WinqBridge.hpp"

CPPStatementCreateView WCDBStatementCreateViewCreate(void)
{
    return WCDBCreateCPPBridgedObject(CPPStatementCreateView,
                                      new WCDB::StatementCreateView());
}

void WCDBStatementCreateViewConfigView(CPPStatementCreateView createView,
                                       const char* _Nullable name)
{
    WCDBGetObjectOrReturn(createView, WCDB::StatementCreateView, cppCreateView);
    cppCreateView->createView(name);
}

void WCDBStatementCreateViewConfigSchema(CPPStatementCreateView createView, CPPSchema schema)
{
    WCDBGetObjectOrReturn(createView, WCDB::StatementCreateView, cppCreateView);
    WCDBGetObjectOrReturn(schema, WCDB::Schema, cppSchema);
    cppCreateView->schema(*cppSchema);
}

void WCDBStatementCreateViewConfigTemp(CPPStatementCreateView createView)
{
    WCDBGetObjectOrReturn(createView, WCDB::StatementCreateView, cppCreateView);
    cppCreateView->temp();
}

void WCDBStatementCreateViewConfigIfNotExist(CPPStatementCreateView createView)
{
    WCDBGetObjectOrReturn(createView, WCDB::StatementCreateView, cppCreateView);
    cppCreateView->ifNotExists();
}

void WCDBStatementCreateViewConfigAs(CPPStatementCreateView createView, CPPStatementSelect select)
{
    WCDBGetObjectOrReturn(createView, WCDB::StatementCreateView, cppCreateView);
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    cppCreateView->as(*cppSelect);
}

void WCDBStatementCreateViewConfigColumns(CPPStatementCreateView createView,
                                          const CPPColumn* _Nullable columns,
                                          int columnNum)
{
    WCDBGetObjectOrReturn(createView, WCDB::StatementCreateView, cppCreateView);
    WCDBGetCPPSyntaxListOrReturn(WCDB::Column, cppColumns, columns, columnNum);
    cppCreateView->syntax().columns = cppColumns;
}
