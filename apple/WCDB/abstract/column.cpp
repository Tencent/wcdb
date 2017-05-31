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

#include <WCDB/column.hpp>

namespace WCDB {

const Column Column::Rowid("rowid");
const Column Column::Any("*");

Column::Column() : Describable("")
{
}

Column::Column(const char *name) : Describable(name)
{
}

Column::Column(const std::string &name) : Describable(name)
{
}

const std::string &Column::getName() const
{
    return m_description;
}

Column Column::inTable(const std::string &table) const
{
    return Column(table + "." + getName());
}

bool Column::operator==(const Column &column) const
{
    return getDescription() == column.getDescription();
}

Column::operator ColumnList() const
{
    return {*this};
}

} //namespace WCDB
