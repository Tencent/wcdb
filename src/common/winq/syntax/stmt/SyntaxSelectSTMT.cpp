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

SelectSTMT::~SelectSTMT() = default;

bool SelectSTMT::isValid() const
{
    return WCDB_SYNTAX_CHECK_OPTIONAL_VALID(select);
}

#pragma mark - Identifier
Identifier::Type SelectSTMT::getType() const
{
    return type;
}

bool SelectSTMT::describle(std::ostream& stream) const
{
    if (!commonTableExpressions.empty()) {
        stream << "WITH ";
        if (recursive) {
            stream << "RECURSIVE ";
        }
        stream << commonTableExpressions << space;
    }
    stream << select.getOrCreate();
    if (!cores.empty()) {
        WCTSyntaxRemedialAssert(cores.size() == compoundOperators.size());
        auto core = cores.begin();
        auto compoundOperator = compoundOperators.begin();
        while (core != cores.end() && compoundOperator != compoundOperators.end()) {
            stream << space << *compoundOperator << space << *core;
            ++compoundOperator;
            ++core;
        }
    }
    if (!orderingTerms.empty()) {
        stream << " ORDER BY " << orderingTerms;
    }
    if (WCDB_SYNTAX_CHECK_OPTIONAL_VALID(limit)) {
        stream << " LIMIT " << limit.value();
        switch (limitParameterType) {
        case LimitParameterType::NotSet:
            break;
        case LimitParameterType::Offset:
            stream << " OFFSET " << limitParameter.getOrCreate();
            break;
        case LimitParameterType::End:
            stream << ", " << limitParameter.getOrCreate();
            break;
        }
    }
    return true;
}

void SelectSTMT::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, true, stop);
    listIterate(commonTableExpressions, iterator, stop);
    recursiveIterate(select.getOrCreate(), iterator, stop);
    listIterate(cores, iterator, stop);
    listIterate(orderingTerms, iterator, stop);
    if (WCDB_SYNTAX_CHECK_OPTIONAL_VALID(limit)) {
        recursiveIterate(limit.value(), iterator, stop);
        switch (limitParameterType) {
        case LimitParameterType::NotSet:
            break;
        case LimitParameterType::Offset:
        case LimitParameterType::End:
            recursiveIterate(limitParameter.getOrCreate(), iterator, stop);
            break;
        }
    }
    Identifier::iterate(iterator, false, stop);
}

} // namespace Syntax

} // namespace WCDB
