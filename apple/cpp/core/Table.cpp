//
// Created by 陈秋文 on 2022/8/28.
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

#include "Table.hpp"
#include "Handle.hpp"
#include "InnerDatabase.hpp"
#include "StatementDropIndex.hpp"

namespace WCDB {

BaseTable::BaseTable(Recyclable<InnerDatabase*> databaseHolder, const UnsafeStringView& tableName)
: m_tableName(tableName)
, m_databaseHolder(databaseHolder)
, m_innerDatabase(databaseHolder.get())
{
}

BaseTable::BaseTable(const BaseTable&) = default;

BaseTable::~BaseTable() = default;

BaseTable& BaseTable::operator=(const BaseTable&) = default;

const Error& BaseTable::getError() const
{
    return m_innerDatabase->getThreadedError();
}

RecyclableHandle BaseTable::getHandle()
{
    return m_databaseHolder->getHandle();
}

const StringView& BaseTable::getTableName() const
{
    return m_tableName;
}

StringView BaseTable::getIndexNameWithSuffix(const UnsafeStringView& suffix) const
{
    return StringView::formatted("%s%s", m_tableName.data(), suffix.data());
}

bool BaseTable::dropIndexWithSuffix(const UnsafeStringView& suffix)
{
    return m_innerDatabase->execute(
    StatementDropIndex().dropIndex(getIndexNameWithSuffix(suffix)).ifExists());
}

} //namespace WCDB
