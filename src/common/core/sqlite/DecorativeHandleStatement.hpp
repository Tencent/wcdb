//
// Created by qiuwenchen on 2023/11/14.
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

#include "ClassDecorator.hpp"
#include "HandleStatement.hpp"

namespace WCDB {

enum class HandleStatementFuncType : unsigned char {
    prepare = 0,
    finalize,
    step,
    reset,
    clearBindings,
    bindInteger,
    bindDouble,
    bindText,
    bindText16,
    bindBLOB,
    bindNull,
    bindPointer,
    Count,
};

class DecorativeHandleStatement;

class HandleStatementDecorator
: public ClassDecorator<HandleStatementFuncType, (unsigned char) HandleStatementFuncType::Count> {
public:
    AbstractHandle *getHandle() const;
    DecorativeHandleStatement *getHandleStatement() const;

    using Integer = HandleStatement::Integer;
    using Text = HandleStatement::Text;
    using Float = HandleStatement::Float;
    using BLOB = HandleStatement::BLOB;

    virtual bool prepare(const Statement &statement);
    WCDBAssertFunctionTypeConsistent(HandleStatementDecorator, HandleStatement, prepare);

    virtual void finalize();
    WCDBAssertFunctionTypeConsistent(HandleStatementDecorator, HandleStatement, finalize)

    virtual bool step();
    WCDBAssertFunctionTypeConsistent(HandleStatementDecorator, HandleStatement, step);

    virtual void reset();
    WCDBAssertFunctionTypeConsistent(HandleStatementDecorator, HandleStatement, reset);

    virtual void clearBindings();
    WCDBAssertFunctionTypeConsistent(HandleStatementDecorator, HandleStatement, clearBindings);

    virtual void bindInteger(const Integer &value, int index);
    WCDBAssertFunctionTypeConsistent(HandleStatementDecorator, HandleStatement, bindInteger);

    virtual void bindDouble(const Float &value, int index);
    WCDBAssertFunctionTypeConsistent(HandleStatementDecorator, HandleStatement, bindDouble);

    virtual void bindText(const Text &value, int index);
    WCDBAssertFunctionTypeConsistent(HandleStatementDecorator, HandleStatement, bindText);

    virtual void bindText16(const char16_t *value, size_t valueLength, int index);
    WCDBAssertFunctionTypeConsistent(HandleStatementDecorator, HandleStatement, bindText16);

    virtual void bindBLOB(const BLOB &value, int index);
    WCDBAssertFunctionTypeConsistent(HandleStatementDecorator, HandleStatement, bindText16);

    virtual void bindNull(int index);
    WCDBAssertFunctionTypeConsistent(HandleStatementDecorator, HandleStatement, bindNull);

    virtual void
    bindPointer(void *ptr, int index, const Text &type, void (*destructor)(void *));
    WCDBAssertFunctionTypeConsistent(HandleStatementDecorator, HandleStatement, bindPointer);
};

class DecorativeHandleStatement final
: public HandleStatement,
  public DecorativeClass<HandleStatementFuncType, (unsigned char) HandleStatementFuncType::Count> {
private:
    using Super = HandleStatement;
    using SuperDecorativeClass
    = DecorativeClass<HandleStatementFuncType, (unsigned char) HandleStatementFuncType::Count>;

#pragma mark - Basic
public:
    DecorativeHandleStatement(AbstractHandle *handle);
    DecorativeHandleStatement() = delete;
    DecorativeHandleStatement(const DecorativeHandleStatement &) = delete;
    DecorativeHandleStatement &operator=(const DecorativeHandleStatement &) = delete;
    DecorativeHandleStatement(DecorativeHandleStatement &&other);
    ~DecorativeHandleStatement() override;

#pragma mark - Decoration
public:
    void initDecorativeFunctions() override final;

#pragma mark - Override
public:
    bool prepare(const Statement &statement) override final;
    void finalize() override final;
    bool step() override final;
    void reset() override final;
    void clearBindings() override final;

    void bindInteger(const Integer &value, int index = 1) override final;
    void bindDouble(const Float &value, int index = 1) override final;
    void bindText(const Text &value, int index = 1) override final;
    void bindText16(const char16_t *value, size_t valueLength, int index = 1) override final;
    void bindBLOB(const BLOB &value, int index = 1) override final;
    void bindNull(int index) override final;
    void bindPointer(void *ptr, int index, const Text &type, void (*destructor)(void *)) override final;
};

} //namespace WCDB
