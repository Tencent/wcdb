//
// Created by 陈秋文 on 2022/8/30.
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
#include "CaseInsensitiveList.hpp"
#include "ChainCall.hpp"
#include "ValueArray.hpp"
#include <assert.h>

namespace WCDB {

template<class ObjectType>
class Insert final : public ChainCall<StatementInsert> {
    friend class TableORMOperation<ObjectType>;
    friend class HandleORMOperation;

    WCDB_CPP_ORM_STATIC_ASSERT_FOR_OBJECT_TYPE

public:
    ~Insert() override final = default;

    /**
     @brief WINQ interface for SQL.
     @return this
     */
    Insert<ObjectType>& orReplace()
    {
        m_statement.orReplace();
        return *this;
    }

    /**
     @brief WINQ interface for SQL.
     @return this
     */
    Insert<ObjectType>& orIgnore()
    {
        m_statement.orIgnore();
        return *this;
    }

    /**
     @brief WINQ interface for SQL.
     @param table The name of the table to insert objects to.
     @return this
     */
    Insert<ObjectType>& intoTable(const UnsafeStringView& table)
    {
        m_statement.insertIntoTable(table);
        return *this;
    }

    /**
     @brief WINQ interface for SQL.
     @param fields Do a partial insertion with the specific fields.
     @return this
     */
    Insert<ObjectType>& onFields(const Fields& fields)
    {
        m_fields = fields;
        m_statement.columns(fields).values(BindParameter::bindParameters(fields.size()));
        return *this;
    }

    /**
     @brief Inset an array of objects.
     @param values Objects to be inserted into table.
     @return this.
     */
    Insert<ObjectType>& values(const ValueArray<ObjectType> values)
    {
        m_values = values;
        return *this;
    }

    /**
     @brief Execute the insert statement.
            Note that it will run embedded transaction while values.count>1 .
            The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     @return True if no error occurs.
     */
    bool execute()
    {
        bool succeed = true;
        if (m_values.size() > 0) {
            if (m_values.size() > 1) {
                succeed = m_handle->runTransaction([&](Handle& handle) {
                    WCDB_UNUSED(handle);
                    return realExecute();
                });
            } else {
                succeed = realExecute();
            }
        }
        m_handle->invalidate();
        return succeed;
    }

protected:
    Insert(Recyclable<InnerDatabase*> databaseHolder)
    : ChainCall(databaseHolder)
    {
    }

private:
    bool realExecute()
    {
        if (m_fields.size() == 0) {
            m_fields = ObjectType::allFields();
        }
        if (m_statement.syntax().columns.empty()) {
            m_statement.columns(m_fields).values(
            BindParameter::bindParameters(m_fields.size()));
        }
        std::vector<bool> autoIncrementsOfDefinitions;
        if (!m_statement.syntax().conflictActionValid()) {
            const CaseInsensitiveList<ColumnDef>& columnDefs
            = ObjectType::getObjectRelationBinding().getColumnDefs();
            for (const Field& field : m_fields) {
                // auto increment?
                auto iter = columnDefs.caseInsensitiveFind(field.getDescription());
                if (iter == columnDefs.end()) {
                    assertError("Related field is not found.");
                    return false;
                }
                autoIncrementsOfDefinitions.push_back(iter->second.syntax().isAutoIncrement());
            }
        }
        bool succeed = false;
        if (m_handle->prepare(m_statement)) {
            succeed = true;

            for (ObjectType& value : m_values) {
                m_handle->reset();
                int index = 1;
                assert(!value.isAutoIncrement
                       || !m_statement.syntax().conflictActionValid());
                for (const Field& field : m_fields) {
                    if (autoIncrementsOfDefinitions.empty()
                        || !autoIncrementsOfDefinitions[index - 1] || !value.isAutoIncrement) {
                        m_handle->bindObject(value, field, index);
                    } else {
                        m_handle->bindNull(index);
                    }
                    ++index;
                }
                if (!m_handle->step()) {
                    succeed = false;
                    break;
                }
                *value.lastInsertedRowID = m_handle->getLastInsertedRowID();
            }
            assignChanges();
            m_handle->finalize();
        }
        return succeed;
    }

    Fields m_fields;
    ValueArray<ObjectType> m_values;
};

} // namespace WCDB
