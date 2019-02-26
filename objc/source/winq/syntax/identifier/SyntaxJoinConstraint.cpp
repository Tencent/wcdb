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

#include <WCDB/SyntaxAssertion.hpp>
#include <WCDB/Syntax.h>

namespace WCDB {

namespace Syntax {

#pragma mark - Identifier
Identifier::Type JoinConstraint::getType() const
{
    return type;
}

String JoinConstraint::getDescription() const
{
    std::ostringstream stream;
    if (!columns.empty()) {
        stream << "USING(" << columns << ")";
    } else {
        stream << "ON " << expression;
    }
    return stream.str();
}

void JoinConstraint::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, stop);
    if (!columns.empty()) {
        listIterate(columns, iterator, stop);
    } else {
        recursiveIterate(expression, iterator, stop);
    }
}

} // namespace Syntax

} // namespace WCDB
