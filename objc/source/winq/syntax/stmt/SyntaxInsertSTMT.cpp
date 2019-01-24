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

#include <WCDB/Assertion.hpp>
#include <WCDB/Enum.hpp>
#include <WCDB/Syntax.h>

namespace WCDB {

namespace Syntax {

#pragma mark - Identifier
Identifier::Type InsertSTMT::getType() const
{
    return type;
}

String InsertSTMT::getDescription(bool skipSchema) const
{
    std::ostringstream stream;
    if (useWithClause) {
        stream << withClause << space;
    }
    stream << "INSERT ";
    if (useConflictAction) {
        stream << conflictAction << space;
    }
    stream << "INTO ";
    if (!skipSchema) {
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
    switch (valueSwitcher) {
    case SwitchValue::Values: {
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
    case SwitchValue::Select:
        stream << select;
        break;
    case SwitchValue::Default:
        stream << "DEFAULT VALUES";
        break;
    }
    if (useUpsertClause) {
        stream << space << upsertClause;
    }
    return stream.str();
}

String InsertSTMT::getDescription() const
{
    return getDescription(false);
}

void InsertSTMT::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, stop);
    if (useWithClause) {
        recursiveIterate(withClause, iterator, stop);
    }
    recursiveIterate(schema, iterator, stop);
    listIterate(columns, iterator, stop);
    switch (valueSwitcher) {
    case SwitchValue::Values: {
        for (auto& expressionsValue : expressionsValues) {
            listIterate(expressionsValue, iterator, stop);
        }
        break;
    }
    case SwitchValue::Select:
        recursiveIterate(select, iterator, stop);
        break;
    case SwitchValue::Default:
        break;
    }
    if (useUpsertClause) {
        recursiveIterate(upsertClause, iterator, stop);
    }
}

} // namespace Syntax

} // namespace WCDB
