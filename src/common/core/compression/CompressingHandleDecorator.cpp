//
// Created by qiuwenchen on 2023/11/28.
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

#include "CompressingHandleDecorator.hpp"
#include "CompressingStatementDecorator.hpp"

namespace WCDB {

CompressingHandleDecorator::CompressingHandleDecorator(Compression& compression)
: HandleDecorator(), Compression::Binder(compression)
{
}

CompressingHandleDecorator::~CompressingHandleDecorator() = default;

void CompressingHandleDecorator::decorate(Decorative* handle)
{
    Super::decorate(handle);
    WCDBSwizzleDecorativeFunction(handle, CompressingHandleDecorator, getStatement);
    WCDBSwizzleDecorativeFunction(handle, CompressingHandleDecorator, commitTransaction);
    WCDBSwizzleDecorativeFunction(handle, CompressingHandleDecorator, rollbackTransaction);
}

InnerHandle* CompressingHandleDecorator::getCurrentHandle() const
{
    return getHandle();
}

DecorativeHandleStatement*
CompressingHandleDecorator::getStatement(const UnsafeStringView& skipDecorator)
{
    DecorativeHandleStatement* statement = Super::getStatement(skipDecorator);
    if (!statement->containDecorator(DecoratorCompressingHandleStatement)
        && !skipDecorator.equal(DecoratorCompressingHandleStatement)
        && !skipDecorator.equal(DecoratorAllType)) {
        statement->tryAddDecorator<CompressingStatementDecorator>(
        DecoratorCompressingHandleStatement, this);
    }
    return statement;
}

bool CompressingHandleDecorator::commitTransaction()
{
    bool ret = Super::commitTransaction();
    if (ret && !getHandle()->isInTransaction()) {
        notifyTransactionCommitted(true);
    }
    return ret;
}

void CompressingHandleDecorator::rollbackTransaction()
{
    Super::rollbackTransaction();
    notifyTransactionCommitted(false);
}

} // namespace WCDB
