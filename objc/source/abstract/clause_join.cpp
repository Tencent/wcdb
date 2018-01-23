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
#include <WCDB/expression.hpp>
#include <WCDB/subquery.hpp>
#include <WCDB/column.hpp>
#include <WCDB/utility.hpp>

namespace WCDB {
    
JoinClause &JoinClause::join(const Subquery &subquery,
                             JoinClause::Type type,
                             bool isNatural)
{
    if (isNatural) {
        m_description.append(" NATURAL");
    }
    if (type != JoinClause::Type::NotSet) {
        switch (type) {
            case JoinClause::Type::Left:
                m_description.append(" LEFT");
                break;
            case JoinClause::Type::LeftOuter:
                m_description.append(" LEFT OUTER");
                break;
            case JoinClause::Type::Inner:
                m_description.append(" INNER");
                break;
            case JoinClause::Type::Cross:
                m_description.append(" CROSS");
                break;
            default:
                break;
        }
    }
    m_description.append(" JOIN " + subquery.getDescription());
    return *this;
}
    
JoinClause &JoinClause::join(const Subquery &subquery, JoinClause::Type type)
{
    return join(subquery, type, false);
}

JoinClause &JoinClause::naturalJoin(const Subquery &subquery, JoinClause::Type type)
{
    return join(subquery, type, true);
}

JoinClause &JoinClause::on(const Expression &expression)
{
    m_description.append(" ON " + expression.getDescription());
    return *this;
}
    
JoinClause &JoinClause::usingColumns(const Column &column)
{
    m_description.append(" USING " + column.getDescription());
    return *this;
}
        
JoinClause &JoinClause::usingColumns(const std::list<const Column> &columnList)
{
    m_description.append(" USING " + stringByJoiningList(columnList));
    return *this;
}
    
}; // namespace WCDB
