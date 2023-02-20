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

#include <WCDB/Syntax.h>
#include <WCDB/SyntaxAssertion.hpp>

namespace WCDB {

namespace Syntax {

WindowDef::~WindowDef() = default;

bool WindowDef::isValid() const
{
    return !expressions.empty() || !orderingTerms.empty() || frameSpec.isValid();
}

#pragma mark - Identifier
Identifier::Type WindowDef::getType() const
{
    return type;
}

bool WindowDef::describle(std::ostream& stream) const
{
    stream << "(";
    bool extraSpace = false;
    if (!expressions.empty()) {
        extraSpace = true;
        stream << "PARTITION BY " << expressions;
    }
    if (!orderingTerms.empty()) {
        if (extraSpace) {
            stream << space;
        }
        stream << "ORDER BY " << orderingTerms;
    }
    if (frameSpec.isValid()) {
        if (extraSpace) {
            stream << space;
        }
        stream << frameSpec;
    }
    stream << ")";
    return true;
}

void WindowDef::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, stop);
    listIterate(expressions, iterator, stop);
    listIterate(orderingTerms, iterator, stop);
    if (frameSpec.isValid()) {
        recursiveIterate(frameSpec, iterator, stop);
    }
}

} // namespace Syntax

} // namespace WCDB
