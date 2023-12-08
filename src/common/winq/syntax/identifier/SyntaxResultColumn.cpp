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

ResultColumn::~ResultColumn() = default;

bool ResultColumn::isValid() const
{
    return WCDB_SYNTAX_CHECK_OPTIONAL_VALID(expression);
}

#pragma mark - Identifier
Identifier::Type ResultColumn::getType() const
{
    return type;
}

bool ResultColumn::describle(std::ostream& stream) const
{
    stream << expression.getOrCreate();
    if (!alias.empty()) {
        stream << " AS " << alias;
    }
    return true;
}

void ResultColumn::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, true, stop);
    recursiveIterate(expression.getOrCreate(), iterator, stop);
    Identifier::iterate(iterator, false, stop);
}

} // namespace Syntax

} // namespace WCDB
