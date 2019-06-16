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

bool ColumnDef::isValid() const
{
    return column.isValid();
}

#pragma mark - Identifier
Identifier::Type ColumnDef::getType() const
{
    return type;
}

StringView ColumnDef::getValidDescription() const
{
    std::ostringstream stream;
    stream << column;
    if (columnTypeValid()) {
        stream << space << columnType;
    };
    for (const auto& constraint : constraints) {
        stream << space << constraint;
    }
    return StringView(stream.str());
}

void ColumnDef::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, stop);
    recursiveIterate(column, iterator, stop);
    listIterate(constraints, iterator, stop);
}

#pragma mark - Utility
bool ColumnDef::isAutoIncrement() const
{
    bool isAutoIncrement = false;
    for (const auto& constraint : constraints) {
        if (constraint.isAutoIncrement()) {
            isAutoIncrement = true;
        }
    }
    return isAutoIncrement;
}

} // namespace Syntax

} // namespace WCDB
