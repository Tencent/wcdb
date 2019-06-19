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
#include <WCDB/SyntaxEnum.hpp>

namespace WCDB {

namespace Syntax {

#pragma mark - Identifier
Identifier::Type ColumnConstraint::getType() const
{
    return type;
}

bool ColumnConstraint::describle(std::ostringstream& stream) const
{
    if (!name.empty()) {
        stream << "CONSTRAINT " << name << space;
    }
    switch (switcher) {
    case Switch::PrimaryKey:
        stream << "PRIMARY KEY";
        if (orderValid()) {
            stream << space << order;
        }
        if (conflictValid()) {
            stream << space << conflict;
        }
        if (autoIncrement) {
            stream << " AUTOINCREMENT";
        }
        break;
    case Switch::NotNull:
        stream << "NOT NULL";
        if (conflictValid()) {
            stream << space << conflict;
        }
        break;
    case Switch::Unique:
        stream << "UNIQUE";
        if (conflictValid()) {
            stream << space << conflict;
        }
        break;
    case Switch::Check:
        stream << "CHECK(" << expression << ")";
        break;
    case Switch::Default:
        stream << "DEFAULT " << expression;
        break;
    case Switch::Collate:
        stream << "COLLATE " << collation;
        break;
    case Switch::ForeignKey:
        stream << foreignKeyClause;
        break;
    }
    return true;
}

void ColumnConstraint::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, stop);
    switch (switcher) {
    case Switch::PrimaryKey:
    case Switch::NotNull:
    case Switch::Unique:
    case Switch::Collate:
        break;
    case Switch::Check:
        recursiveIterate(expression, iterator, stop);
        break;
    case Switch::Default:
        recursiveIterate(expression, iterator, stop);
        break;
    case Switch::ForeignKey:
        recursiveIterate(foreignKeyClause, iterator, stop);
        break;
    }
}

#pragma mark - Utility
bool ColumnConstraint::isAutoIncrement() const
{
    return switcher == Switch::PrimaryKey && autoIncrement;
}

} // namespace Syntax

} // namespace WCDB
