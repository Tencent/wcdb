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
Identifier::Type TableConstraint::getType() const
{
    return type;
}

String TableConstraint::getDescription() const
{
    std::ostringstream stream;
    if (!name.empty()) {
        stream << "CONSTRAINT " << name << space;
    }
    switch (switcher) {
    case Switch::PrimaryKey:
        stream << "PRIMARY KEY(" << indexedColumns << ")";
        if (useConflict) {
            stream << space << conflict;
        }
        break;
    case Switch::Unique:
        stream << "UNIQUE(" << indexedColumns << ")";
        if (useConflict) {
            stream << space << conflict;
        }
        break;
    case Switch::Check:
        stream << "CHECK(" << expression << ")";
        break;
    case Switch::ForeignKey:
        stream << "FOREIGN KEY(" << columns << ") " << foreignKeyClause;
        break;
    }
    return stream.str();
}

void TableConstraint::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, stop);
    switch (switcher) {
    case Switch::PrimaryKey:
    case Switch::Unique:
        listIterate(indexedColumns, iterator, stop);
        break;
    case Switch::Check:
        recursiveIterate(expression, iterator, stop);
        break;
    case Switch::ForeignKey:
        listIterate(columns, iterator, stop);
        recursiveIterate(foreignKeyClause, iterator, stop);
        break;
    }
}

} // namespace Syntax

} // namespace WCDB
