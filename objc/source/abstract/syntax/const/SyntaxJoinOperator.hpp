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

#ifndef SyntaxJoinOperator_hpp
#define SyntaxJoinOperator_hpp

#include <WCDB/Enum.hpp>

namespace WCDB {

namespace Syntax {

enum class JoinOperator : int {
    With,
    Join,
    LeftOuterJoin,
    LeftJoin,
    InnerJoin,
    CrossJoin,
    NaturalJoin,
    NaturalLeftOuterJoin,
    NaturalLeftJoin,
    NaturalInnerJoin,
    NaturalCrossJoin,
};

}

template<>
constexpr const char* Enum::description(const Syntax::JoinOperator& joinOperator)
{
    switch (joinOperator) {
    case Syntax::JoinOperator::With:
        return ",";
    case Syntax::JoinOperator::Join:
        return "JOIN";
    case Syntax::JoinOperator::LeftOuterJoin:
        return "LEFT OUTER JOIN";
    case Syntax::JoinOperator::LeftJoin:
        return "LEFT JOIN";
    case Syntax::JoinOperator::InnerJoin:
        return "INNER JOIN";
    case Syntax::JoinOperator::CrossJoin:
        return "CROSS JOIN";
    case Syntax::JoinOperator::NaturalJoin:
        return "NATURAL JOIN";
    case Syntax::JoinOperator::NaturalLeftOuterJoin:
        return "NATURAL LEFT OUTER JOIN";
    case Syntax::JoinOperator::NaturalLeftJoin:
        return "NATURAL LEFT JOIN";
    case Syntax::JoinOperator::NaturalInnerJoin:
        return "NATURAL INNER JOIN";
    case Syntax::JoinOperator::NaturalCrossJoin:
        return "NATURAL CROSS JOIN";
    }
}

} // namespace WCDB

#endif /* SyntaxJoinOperator_hpp */
