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

#ifndef table_constraint_hpp
#define table_constraint_hpp

#include <WCDB/declare.hpp>
#include <WCDB/convertible.hpp>

namespace WCDB {

class TableConstraint : public Describable {
public:
    TableConstraint(const std::string &name);

    template <typename T>
    typename std::enable_if<ColumnIndexConvertible<T>::value, TableConstraint &>::type
    makePrimary(const std::list<const T> &list)
    {
        m_description.append(" PRIMARY KEY(" + stringByJoiningList(list) + ")");
        return *this;
    }
    
    TableConstraint &makePrimary(const std::list<const ColumnIndex> &list);
    
    TableConstraint &makePrimary(const ColumnIndex &columnIndex);

    template <typename T>
    typename std::enable_if<ColumnIndexConvertible<T>::value, TableConstraint &>::type
    makeUnique(const std::list<const T> &list)
    {
        m_description.append(" UNIQUE(" + stringByJoiningList(list) + ")");
        return *this;
    }

    TableConstraint &makeUnique(const std::list<const ColumnIndex> &list);
    
    TableConstraint &makeUnique(const ColumnIndex &columnIndex);
    
    TableConstraint &onConflict(Conflict conflict);

    TableConstraint &check(const Expression &expression);
    
    template <typename T>
    typename std::enable_if<ColumnConvertible<T>::value, TableConstraint &>::type makeForeignKey(const std::list<const T> &list, const ForeignKey &foreignKey) {
        m_description.append(" FOREIGN KEY(" + stringByJoiningList(list) + ") " + foreignKey.getDescription());
        return *this;
    }
    
    TableConstraint &makeForeignKey(const std::list<const Column> &list, const ForeignKey &foreignKey);

    TableConstraint &makeForeignKey(const Column &column, const ForeignKey &foreignKey);
    
    operator std::list<const TableConstraint>() const;
};

} //namespace WCDB

#endif /* table_constraint_hpp */
