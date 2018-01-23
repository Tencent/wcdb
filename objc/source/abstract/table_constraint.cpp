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
#include <WCDB/foreign_key.hpp>
#include <WCDB/table_constraint.hpp>
#include <WCDB/expression.hpp>
#include <WCDB/column_index.hpp>
#include <WCDB/utility.hpp>

namespace WCDB {

TableConstraint::TableConstraint(const std::string &name)
    : Describable("CONSTRAINT " + name)
{
}
        
TableConstraint &TableConstraint::makePrimary(const std::list<const ColumnIndex> &list)
{
    m_description.append(" PRIMARY KEY(" + stringByJoiningList(list) + ")");
    return *this;
}
    
TableConstraint &TableConstraint::makePrimary(const ColumnIndex &columnIndex)
{
    m_description.append(" PRIMARY KEY(" + columnIndex.getDescription() + ")");
    return *this;
}
    
TableConstraint &TableConstraint::makeUnique(const std::list<const ColumnIndex> &list)
{
    m_description.append(" UNIQUE(" + stringByJoiningList(list) + ")");
    return *this;
}
    
TableConstraint &TableConstraint::makeUnique(const ColumnIndex &columnIndex)
{
    m_description.append(" UNIQUE(" + columnIndex.getDescription() + ")");
    return *this;
}
    
TableConstraint &TableConstraint::onConflict(Conflict conflict)
{
    if (conflict != WCDB::Conflict::NotSet) {
        m_description.append(" ON CONFLICT ");
        m_description.append(ConflictName(conflict));
    }
    return *this;
}

TableConstraint &TableConstraint::check(const Expression &expression)
{
    m_description.append(" CHECK" + expression.getDescription());
    return *this;
}

TableConstraint &TableConstraint::makeForeignKey(const std::list<const Column> &list, const ForeignKey &foreignKey) {
    m_description.append(" FOREIGN KEY(" + stringByJoiningList(list) + ") " + foreignKey.getDescription());
    return *this;
}
    
TableConstraint &TableConstraint::makeForeignKey(const Column &column, const ForeignKey &foreignKey)
{
    m_description.append(" FOREIGN KEY(" + column.getDescription() + ") " + foreignKey.getDescription());
    return *this;
}
    
TableConstraint::operator std::list<const TableConstraint>() const
{
    return {*this};
}
    
} //namespace WCDB
