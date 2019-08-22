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

#ifndef constraint_table_hpp
#define constraint_table_hpp

#include <WCDB/declare.hpp>
#include <WCDB/describable.hpp>

namespace WCDB {

class TableConstraint : public Describable {
public:
    TableConstraint();
    TableConstraint(const char *name);
    TableConstraint(const std::string &name);

    template <typename T = ColumnIndex>
    typename std::enable_if<std::is_base_of<ColumnIndex, T>::value,
                            TableConstraint &>::type
    makePrimary(const std::list<const T> &columnIndexList)
    {
        m_description.append(" PRIMARY KEY(");
        joinDescribableList(columnIndexList);
        m_description.append(")");
        return *this;
    }

    template <typename T = ColumnIndex>
    typename std::enable_if<std::is_base_of<ColumnIndex, T>::value,
                            TableConstraint &>::type
    makeUnique(const std::list<const T> &columnIndexList)
    {
        m_description.append(" UNIQUE (");
        joinDescribableList(columnIndexList);
        m_description.append(")");
        return *this;
    }

    TableConstraint &onConflict(Conflict conflict);

    TableConstraint &makeCheck(const Expr &expr);
};

} //namespace WCDB

#endif /* constraint_table_hpp */
