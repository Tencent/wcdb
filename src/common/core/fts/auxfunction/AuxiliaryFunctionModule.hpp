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

#pragma once

#include "FTSError.hpp"
#include "ScalarFunctionModule.hpp"
#include <functional>

typedef struct FTS5AuxiliaryFunctionAPIPointer FTS5AuxiliaryFunctionAPIPointer;
typedef struct FTS5AuxiliaryFunctionContext FTS5AuxiliaryFunctionContext;

namespace WCDB {

class AbstractFTS5AuxiliaryFunctionObject;

template<typename AuxiliaryFunctionObject>
class FTS5AuxiliaryFunctionTemplate;

class WCDB_API FTS5AuxiliaryFunctionAPI : public ScalarFunctionAPI {
    template<typename AuxiliaryFunctionObject>
    friend class FTS5AuxiliaryFunctionTemplate;

public:
    // FTS5 Extension API
    int getColumnCount();
    int getRowCount(int64_t* rowCount);
    int64_t getRowid();
    int getColumnSize(int column, int* columnSize);
    int getColumnTotalSize(int column, int64_t* columnTotalSize);
    int getPhraseCount();
    int getPhraseSize(int phraseNum);
    int instCount(int* count);
    int inst(int instIndex, int* phraseNum, int* column, int* instOffset);
    int getTextForThisRow(int column, UnsafeStringView& output);
    int tokenize(const UnsafeStringView& text,
                 void* context,
                 int (*tokenCallback)(void*, int, const char*, int, int, int));
    int queryPhrase(int phraseNum,
                    void* userData,
                    int (*callback)(FTS5AuxiliaryFunctionAPI&, void*));

protected:
    FTS5AuxiliaryFunctionAPI(const FTS5AuxiliaryFunctionAPIPointer* ptr,
                             FTS5AuxiliaryFunctionContext* functionContext,
                             SQLiteContext* sqliteContext,
                             SQLiteValue** values,
                             int valueNum);

    AbstractFTS5AuxiliaryFunctionObject*
    getOrCreateFunctionObject(std::function<AbstractFTS5AuxiliaryFunctionObject*(void*)> createHandler,
                              void (*freeHandler)(void*));

private:
    static int queryCallBack(const FTS5AuxiliaryFunctionAPIPointer* apiPointer,
                             FTS5AuxiliaryFunctionContext* functionContext,
                             void* queryContext);

    const FTS5AuxiliaryFunctionAPIPointer* m_ptr;
    FTS5AuxiliaryFunctionContext* m_functionContext;
    std::function<int(FTS5AuxiliaryFunctionAPI&)> m_queryCallback;
};

class WCDB_API AbstractFTS5AuxiliaryFunctionObject {
public:
    AbstractFTS5AuxiliaryFunctionObject(void* userContext, FTS5AuxiliaryFunctionAPI& apiObj);
    virtual ~AbstractFTS5AuxiliaryFunctionObject() = 0;
    virtual void process(FTS5AuxiliaryFunctionAPI& apiObj) = 0;
};

class WCDB_API FTS5AuxiliaryFunctionModule final {
public:
    typedef void (*AuxiliaryFunction)(const FTS5AuxiliaryFunctionAPIPointer* pApi,
                                      FTS5AuxiliaryFunctionContext* pFts,
                                      SQLiteContext* pCtx,
                                      int nVal,
                                      SQLiteValue** apVal);
    FTS5AuxiliaryFunctionModule() = delete;
    FTS5AuxiliaryFunctionModule(const AuxiliaryFunction& func, void* context = nullptr);
    AuxiliaryFunction getFunc() const;
    void* getContext() const;

private:
    AuxiliaryFunction m_func;
    void* m_context;
};

} // namespace WCDB
