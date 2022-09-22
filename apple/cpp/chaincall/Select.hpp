//
// Created by qiuwenchen on 2022/8/31.
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

#include <WCDB/CPPORM.h>
#include <WCDB/ChainCall.hpp>

namespace WCDB {

template<class ObjectType>
class Select final : public ChainCall<StatementSelect> {
    friend class TableORMOperation<ObjectType>;
    friend class HandleORMOperation;

    WCDB_CPP_ORM_STATIC_ASSERT_FOR_OBJECT_TYPE

public:
    ~Select() override final = default;

    Select<ObjectType> &where(const Expression &condition)
    {
        m_statement.where(condition);
        return *this;
    }

    Select<ObjectType> &orders(const OrderingTerms &orders)
    {
        m_statement.orders(orders);
        return *this;
    }

    Select<ObjectType> &limit(const Expression &limit)
    {
        m_statement.limit(limit);
        return *this;
    }

    Select<ObjectType> &offset(const Expression &offset)
    {
        m_statement.offset(offset);
        return *this;
    }

    Select<ObjectType> &fromTable(const UnsafeStringView &tableName)
    {
        m_statement.from(tableName);
        return *this;
    }

    Select<ObjectType> &onResultColumns(const ResultFields &resultFields)
    {
        m_fields = resultFields;
        m_statement.select(resultFields);
        return *this;
    }

    std::optional<ValueArray<ObjectType>> allObjects()
    {
        std::optional<ValueArray<ObjectType>> objects;
        if (prepareStatement()) {
            objects = m_handle->getAllObject<ObjectType>(m_fields);
            m_handle->finalize();
        }
        m_handle->invalidate();
        return objects;
    }

    std::optional<ObjectType> firstObject()
    {
        std::optional<ObjectType> object;
        if (prepareStatement()) {
            if (m_handle->step() && !m_handle->done()) {
                object = m_handle->getObject<ObjectType>(m_fields);
            }
            m_handle->finalize();
        }
        m_handle->invalidate();
        return object;
    }

protected:
    Select(Recyclable<InnerDatabase *> databaseHolder)
    : ChainCall(databaseHolder)
    {
    }

private:
    bool prepareStatement()
    {
        if (m_handle->isPrepared()) {
            return true;
        }
        if (m_fields.size() == 0) {
            m_fields = ObjectType::allFields();
        }
        if (m_statement.syntax().select.resultColumns.size() == 0) {
            m_statement.select(m_fields);
        }
        if (m_statement.syntax().orderingTerms.empty()) {
            const Binding &binding = ObjectType::getObjectRelationBinding();
            const StatementCreateTable &statement = binding.statementTable;
            if (!statement.syntax().withoutRowid) {
                m_statement.order(OrderingTerm::ascendingRowid());
            }
        }
        return m_handle->prepare(m_statement);
    }
    ResultFields m_fields;
};

} //namespace WCDB
