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

#include "DecorativeHandleStatement.hpp"

namespace WCDB {

#pragma mark - HandleStatementDecorator
DecorativeHandleStatement *HandleStatementDecorator::getHandleStatement() const
{
    return static_cast<DecorativeHandleStatement *>(m_decoratedObj);
}

AbstractHandle *HandleStatementDecorator::getHandle() const
{
    return getHandleStatement()->getHandle();
}

bool HandleStatementDecorator::prepare(const Statement &statement)
{
    return WCDBCallDecoratedFunction(HandleStatement, prepare, statement);
}

void HandleStatementDecorator::finalize()
{
    WCDBCallDecoratedFunction(HandleStatement, finalize);
}

bool HandleStatementDecorator::step()
{
    return WCDBCallDecoratedFunction(HandleStatement, step);
}

void HandleStatementDecorator::reset()
{
    WCDBCallDecoratedFunction(HandleStatement, reset);
}

void HandleStatementDecorator::clearBindings()
{
    WCDBCallDecoratedFunction(HandleStatement, clearBindings);
}

void HandleStatementDecorator::bindInteger(const Integer &value, int index)
{
    WCDBCallDecoratedFunction(HandleStatement, bindInteger, value, index);
}

void HandleStatementDecorator::bindDouble(const Float &value, int index)
{
    WCDBCallDecoratedFunction(HandleStatement, bindDouble, value, index);
}

void HandleStatementDecorator::bindText(const Text &value, int index)
{
    WCDBCallDecoratedFunction(HandleStatement, bindText, value, index);
}

void HandleStatementDecorator::bindText16(const char16_t *value, size_t valueLength, int index)
{
    WCDBCallDecoratedFunction(HandleStatement, bindText16, value, valueLength, index);
}

void HandleStatementDecorator::bindBLOB(const BLOB &value, int index)
{
    WCDBCallDecoratedFunction(HandleStatement, bindBLOB, value, index);
}

void HandleStatementDecorator::bindNull(int index)
{
    WCDBCallDecoratedFunction(HandleStatement, bindNull, index);
}

void HandleStatementDecorator::bindPointer(void *ptr,
                                           int index,
                                           const Text &type,
                                           void (*destructor)(void *))
{
    WCDBCallDecoratedFunction(HandleStatement, bindPointer, ptr, index, type, destructor);
}

#pragma mark - DecorativeHandleStatement - Basic
DecorativeHandleStatement::DecorativeHandleStatement(DecorativeHandleStatement &&other)
: HandleStatement(std::move(other)), SuperDecorativeClass(std::move(other))
{
}

DecorativeHandleStatement::DecorativeHandleStatement(AbstractHandle *handle)
: HandleStatement(handle), SuperDecorativeClass()
{
}

DecorativeHandleStatement::~DecorativeHandleStatement() = default;

#pragma mark - Decoration

void DecorativeHandleStatement::initDecorativeFunctions()
{
    WCDBRegisterDecorativeFunction(HandleStatement, prepare);
    WCDBRegisterDecorativeFunction(HandleStatement, finalize);
    WCDBRegisterDecorativeFunction(HandleStatement, step);
    WCDBRegisterDecorativeFunction(HandleStatement, reset);
    WCDBRegisterDecorativeFunction(HandleStatement, clearBindings);
    WCDBRegisterDecorativeFunction(HandleStatement, bindInteger);
    WCDBRegisterDecorativeFunction(HandleStatement, bindDouble);
    WCDBRegisterDecorativeFunction(HandleStatement, bindText);
    WCDBRegisterDecorativeFunction(HandleStatement, bindText16);
    WCDBRegisterDecorativeFunction(HandleStatement, bindBLOB);
    WCDBRegisterDecorativeFunction(HandleStatement, bindNull);
    WCDBRegisterDecorativeFunction(HandleStatement, bindPointer);
}

#pragma mark - DecorativeHandleStatement - Override
bool DecorativeHandleStatement::prepare(const Statement &statement)
{
    if (!WCDBCallDecorativeFunction(HandleStatement, prepare, statement)) {
        finalize();
        return false;
    }
    return true;
}

void DecorativeHandleStatement::finalize()
{
    WCDBCallDecorativeFunction(HandleStatement, finalize);
}

bool DecorativeHandleStatement::step()
{
    return WCDBCallDecorativeFunction(HandleStatement, step);
}

void DecorativeHandleStatement::reset()
{
    WCDBCallDecorativeFunction(HandleStatement, reset);
}

void DecorativeHandleStatement::clearBindings()
{
    WCDBCallDecorativeFunction(HandleStatement, clearBindings);
}

void DecorativeHandleStatement::bindInteger(const Integer &value, int index)
{
    WCDBCallDecorativeFunction(HandleStatement, bindInteger, value, index);
}

void DecorativeHandleStatement::bindDouble(const Float &value, int index)
{
    WCDBCallDecorativeFunction(HandleStatement, bindDouble, value, index);
}

void DecorativeHandleStatement::bindText(const Text &value, int index)
{
    WCDBCallDecorativeFunction(HandleStatement, bindText, value, index);
}

void DecorativeHandleStatement::bindText16(const char16_t *value, size_t valueLength, int index)
{
    WCDBCallDecorativeFunction(HandleStatement, bindText16, value, valueLength, index);
}

void DecorativeHandleStatement::bindBLOB(const BLOB &value, int index)
{
    WCDBCallDecorativeFunction(HandleStatement, bindBLOB, value, index);
}

void DecorativeHandleStatement::bindNull(int index)
{
    WCDBCallDecorativeFunction(HandleStatement, bindNull, index);
}

void DecorativeHandleStatement::bindPointer(void *ptr,
                                            int index,
                                            const Text &type,
                                            void (*destructor)(void *))
{
    WCDBCallDecorativeFunction(HandleStatement, bindPointer, ptr, index, type, destructor);
}

} //namespace WCDB
