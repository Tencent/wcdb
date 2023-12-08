//
// Created by sanhuazhang on 2019/05/02
//

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

#include "Syntax.h"
#include "SyntaxAssertion.hpp"
#include "SyntaxEnum.hpp"

namespace WCDB {

namespace Syntax {

JoinClause::~JoinClause() = default;

bool JoinClause::isValid() const
{
    return !tableOrSubqueries.empty();
}

#pragma mark - Identifier
Identifier::Type JoinClause::getType() const
{
    return type;
}

bool JoinClause::describle(std::ostream& stream) const
{
    auto tableOrSubquery = tableOrSubqueries.begin();
    stream << *tableOrSubquery;
    if (++tableOrSubquery != tableOrSubqueries.end()) {
        auto joinOperator = joinOperators.begin();
        auto joinConstraint = joinConstraints.begin();
        WCTSyntaxRemedialAssert(std::distance(tableOrSubquery, tableOrSubqueries.end())
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
    return true;
}

void JoinClause::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, true, stop);
    auto tableOrSubquery = tableOrSubqueries.begin();
    tableOrSubquery->iterate(iterator, stop);
    if (++tableOrSubquery != tableOrSubqueries.end()) {
        auto joinOperator = joinOperators.begin();
        auto joinConstraint = joinConstraints.begin();
        WCTIterateRemedialAssert(
        std::distance(tableOrSubquery, tableOrSubqueries.end())
        == std::distance(joinOperator, joinOperators.end()));
        while (joinOperator != joinOperators.end()
               && tableOrSubquery != tableOrSubqueries.end()
               && joinConstraint != joinConstraints.end()) {
            tableOrSubquery->iterate(iterator, stop);
            if (joinConstraint->get() != nullptr) {
                joinConstraint->get()->iterate(iterator, stop);
            }
            ++joinOperator;
            ++tableOrSubquery;
            ++joinConstraint;
        }
    }
    Identifier::iterate(iterator, false, stop);
}

} // namespace Syntax

} // namespace WCDB
