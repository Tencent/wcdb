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
#include <WCDB/Syntax.h>

namespace WCDB {

template<>
constexpr const char* Enum::description(const Syntax::InsertSTMT::Switch& switcher)
{
    switch (switcher) {
    case Syntax::InsertSTMT::Switch::Insert:
        return "INSERT";
    case Syntax::InsertSTMT::Switch::InsertOrReplace:
        return "INSERT OR REPLACE";
    case Syntax::InsertSTMT::Switch::InsertOrRollback:
        return "INSERT OR ROLLBACK";
    case Syntax::InsertSTMT::Switch::InsertOrAbort:
        return "INSERT OR ABORT";
    case Syntax::InsertSTMT::Switch::InsertOrFail:
        return "INSERT OR FAIL";
    case Syntax::InsertSTMT::Switch::InsertOrIgnore:
        return "INSERT OR IGNORE";
    }
}

namespace Syntax {

#pragma mark - Identifier
Identifier::Type InsertSTMT::getType() const
{
    return type;
}

std::string InsertSTMT::getDescription() const
{
    std::ostringstream stream;
    if (useWithClause) {
        stream << withClause << space;
    }
    stream << switcher << " INTO " << schema << "." << table;
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

void InsertSTMT::iterate(const Iterator& iterator, void* parameter)
{
    Identifier::iterate(iterator, parameter);
    if (useWithClause) {
        withClause.iterate(iterator, parameter);
    }
    schema.iterate(iterator, parameter);
    listIterate(columns, iterator, parameter);
    switch (valueSwitcher) {
    case SwitchValue::Values: {
        for (auto& expressionsValue : expressionsValues) {
            listIterate(expressionsValue, iterator, parameter);
        }
        break;
    }
    case SwitchValue::Select:
        select.iterate(iterator, parameter);
        break;
    case SwitchValue::Default:
        break;
    }
    if (useUpsertClause) {
        upsertClause.iterate(iterator, parameter);
    }
}

} // namespace Syntax

} // namespace WCDB
