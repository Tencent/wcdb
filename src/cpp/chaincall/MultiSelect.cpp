//
// Created by 陈秋文 on 2022/10/25.
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

#include "MultiSelect.hpp"
#include "Assertion.hpp"
#include "Handle.hpp"
#include "MultiObject.hpp"
#include "ResultField.hpp"

namespace WCDB {

MultiSelect::~MultiSelect() = default;

MultiSelect::MultiSelect(Recyclable<InnerDatabase *> databaseHolder)
: ChainCall(databaseHolder)
{
}

MultiSelect &MultiSelect::fromTables(ValueArray<StringView> tableNames)
{
    WCTRemedialAssert(tableNames.size() != 0, "Table names can't be empty.", return *this;);
    TablesOrSubqueries tables;
    for (const StringView &name : tableNames) {
        tables.push_back(name);
    }
    m_statement.from(tables);
    return *this;
}

MultiSelect &MultiSelect::where(const Expression &condition)
{
    m_statement.where(condition);
    return *this;
}

MultiSelect &MultiSelect::orders(const OrderingTerms &orders)
{
    m_statement.orders(orders);
    return *this;
}

MultiSelect &MultiSelect::limit(const Expression &limit)
{
    m_statement.limit(limit);
    return *this;
}

MultiSelect &MultiSelect::offset(const Expression &offset)
{
    m_statement.offset(offset);
    return *this;
}

MultiSelect &MultiSelect::onResultFields(const ResultFields &resultFields)
{
    m_fields = resultFields;
    m_statement.select(resultFields);
    return *this;
}

bool MultiSelect::prepareStatement()
{
    if (m_handle->isPrepared()) {
        return true;
    }
    return m_handle->prepare(m_statement);
}

OptionalMultiObject MultiSelect::firstMultiObject()
{
    OptionalMultiObject object;
    WCTRemedialAssert(m_fields.size() != 0, "Result columns can't be empty.", return object;);
    if (prepareStatement()) {
        if (m_handle->step() && !m_handle->done()) {
            object = m_handle->extractOneMultiObject(m_fields);
        }
        m_handle->finalize();
    }
    m_handle->invalidate();
    return object;
}

OptionalMultiObjectArray MultiSelect::allMultiObjects()
{
    OptionalMultiObjectArray objects;
    WCTRemedialAssert(
    m_fields.size() != 0, "Result columns can't be empty.", return objects;);
    if (prepareStatement()) {
        objects = m_handle->extractAllMultiObjects(m_fields);
        m_handle->finalize();
    }
    m_handle->invalidate();
    return objects;
}

} //namespace WCDB
