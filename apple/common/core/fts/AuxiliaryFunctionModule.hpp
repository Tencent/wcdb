//
// Created by 陈秋文 on 2021/10/17.
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

#import <WCDB/FTSError.hpp>
#import <WCDB/StringView.hpp>
#include <functional>

typedef struct FTS5AuxiliaryFunctionValue FTS5AuxiliaryFunctionValue;
typedef struct FTS5AuxiliaryFunctionAPIPointer FTS5AuxiliaryFunctionAPIPointer;
typedef struct FTS5AuxiliaryFunctionContext FTS5AuxiliaryFunctionContext;
typedef struct FTS5SQLiteContext FTS5SQLiteContext;

namespace WCDB {

class AbstractFTS5AuxiliaryFunctionObject;

class FTS5AuxiliaryFunctionAPI {
public:
    FTS5AuxiliaryFunctionAPI(const FTS5AuxiliaryFunctionAPIPointer* ptr,
                             FTS5SQLiteContext* sqliteContext,
                             FTS5AuxiliaryFunctionContext* functionContext);

    AbstractFTS5AuxiliaryFunctionObject*
    getOrCreateFunctionObject(std::function<AbstractFTS5AuxiliaryFunctionObject*(void*)> createHandler,
                              void (*freeHandler)(void*));

    // Access value
    const void* getBlobValue(FTS5AuxiliaryFunctionValue* value);
    double getDoubleValue(FTS5AuxiliaryFunctionValue* value);
    int64_t getIntValue(FTS5AuxiliaryFunctionValue* value);
    const UnsafeStringView getTextValue(FTS5AuxiliaryFunctionValue* value);
    int getBytesValue(FTS5AuxiliaryFunctionValue* value);

    // Output result
    void setBlobResult(const void* resultBufffer, int length, void (*freeHandler)(void*));
    void setDoubleResult(double result);
    void setIntResult(int64_t result);
    void setNullResult();
    void setTextResult(const UnsafeStringView& result);
    void setErrorResult(const UnsafeStringView& msg, int code);

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
                    int (*callback)(const FTS5AuxiliaryFunctionAPI*, void*));

private:
    static int queryCallBack(const FTS5AuxiliaryFunctionAPIPointer* apiPointer,
                             FTS5AuxiliaryFunctionContext* functionContext,
                             void* queryContext);

    const FTS5AuxiliaryFunctionAPIPointer* m_ptr;
    FTS5SQLiteContext* m_sqliteContext;
    FTS5AuxiliaryFunctionContext* m_functionContext;
    std::function<int(const FTS5AuxiliaryFunctionAPI*)> m_queryCallback;
};

class AbstractFTS5AuxiliaryFunctionObject {
public:
    AbstractFTS5AuxiliaryFunctionObject(int nVal,
                                        FTS5AuxiliaryFunctionValue** apVal,
                                        void* context,
                                        FTS5AuxiliaryFunctionAPI* apiObj);
    virtual ~AbstractFTS5AuxiliaryFunctionObject() = 0;
    virtual void process(FTS5AuxiliaryFunctionAPI* apiObj) = 0;
};

class FTS5AuxiliaryFunctionModule final {
public:
    typedef void (*AuxiliaryFunction)(const FTS5AuxiliaryFunctionAPIPointer* pApi,
                                      FTS5AuxiliaryFunctionContext* pFts,
                                      FTS5SQLiteContext* pCtx,
                                      int nVal,
                                      FTS5AuxiliaryFunctionValue** apVal);
    FTS5AuxiliaryFunctionModule();
    FTS5AuxiliaryFunctionModule(const AuxiliaryFunction& func, void* context = nullptr);
    AuxiliaryFunction getFunc() const;
    void* getContext() const;

private:
    AuxiliaryFunction m_func;
    void* m_context;
};

} // namespace WCDB
