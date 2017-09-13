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

#include <WCDB/pragma.hpp>
#include <WCDB/statement_pragma.hpp>

namespace WCDB {

StatementPragma &StatementPragma::pragma(const Pragma &pragma)
{
    m_description.append("PRAGMA " + pragma.getDescription());
    return *this;
}

StatementPragma &StatementPragma::pragma(const Pragma &pragma,
                                         const char *value)
{
    m_description.append("PRAGMA " + pragma.getDescription() + "=" +
                         LiteralValue(value).getDescription());
    return *this;
}

StatementPragma &StatementPragma::pragma(const Pragma &pragma,
                                         const std::string &value)
{
    m_description.append("PRAGMA " + pragma.getDescription() + "=" +
                         LiteralValue(value).getDescription());
    return *this;
}

Statement::Type StatementPragma::getStatementType() const
{
    return Statement::Type::Pragma;
}

} //namespace WCDB
