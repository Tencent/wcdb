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
Identifier::Type InsertSTMT::getType() const
{
    return type;
}

bool InsertSTMT::describle(std::ostringstream& stream, bool skipSchema) const
{
    if (withClause.isValid()) {
        stream << withClause << space;
    }
    stream << "INSERT ";
    if (conflictActionValid()) {
        stream << conflictAction << space;
    }
    stream << "INTO ";
    if (!skipSchema && !schema.empty()) {
        stream << schema << ".";
    }
    stream << table;
    if (!alias.empty()) {
        stream << " AS " << alias;
    }
    if (!columns.empty()) {
        stream << "(" << columns << ")";
    }
    stream << space;
    switch (switcher) {
    case Switch::Values: {
        stream << "VALUES";
        bool comma = false;
        for (const auto& expressionsValue : expressionsValues) {
            if (comma) {
                stream << ", ";
            } else {
                comma = true;
            }
            stream << "(" << expressionsValue << ")";
        }
        break;
    }
    case Switch::Select:
        stream << select;
        break;
    case Switch::Default:
        stream << "DEFAULT VALUES";
        break;
    }
    if (upsertClause.isValid()) {
        stream << space << upsertClause;
    }
    return true;
}

bool InsertSTMT::describle(std::ostringstream& stream) const
{
    return describle(stream, false);
}

void InsertSTMT::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, stop);
    if (withClause.isValid()) {
        recursiveIterate(withClause, iterator, stop);
    }
    recursiveIterate(schema, iterator, stop);
    listIterate(columns, iterator, stop);
    switch (switcher) {
    case Switch::Values: {
        for (auto& expressionsValue : expressionsValues) {
            listIterate(expressionsValue, iterator, stop);
        }
        break;
    }
    case Switch::Select:
        recursiveIterate(select, iterator, stop);
        break;
    case Switch::Default:
        break;
    }
    if (upsertClause.isValid()) {
        recursiveIterate(upsertClause, iterator, stop);
    }
}

#pragma mark - Utility
    bool InsertSTMT::isMultiWrite() const
    {
        return (switcher == Switch::Values && expressionsValues.size() > 1)
        || switcher == Switch::Select;
    }
    
bool InsertSTMT::isTargetingSameTable(const InsertSTMT& other) const
{
    return table == other.table && schema.isTargetingSameSchema(other.schema);
}

} // namespace Syntax

} // namespace WCDB
