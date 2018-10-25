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

namespace Syntax {

#pragma mark - Identifier
Identifier::Type CreateTableSTMT::getType() const
{
    return type;
}

String CreateTableSTMT::getDescription() const
{
    std::ostringstream stream;
    stream << "CREATE ";
    if (temp) {
        stream << "TEMP ";
    }
    stream << "TABLE ";
    if (ifNotExists) {
        stream << "IF NOT EXISTS ";
    }
    stream << schema << "." << table;
    switch (switcher) {
    case Switch::ColumnDefs:
        stream << "(" << columnDefs;
        if (!tableConstraints.empty()) {
            stream << ", " << tableConstraints;
        }
        stream << ")";
        if (withoutRowid) {
            stream << " WITHOUT ROWID";
        }
        break;
    case Switch::Select:
        stream << " AS " << select;
        break;
    }
    return stream.str();
}

void CreateTableSTMT::iterate(const Iterator& iterator, void* parameter)
{
    Identifier::iterate(iterator, parameter);
    schema.iterate(iterator, parameter);
    switch (switcher) {
    case Switch::ColumnDefs:
        listIterate(columnDefs, iterator, parameter);
        listIterate(tableConstraints, iterator, parameter);
        break;
    case Switch::Select:
        select.iterate(iterator, parameter);
        break;
    }
}

} // namespace Syntax

} // namespace WCDB
