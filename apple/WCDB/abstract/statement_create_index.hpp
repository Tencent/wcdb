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

#ifndef statement_create_index_hpp
#define statement_create_index_hpp

#include <WCDB/statement.hpp>

namespace WCDB {

class StatementCreateIndex : public Statement {
public:
    StatementCreateIndex &create(const std::string &index,
                                 bool unique = false,
                                 bool ifNotExists = true);

    template <typename T = ColumnIndex>
    typename std::enable_if<std::is_base_of<ColumnIndex, T>::value,
                            StatementCreateIndex &>::type
    on(const std::string &table, const std::list<const T> &indexList)
    {
        m_description.append(" ON " + table + "(");
        joinDescribableList(indexList);
        m_description.append(")");
        return *this;
    }
    StatementCreateIndex &where(const Expr &expr);

    virtual Statement::Type getStatementType() const override;
};

} //namespace WCDB

#endif /* statement_create_index_hpp */
