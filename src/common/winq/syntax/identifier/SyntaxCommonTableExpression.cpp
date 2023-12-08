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

namespace WCDB {

namespace Syntax {

CommonTableExpression::~CommonTableExpression() = default;

bool CommonTableExpression::isValid() const
{
    return !table.empty();
}

#pragma mark - Identifier
Identifier::Type CommonTableExpression::getType() const
{
    return type;
}

bool CommonTableExpression::describle(std::ostream& stream) const
{
    stream << table;
    if (!columns.empty()) {
        stream << "(" << columns << ")";
    }
    if (select != nullptr) {
        stream << " AS(" << *select.get() << ")";
    }
    return true;
}

void CommonTableExpression::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, true, stop);
    listIterate(columns, iterator, stop);
    if (select != nullptr) {
        recursiveIterate(*select.get(), iterator, stop);
    }
    Identifier::iterate(iterator, false, stop);
}

} // namespace Syntax

} // namespace WCDB
