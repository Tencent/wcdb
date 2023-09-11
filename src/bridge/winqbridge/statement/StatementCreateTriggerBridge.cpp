//
// Created by qiuwenchen on 2022/7/13.
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

#include "StatementCreateTriggerBridge.h"
#include "Column.hpp"
#include "Schema.hpp"
#include "StatementCreateTrigger.hpp"
#include "StatementDelete.hpp"
#include "StatementInsert.hpp"
#include "StatementSelect.hpp"
#include "StatementUpdate.hpp"
#include "WinqBridge.hpp"

CPPStatementCreateTrigger WCDBStatementCreateTriggerCreate(void)
{
    return WCDBCreateCPPBridgedObject(CPPStatementCreateTrigger, WCDB::StatementCreateTrigger);
}

void WCDBStatementCreateTriggerConfigTrigger(CPPStatementCreateTrigger createTrigger,
                                             const char* _Nullable name)
{
    WCDBGetObjectOrReturn(createTrigger, WCDB::StatementCreateTrigger, cppCreateTrigger);
    cppCreateTrigger->createTrigger(name);
}

void WCDBStatementCreateTriggerConfigSchema(CPPStatementCreateTrigger createTrigger,
                                            CPPSchema schema)
{
    WCDBGetObjectOrReturn(createTrigger, WCDB::StatementCreateTrigger, cppCreateTrigger);
    WCDBGetObjectOrReturn(schema, WCDB::Schema, cppSchema);
    cppCreateTrigger->schema(*cppSchema);
}

void WCDBStatementCreateTriggerConfigSchema2(CPPStatementCreateTrigger createTrigger,
                                             CPPCommonValue schema)
{
    WCDBGetObjectOrReturn(createTrigger, WCDB::StatementCreateTrigger, cppCreateTrigger);
    cppCreateTrigger->schema(WCDBCreateSchemaFromCommonValue(schema));
}

void WCDBStatementCreateTriggerConfigTemp(CPPStatementCreateTrigger createTrigger)
{
    WCDBGetObjectOrReturn(createTrigger, WCDB::StatementCreateTrigger, cppCreateTrigger);
    cppCreateTrigger->temp();
}

void WCDBStatementCreateTriggerConfigIfNotExist(CPPStatementCreateTrigger createTrigger)
{
    WCDBGetObjectOrReturn(createTrigger, WCDB::StatementCreateTrigger, cppCreateTrigger);
    cppCreateTrigger->ifNotExists();
}

void WCDBStatementCreateTriggerConfigBefore(CPPStatementCreateTrigger createTrigger)
{
    WCDBGetObjectOrReturn(createTrigger, WCDB::StatementCreateTrigger, cppCreateTrigger);
    cppCreateTrigger->before();
}

void WCDBStatementCreateTriggerConfigAfter(CPPStatementCreateTrigger createTrigger)
{
    WCDBGetObjectOrReturn(createTrigger, WCDB::StatementCreateTrigger, cppCreateTrigger);
    cppCreateTrigger->after();
}
void WCDBStatementCreateTriggerConfigInsteadOf(CPPStatementCreateTrigger createTrigger)
{
    WCDBGetObjectOrReturn(createTrigger, WCDB::StatementCreateTrigger, cppCreateTrigger);
    cppCreateTrigger->insteadOf();
}

void WCDBStatementCreateTriggerConfigDelete(CPPStatementCreateTrigger createTrigger)
{
    WCDBGetObjectOrReturn(createTrigger, WCDB::StatementCreateTrigger, cppCreateTrigger);
    cppCreateTrigger->delete_();
}

void WCDBStatementCreateTriggerConfigInsert(CPPStatementCreateTrigger createTrigger)
{
    WCDBGetObjectOrReturn(createTrigger, WCDB::StatementCreateTrigger, cppCreateTrigger);
    cppCreateTrigger->insert();
}

void WCDBStatementCreateTriggerConfigUpdate(CPPStatementCreateTrigger createTrigger)
{
    WCDBGetObjectOrReturn(createTrigger, WCDB::StatementCreateTrigger, cppCreateTrigger);
    cppCreateTrigger->update();
}

