//
// Created by qiuwenchen on 2021/10/17.
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

#include "AuxiliaryFunctionModule.hpp"
#include "Assertion.hpp"
#include "SQLite.h"

namespace WCDB {

FTS5AuxiliaryFunctionAPI::FTS5AuxiliaryFunctionAPI(const FTS5AuxiliaryFunctionAPIPointer* ptr,
                                                   FTS5AuxiliaryFunctionContext* functionContext,
                                                   SQLiteContext* sqliteContext,
                                                   SQLiteValue** values,
                                                   int valueNum)
: ScalarFunctionAPI(sqliteContext, values, valueNum)
, m_ptr(ptr)
, m_functionContext(functionContext)
, m_queryCallback(nullptr)
{
}

AbstractFTS5AuxiliaryFunctionObject* FTS5AuxiliaryFunctionAPI::getOrCreateFunctionObject(
std::function<AbstractFTS5AuxiliaryFunctionObject*(void*)> createHandler,
void (*freeHandler)(void*))
{
    AbstractFTS5AuxiliaryFunctionObject* funcObject
    = (AbstractFTS5AuxiliaryFunctionObject*) ((Fts5ExtensionApi*) m_ptr)
      ->xGetAuxdata((Fts5Context*) m_functionContext, false);
    if (funcObject == nullptr) {
        WCTAssert(createHandler != nullptr);
        if (createHandler == nullptr) {
            return nullptr;
        }
        void* userData
        = ((Fts5ExtensionApi*) m_ptr)->xUserData((Fts5Context*) m_functionContext);
        funcObject = createHandler(userData);
        ((Fts5ExtensionApi*) m_ptr)->xSetAuxdata((Fts5Context*) m_functionContext, (void*) funcObject, freeHandler);
    }
    return funcObject;
}

int FTS5AuxiliaryFunctionAPI::getColumnCount()
{
    return ((Fts5ExtensionApi*) m_ptr)->xColumnCount((Fts5Context*) m_functionContext);
}

int FTS5AuxiliaryFunctionAPI::getRowCount(int64_t* rowCount)
{
    return ((Fts5ExtensionApi*) m_ptr)->xRowCount((Fts5Context*) m_functionContext, (sqlite3_int64*) rowCount);
}

int64_t FTS5AuxiliaryFunctionAPI::getRowid()
{
    return ((Fts5ExtensionApi*) m_ptr)->xRowid((Fts5Context*) m_functionContext);
}

int FTS5AuxiliaryFunctionAPI::getColumnSize(int column, int* columnSize)
{
    return ((Fts5ExtensionApi*) m_ptr)->xColumnSize((Fts5Context*) m_functionContext, column, columnSize);
}

int FTS5AuxiliaryFunctionAPI::getColumnTotalSize(int column, int64_t* columnTotalSize)
{
    return ((Fts5ExtensionApi*) m_ptr)
    ->xColumnTotalSize((Fts5Context*) m_functionContext, column, (sqlite3_int64*) columnTotalSize);
}

int FTS5AuxiliaryFunctionAPI::getPhraseCount()
{
    return ((Fts5ExtensionApi*) m_ptr)->xPhraseCount((Fts5Context*) m_functionContext);
}

int FTS5AuxiliaryFunctionAPI::getPhraseSize(int phraseNum)
{
    return ((Fts5ExtensionApi*) m_ptr)->xPhraseSize((Fts5Context*) m_functionContext, phraseNum);
}

int FTS5AuxiliaryFunctionAPI::instCount(int* count)
{
    return ((Fts5ExtensionApi*) m_ptr)->xInstCount((Fts5Context*) m_functionContext, count);
}

int FTS5AuxiliaryFunctionAPI::inst(int instIndex, int* phraseNum, int* column, int* instOffset)
{
    return ((Fts5ExtensionApi*) m_ptr)
    ->xInst((Fts5Context*) m_functionContext, instIndex, phraseNum, column, instOffset);
}

int FTS5AuxiliaryFunctionAPI::getTextForThisRow(int column, UnsafeStringView& output)
{
    const char* text = nullptr;
    int textLength = 0;
    int ret
    = ((Fts5ExtensionApi*) m_ptr)->xColumnText((Fts5Context*) m_functionContext, column, &text, &textLength);
    if (FTSError::isOK(ret)) {
        output = UnsafeStringView(text, textLength);
    }
    return ret;
}

int FTS5AuxiliaryFunctionAPI::tokenize(const UnsafeStringView& text,
                                       void* context,
                                       int (*tokenCallback)(void*, int, const char*, int, int, int))
{
    return ((Fts5ExtensionApi*) m_ptr)
    ->xTokenize(
    (Fts5Context*) m_functionContext, text.data(), (int) text.length(), context, tokenCallback);
}

int FTS5AuxiliaryFunctionAPI::queryPhrase(int phraseNum,
                                          void* userData,
                                          int (*callback)(FTS5AuxiliaryFunctionAPI&, void*))
{
    WCTRemedialAssert(
    callback != nullptr, "callback should not be null", return FTSError::Error(););
    m_queryCallback = [userData, callback](FTS5AuxiliaryFunctionAPI& apiObj) {
        return callback(apiObj, userData);
    };
    int ret = ((Fts5ExtensionApi*) m_ptr)
              ->xQueryPhrase((Fts5Context*) m_functionContext,
                             phraseNum,
                             (void*) this,
                             (int (*)(const Fts5ExtensionApi*, Fts5Context*, void*)) queryCallBack);
    m_queryCallback = nullptr;
    return ret;
}

int FTS5AuxiliaryFunctionAPI::queryCallBack(const FTS5AuxiliaryFunctionAPIPointer* apiPointer,
                                            FTS5AuxiliaryFunctionContext* functionContext,
                                            void* queryContext)
{
    FTS5AuxiliaryFunctionAPI* apiObj = (FTS5AuxiliaryFunctionAPI*) queryContext;
    FTS5AuxiliaryFunctionAPI newAPIObj
    = FTS5AuxiliaryFunctionAPI(apiPointer, functionContext, nullptr, nullptr, 0);
    WCTAssert(apiObj->m_queryCallback != nullptr);
    return apiObj->m_queryCallback(newAPIObj);
}

AbstractFTS5AuxiliaryFunctionObject::AbstractFTS5AuxiliaryFunctionObject(
void* userContext, FTS5AuxiliaryFunctionAPI& apiObj)
{
    WCDB_UNUSED(userContext);
    WCDB_UNUSED(apiObj);
}

AbstractFTS5AuxiliaryFunctionObject::~AbstractFTS5AuxiliaryFunctionObject() = default;

FTS5AuxiliaryFunctionModule::FTS5AuxiliaryFunctionModule(const AuxiliaryFunction& func,
                                                         void* context)
: m_func(func), m_context(context)
{
}

FTS5AuxiliaryFunctionModule::AuxiliaryFunction FTS5AuxiliaryFunctionModule::getFunc() const
{
    return m_func;
}

void* FTS5AuxiliaryFunctionModule::getContext() const
{
    return m_context;
}

} // namespace WCDB
