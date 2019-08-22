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

#include <WCDB/clause_join.hpp>
#include <WCDB/statement_select.hpp>
#include <WCDB/subquery.hpp>

namespace WCDB {

Subquery::Subquery(const char *table) : Describable(table)
{
}

Subquery::Subquery(const std::string &table) : Describable(table)
{
}

Subquery::Subquery(const JoinClause &joinClause)
    : Describable("(" + joinClause.getDescription() + ")")
{
}

Subquery::Subquery(const StatementSelect &statementSelect)
    : Describable("(" + statementSelect.getDescription() + ")")
{
}

Subquery &Subquery::as(const std::string &alias)
{
    m_description.append(" AS " + alias);
    return *this;
}

Subquery::operator SubqueryList() const
{
    return {*this};
}

} //namespace WCDB
