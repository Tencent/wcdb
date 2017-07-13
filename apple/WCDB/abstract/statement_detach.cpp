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

#include <WCDB/expr.hpp>
#include <WCDB/statement_detach.hpp>

namespace WCDB {

Statement::Type StatementDetach::getStatementType() const
{
    return Statement::Type::Detach;
}

StatementDetach &StatementDetach::detach(const std::string &schema)
{
    m_description.append("DETACH " + schema);
    return *this;
}

StatementDetach &StatementDetach::detach(const std::string &schema,
                                         const std::string &database)
{
    m_description.append("DETACH " + database + " " + schema);
    return *this;
}

} //namespace WCDB
