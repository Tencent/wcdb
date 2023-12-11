//
// Created by qiuwenchen on 2023/11/15.
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
#include "DecorativeHandleStatement.hpp"
#include "HandleOperator.hpp"
#include "InnerHandle.hpp"

namespace WCDB {

enum class HandleFuncType : unsigned char {
    getColumns = 0,
    addColumn,
    commitTransaction,
    rollbackTransaction,
    getStatement,
    finalizeStatements,
    Count,
};

class DecorativeHandle;

class HandleDecorator
: public ClassDecorator<HandleFuncType, (unsigned char) HandleFuncType::Count> {
public:
    DecorativeHandle *getHandle() const;

    virtual Optional<StringViewSet>
    getColumns(const Schema &schema, const UnsafeStringView &table);
    WCDBAssertFunctionTypeConsistent(HandleDecorator, InnerHandle, getColumns);

    virtual bool
    addColumn(const Schema &schema, const UnsafeStringView &table, const ColumnDef &column);
    WCDBAssertFunctionTypeConsistent(HandleDecorator, InnerHandle, addColumn);

    virtual bool commitTransaction();
    WCDBAssertFunctionTypeConsistent(HandleDecorator, InnerHandle, commitTransaction);

    virtual void rollbackTransaction();
    WCDBAssertFunctionTypeConsistent(HandleDecorator, InnerHandle, rollbackTransaction);

    virtual DecorativeHandleStatement *
    getStatement(const UnsafeStringView &skipDecorator = UnsafeStringView());
    WCDBAssertFunctionTypeConsistent(HandleDecorator, InnerHandle, getStatement);

    virtual void finalizeStatements();
    WCDBAssertFunctionTypeConsistent(HandleDecorator, InnerHandle, finalizeStatements);
};

class DecorativeHandle
: public DecorativeClass<HandleFuncType, (unsigned char) HandleFuncType::Count>,
  public InnerHandle {
private:
    using Super = InnerHandle;
    using SuperDecorativeClass
    = DecorativeClass<HandleFuncType, (unsigned char) HandleFuncType::Count>;

#pragma mark - Decoration
public:
    void onDecorationChange() override final;
    void initDecorativeFunctions() override final;

#pragma mark - Operator
public:
    template<typename Operator, typename... Args>
    Operator &getOrCreateOperator(const UnsafeStringView &name, const Args &...args)
    {
        static_assert(std::is_base_of<HandleOperator, Operator>::value, "");
        auto iter = m_operators.find(name);
        if (iter != m_operators.end()) {
            return *(static_cast<Operator *>(iter->second.get()));
        } else {
            std::shared_ptr<HandleOperator> newOperator
            = std::make_shared<Operator>(this, args...);
            m_operators.emplace(name, newOperator);
            return *(static_cast<Operator *>(newOperator.get()));
        }
    }

private:
    StringViewMap<std::shared_ptr<HandleOperator>> m_operators;

#pragma mark - Override
public:
    Optional<StringViewSet>
    getColumns(const Schema &schema, const UnsafeStringView &table) override final;
    bool addColumn(const Schema &schema,
                   const UnsafeStringView &table,
                   const ColumnDef &column) override final;

    bool commitTransaction() override final;
    void rollbackTransaction() override final;
    DecorativeHandleStatement *getStatement(const UnsafeStringView &skipDecorator
                                            = UnsafeStringView()) override final;
    void finalizeStatements() override final;
};

} // namespace WCDB
