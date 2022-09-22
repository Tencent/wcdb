//
// Created by qiuwenchen on 2022/4/1.
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

#include <WCDB/HandleBridge.h>
#include <WCDB/ObjectBridge.hpp>
#include <WCDB/RecyclableHandle.hpp>

CPPError WCDBHandleGetError(CPPHandle handle)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::Handle, cppHandle, CPPError());
    const WCDB::Error& error = cppHandle->getError();
    return WCDBCreateUnmanageCPPObject(CPPError, &error);
}

bool WCDBHandleCheckValid(CPPHandle handle)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::Handle, cppHandle, false);
    WCDB_UNUSED(cppHandle);
    return true;
}

CPPHandleStatement WCDBHandlePrepare(CPPHandle handle, CPPStatement statement)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::Handle, cppHandle, CPPHandleStatement());
    WCDBGetObjectOrReturnValue(
    statement, WCDB::Statement, cppStatement, CPPHandleStatement());
    WCDB::HandleStatement* handleStatement = cppHandle->getStatement();
    handleStatement->prepare(*cppStatement);

    WCDB::Recyclable<WCDB::HandleStatement*>* recycleHandleStatement
    = new WCDB::Recyclable<WCDB::HandleStatement*>(
    handleStatement, [cppHandle](WCDB::HandleStatement* handleStatement) {
        cppHandle->returnStatement(handleStatement);
    });
    return WCDBCreateRecylableCPPObject(CPPHandleStatement, recycleHandleStatement);
}

bool WCDBHandleExcute(CPPHandle handle, CPPStatement statement)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::Handle, cppHandle, false);
    WCDBGetObjectOrReturnValue(statement, WCDB::Statement, cppStatement, false);
    return cppHandle->execute(*cppStatement);
}

int WCDBHandleGetChange(CPPHandle handle)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::Handle, cppHandle, 0);
    return cppHandle->getChanges();
}

int WCDBHandleGetTotalChange(CPPHandle handle)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::Handle, cppHandle, 0);
    return cppHandle->getTotalChange();
}

signed long long WCDBHandleGetLastInsertedRowID(CPPHandle handle)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::Handle, cppHandle, 0);
    return cppHandle->getLastInsertedRowID();
}

bool WCDBHandleIsReadOnly(CPPHandle handle)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::Handle, cppHandle, true);
    return cppHandle->isReadonly();
}

bool WCDBHandleIsInTransaction(CPPHandle handle)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::Handle, cppHandle, false);
    return cppHandle->isInTransaction();
}
