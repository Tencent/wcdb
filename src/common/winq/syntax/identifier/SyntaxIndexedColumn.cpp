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

IndexedColumn::~IndexedColumn() = default;

#pragma mark - Identifier
Identifier::Type IndexedColumn::getType() const
{
    return type;
}

bool IndexedColumn::describle(std::ostream& stream) const
{
    switch (switcher) {
    case Switch::Column:
        stream << column.getOrCreate();
        break;
    case Switch::Expression:
        stream << expression.getOrCreate();
        break;
    }
    if (collation.length() > 0) {
        stream << " COLLATE " << collation;
    }
    if (orderValid()) {
        stream << space << order;
    }
    return true;
}

void IndexedColumn::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, true, stop);
    switch (switcher) {
    case Switch::Column:
        recursiveIterate(column.getOrCreate(), iterator, stop);
        break;
    case Switch::Expression:
        recursiveIterate(expression.getOrCreate(), iterator, stop);
    }
    Identifier::iterate(iterator, false, stop);
}

} // namespace Syntax

} // namespace WCDB
