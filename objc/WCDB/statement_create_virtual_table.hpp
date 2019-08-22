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

#ifndef statement_create_virtual_table_hpp
#define statement_create_virtual_table_hpp

#include <WCDB/declare.hpp>
#include <WCDB/statement.hpp>

namespace WCDB {

class StatementCreateVirtualTable : public Statement {
public:
    StatementCreateVirtualTable &create(const std::string &table,
                                        bool ifNotExists = true);

    StatementCreateVirtualTable &usingModule(const std::string &moduleName);

    template <typename T = ModuleArgument>
    typename std::enable_if<std::is_base_of<ModuleArgument, T>::value,
                            StatementCreateVirtualTable &>::type
    usingModule(const std::string &moduleName,
                const std::list<T> &moduleArgumentList)
    {
        m_description.append(" USING " + moduleName);
        if (!moduleArgumentList.empty()) {
            m_description.append("(");
            joinDescribableList(moduleArgumentList);
            m_description.append(")");
        }
        return *this;
    }

    virtual Statement::Type getStatementType() const override;
};

} //namespace WCDB

#endif /* statement_create_virtual_table_hpp */
