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

#include "CPPORM.h"
#include "ChainCall.hpp"
#include "ValueArray.hpp"

namespace WCDB {

template<class ObjectType>
class Update final : public ChainCall<StatementUpdate> {
    WCDB_CPP_ORM_STATIC_ASSERT_FOR_OBJECT_TYPE
    friend class TableORMOperation<ObjectType>;
    friend class HandleORMOperation;

public:
    ~Update() override final = default;

    /**
     @brief WINQ interface for SQL.
     @param table The name of the table to update content.
     @return this
     */
    Update<ObjectType> &table(const UnsafeStringView &table)
    {
        m_statement.update(table);
        return *this;
    }

    /**
     @brief WINQ interface for SQL.
     @param fields The fields used to specify the columns where data is updated.
     @return this
     */
    Update<ObjectType> &set(const Fields &fields)
    {
        m_fields = fields;
        int bindParameterIndex = 1;
        for (const Field &field : fields) {
            m_statement.set(field).to(WCDB::BindParameter(bindParameterIndex));
            ++bindParameterIndex;
        }
        return *this;
    }

    /**
     @brief WINQ interface for SQL.
     @param condition condition
     @return this
     */
    Update<ObjectType> &where(const Expression &condition)
    {
        m_statement.where(condition);
        return *this;
    }

    /**
     @brief WINQ interface for SQL.
     @param orders order list
     @return this
     */
    Update<ObjectType> &orders(const OrderingTerms orders)
    {
        m_statement.orders(orders);
        return *this;
    }

    /**
     @brief WINQ interface for SQL.
     @param limit limit
     @return this
     */
    Update<ObjectType> &limit(const Expression &limit)
    {
        m_statement.limit(limit);
        return *this;
    }

    /**
     @brief WINQ interface for SQL.
     @param offset offset
     @return this
     */
    Update<ObjectType> &offset(const Expression &offset)
    {
        m_statement.offset(offset);
        return *this;
    }

    /**
     @brief Execute the update statement with one object.
     @param obj Object to be updated into table.
     @return this.
     */
    Update<ObjectType> &toObject(const ObjectType &obj)
    {
        m_obj = obj;
        return *this;
    }

    /**
     @brief Execute the update statement with one value.
     @param value Value to be updated into table.
     @return this.
     */
    Update<ObjectType> &toValue(const Value &value)
    {
        m_row = { value };
        return *this;
    }

    /**
     @brief Execute the update statement with one row of value.
     @param row The row to be updated into table.
     @return this.
     */
    Update<ObjectType> &toRow(const OneRowValue &row)
    {
        m_row = row;
        return *this;
    }

    /**
     @brief Execute the update statement.
     @return True if no error occurs.
     */
    bool execute()
    {
        bool result = true;
        if (m_row.succeed() || m_obj.succeed()) {
            result = false;
            if (m_handle->prepare(m_statement)) {
                if (m_obj.succeed()) {
                    m_handle->bindObject(m_obj.value(), m_fields);
                } else {
                    m_handle->bindRow(m_row.value());
                }
                result = m_handle->step();
                m_handle->finalize();
            }
        }
        m_handle->invalidate();
        return result;
    }

protected:
    Update(Recyclable<InnerDatabase *> databaseHolder)
    : ChainCall(databaseHolder)
    {
    }

private:
    Fields m_fields;
    OptionalOneRow m_row;
    Optional<ObjectType> m_obj;
};

} //namespace WCDB
