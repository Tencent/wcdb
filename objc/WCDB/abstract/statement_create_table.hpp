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

#ifndef statement_create_table_hpp
#define statement_create_table_hpp

#include <WCDB/declare.hpp>
#include <WCDB/statement.hpp>

namespace WCDB {

class StatementCreateTable : public Statement {
public:
    //temp table is not supported
    StatementCreateTable &create(const std::string &table,
                                 bool ifNotExists = true);
    StatementCreateTable &as(const StatementSelect &statementSelect);

    template <typename T = ColumnDef>
    typename std::enable_if<std::is_base_of<ColumnDef, T>::value,
                            StatementCreateTable &>::type
    create(const std::string &table,
           const std::list<const T> &columnDefList,
           bool ifNotExists = true)
    {
        m_description.append("CREATE TABLE ");
        if (ifNotExists) {
            m_description.append("IF NOT EXISTS ");
        }
        m_description.append(table + "(");
        joinDescribableList(columnDefList);
        m_description.append(")");
        return *this;
    }

    template <typename T, typename U>
    typename std::enable_if<std::is_base_of<ColumnDef, T>::value &&
                                std::is_base_of<TableConstraint, U>::value,
                            StatementCreateTable &>::type
    create(const std::string &table,
           const std::list<const T> &columnDefList,
           const std::list<const U> &tableConstraintList,
           bool ifNotExists = true)
    {
        m_description.append("CREATE TABLE ");
        if (ifNotExists) {
            m_description.append("IF NOT EXISTS ");
        }
        m_description.append(table + "(");
        joinDescribableList(columnDefList);
        if (!tableConstraintList.empty()) {
            m_description.append(" ,");
            joinDescribableList(tableConstraintList);
        }
        m_description.append(")");
        return *this;
    }

    virtual Statement::Type getStatementType() const override;
};

} //namespace WCDB

#endif /* statement_create_table_hpp */
