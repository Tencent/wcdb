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

TableConstraint::~TableConstraint() = default;

#pragma mark - Identifier
Identifier::Type TableConstraint::getType() const
{
    return type;
}

bool TableConstraint::describle(std::ostream& stream) const
{
    if (!name.empty()) {
        stream << "CONSTRAINT " << name << space;
    }
    switch (switcher) {
    case Switch::PrimaryKey:
        stream << "PRIMARY KEY(" << indexedColumns << ")";
        if (conflictValid()) {
            stream << space << conflict;
        }
        break;
    case Switch::Unique:
        stream << "UNIQUE(" << indexedColumns << ")";
        if (conflictValid()) {
            stream << space << conflict;
        }
        break;
    case Switch::Check:
        stream << "CHECK(" << expression.getOrCreate() << ")";
        break;
    case Switch::ForeignKey:
        stream << "FOREIGN KEY(" << columns << ") " << foreignKeyClause.getOrCreate();
        break;
    }
    return true;
}

void TableConstraint::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, true, stop);
    switch (switcher) {
    case Switch::PrimaryKey:
    case Switch::Unique:
        listIterate(indexedColumns, iterator, stop);
        break;
    case Switch::Check:
        recursiveIterate(expression.getOrCreate(), iterator, stop);
        break;
    case Switch::ForeignKey:
        listIterate(columns, iterator, stop);
        recursiveIterate(foreignKeyClause.getOrCreate(), iterator, stop);
        break;
    }
    Identifier::iterate(iterator, false, stop);
}

} // namespace Syntax

} // namespace WCDB
