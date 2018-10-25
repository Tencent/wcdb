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

#include <WCDB/Assertion.hpp>
#include <WCDB/Syntax.h>

namespace WCDB {

namespace Syntax {

#pragma mark - Identifier
Identifier::Type JoinClause::getType() const
{
    return type;
}

String JoinClause::getDescription() const
{
    std::ostringstream stream;
    auto tableOrSubquery = tableOrSubqueries.begin();
    stream << *tableOrSubquery;
    if (++tableOrSubquery != tableOrSubqueries.end()) {
        auto joinOperator = joinOperators.begin();
        auto joinConstraint = joinConstraints.begin();
        SyntaxRemedialAssert(std::distance(tableOrSubquery, tableOrSubqueries.end())
                             == std::distance(joinOperator, joinOperators.end()));
        while (joinOperator != joinOperators.end()
               && tableOrSubquery != tableOrSubqueries.end()
               && joinConstraint != joinConstraints.end()) {
            if (*joinOperator != JoinOperator::With) {
                stream << space;
            }
            stream << *joinOperator << space << *tableOrSubquery;
            if (joinConstraint->get() != nullptr) {
                stream << space << *joinConstraint->get();
            }
            ++joinOperator;
            ++tableOrSubquery;
            ++joinConstraint;
        }
    }
    return stream.str();
}

void JoinClause::iterate(const Iterator& iterator, void* parameter)
{
    Identifier::iterate(iterator, parameter);
    auto tableOrSubquery = tableOrSubqueries.begin();
    tableOrSubquery->iterate(iterator, parameter);
    if (++tableOrSubquery != tableOrSubqueries.end()) {
        auto joinOperator = joinOperators.begin();
        auto joinConstraint = joinConstraints.begin();
        IterateRemedialAssert(std::distance(tableOrSubquery, tableOrSubqueries.end())
                              == std::distance(joinOperator, joinOperators.end()));
        while (joinOperator != joinOperators.end()
               && tableOrSubquery != tableOrSubqueries.end()
               && joinConstraint != joinConstraints.end()) {
            tableOrSubquery->iterate(iterator, parameter);
            if (joinConstraint->get() != nullptr) {
                joinConstraint->get()->iterate(iterator, parameter);
            }
            ++joinOperator;
            ++tableOrSubquery;
            ++joinConstraint;
        }
    }
}

} // namespace Syntax

} // namespace WCDB
