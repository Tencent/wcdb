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

#pragma mark - Identifier
Identifier::Type TableOrSubquery::getType() const
{
    return type;
}

bool TableOrSubquery::describle(std::ostringstream& stream) const
{
    switch (switcher) {
    case Switch::Table:
        if (!schema.empty()) {
            stream << schema << ".";
        }
        stream << tableOrFunction;
        if (!alias.empty()) {
            stream << " AS " << alias;
        }
        switch (indexType) {
        case IndexType::NotSet:
            break;
        case IndexType::Indexed:
            stream << " INDEXED BY " << index;
            break;
        case IndexType::NotIndexed:
            stream << " NOT INDEXED";
            break;
        }
        break;
    case Switch::Function:
        if (!schema.empty()) {
            stream << schema << ".";
        }
        stream << tableOrFunction << "(" << expressions << ")";
        if (!alias.empty()) {
            stream << " AS " << alias;
        }
        break;
    case Switch::TableOrSubqueries:
        stream << "(" << tableOrSubqueries << ")";
        break;
    case Switch::JoinClause:
        stream << "(" << joinClause << ")";
        break;
    case Switch::Select:
        stream << "(" << select << ")";
        if (!alias.empty()) {
            stream << " AS " << alias;
        }
        break;
    }
    return true;
}

void TableOrSubquery::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, stop);
    switch (switcher) {
    case Switch::Table:
        recursiveIterate(schema, iterator, stop);
        break;
    case Switch::Function:
        recursiveIterate(schema, iterator, stop);
        listIterate(expressions, iterator, stop);
        break;
    case Switch::TableOrSubqueries:
        listIterate(tableOrSubqueries, iterator, stop);
        break;
    case Switch::JoinClause:
        recursiveIterate(joinClause, iterator, stop);
        break;
    case Switch::Select:
        recursiveIterate(select, iterator, stop);
        break;
    }
}

} // namespace Syntax

} // namespace WCDB
