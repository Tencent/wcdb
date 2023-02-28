//
// Created by qiuwenchen on 2022/9/2.
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

#include <WCDB/Delete.hpp>
#include <WCDB/Handle.hpp>

namespace WCDB {

Delete::~Delete() = default;

Delete::Delete(Recyclable<InnerDatabase *> databaseHolder)
: ChainCall(databaseHolder)
{
}

Delete &Delete::fromTable(const UnsafeStringView &tableName)
{
    m_statement.deleteFrom(tableName);
    return *this;
}

Delete &Delete::where(const Expression &condition)
{
    m_statement.where(condition);
    return *this;
}

Delete &Delete::orders(const OrderingTerms &orders)
{
    m_statement.orders(orders);
    return *this;
}

Delete &Delete::limit(const Expression &limit)
{
    m_statement.limit(limit);
    return *this;
}

Delete &Delete::offset(const Expression &offset)
{
    m_statement.offset(offset);
    return *this;
}

bool Delete::execute()
{
    bool result = m_handle->execute(m_statement);
    m_handle->invalidate();
    return result;
}

} //namespace WCDB
