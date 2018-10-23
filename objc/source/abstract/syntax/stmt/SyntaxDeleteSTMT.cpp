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
Identifier::Type DeleteSTMT::getType() const
{
    return type;
}

std::string DeleteSTMT::getDescription() const
{
    std::ostringstream stream;
    if (useWithClause) {
        stream << withClause << space;
    }
    stream << "DELETE FROM " << table;
    if (useCondition) {
        stream << " WHERE " << condition;
    }
    if (!orderingTerms.empty()) {
        stream << " ORDER BY " << orderingTerms;
    }
    if (useLimit) {
        stream << " LIMIT " << limit;
        switch (limitParameterType) {
        case LimitParameterType::NotSet:
            break;
        case LimitParameterType::Offset:
            stream << " OFFSET " << limitParameter;
            break;
        case LimitParameterType::End:
            stream << ", " << limitParameter;
            break;
        }
    }
    return stream.str();
}

void DeleteSTMT::iterate(const Iterator& iterator, void* parameter)
{
    Identifier::iterate(iterator, parameter);
    if (useWithClause) {
        withClause.iterate(iterator, parameter);
    }
    table.iterate(iterator, parameter);
    if (useCondition) {
        condition.iterate(iterator, parameter);
    }
    listIterate(orderingTerms, iterator, parameter);
    if (useLimit) {
        limit.iterate(iterator, parameter);
        switch (limitParameterType) {
        case LimitParameterType::NotSet:
            break;
        case LimitParameterType::Offset:
        case LimitParameterType::End:
            limitParameter.iterate(iterator, parameter);
            break;
        }
    }
}

} // namespace Syntax

} // namespace WCDB