void WCDBStatementCreateTriggerConfigColumns(CPPStatementCreateTrigger createTrigger,
                                             const CPPColumn* _Nullable columns,
                                             int colNum)
{
    WCDBGetObjectOrReturn(createTrigger, WCDB::StatementCreateTrigger, cppCreateTrigger);
    WCDBGetCPPSyntaxListOrReturn(WCDB::Column, cppColumns, columns, colNum);
    cppCreateTrigger->syntax().columns = cppColumns;
}

void WCDBStatementCreateTriggerConfigColumns2(CPPStatementCreateTrigger createTrigger,
                                              CPPCommonArray columns)
{
    WCDBGetObjectOrReturn(createTrigger, WCDB::StatementCreateTrigger, cppCreateTrigger);
    WCDB::Columns cppColumns;
    for (int i = 0; i < columns.length; i++) {
        switch (columns.type) {
        case WCDBBridgedType_String:
            cppColumns.emplace_back(WCDB::UnsafeStringView(
            WCDBGetCommonArrayLiteralValue(const char*, columns, i)));
            break;
        case WCDBBridgedType_Column:
            cppColumns.push_back(WCDBGetCommonArrayObject(WCDB::Column, columns, i));
            break;
        default:
            assert(0);
            break;
        }
    }
    cppCreateTrigger->syntax().columns = cppColumns;
}

void WCDBStatementCreateTriggerConfigTable(CPPStatementCreateTrigger createTrigger,
                                           const char* _Nullable name)
{
    WCDBGetObjectOrReturn(createTrigger, WCDB::StatementCreateTrigger, cppCreateTrigger);
    cppCreateTrigger->on(name);
}

void WCDBStatementCreateTriggerConfigForEachRow(CPPStatementCreateTrigger createTrigger)
{
    WCDBGetObjectOrReturn(createTrigger, WCDB::StatementCreateTrigger, cppCreateTrigger);
    cppCreateTrigger->forEachRow();
}

void WCDBStatementCreateTriggerConfigWhen(CPPStatementCreateTrigger createTrigger,
                                          CPPExpression expression)
{
    WCDBGetObjectOrReturn(createTrigger, WCDB::StatementCreateTrigger, cppCreateTrigger);
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppCreateTrigger->when(*cppExpression);
}

void WCDBStatementCreateTriggerExecuteInsert(CPPStatementCreateTrigger createTrigger,
                                             CPPStatementInsert insert)
{
    WCDBGetObjectOrReturn(createTrigger, WCDB::StatementCreateTrigger, cppCreateTrigger);
    WCDBGetObjectOrReturn(insert, WCDB::StatementInsert, cppInsert);
    cppCreateTrigger->execute(*cppInsert);
}

void WCDBStatementCreateTriggerExecuteUpdate(CPPStatementCreateTrigger createTrigger,
                                             CPPStatementUpdate update)
{
    WCDBGetObjectOrReturn(createTrigger, WCDB::StatementCreateTrigger, cppCreateTrigger);
    WCDBGetObjectOrReturn(update, WCDB::StatementUpdate, cppUpdate);
    cppCreateTrigger->execute(*cppUpdate);
}

void WCDBStatementCreateTriggerExecuteDelete(CPPStatementCreateTrigger createTrigger,
                                             CPPStatementDelete delete_)
{
    WCDBGetObjectOrReturn(createTrigger, WCDB::StatementCreateTrigger, cppCreateTrigger);
    WCDBGetObjectOrReturn(delete_, WCDB::StatementDelete, cppDelete);
    cppCreateTrigger->execute(*cppDelete);
}

void WCDBStatementCreateTriggerExecuteSelect(CPPStatementCreateTrigger createTrigger,
                                             CPPStatementSelect select)
{
    WCDBGetObjectOrReturn(createTrigger, WCDB::StatementCreateTrigger, cppCreateTrigger);
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    cppCreateTrigger->execute(*cppSelect);
}
