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

ColumnDef::~ColumnDef() = default;

bool ColumnDef::isValid() const
{
    return WCDB_SYNTAX_CHECK_OPTIONAL_VALID(column);
}

#pragma mark - Identifier
Identifier::Type ColumnDef::getType() const
{
    return type;
}

bool ColumnDef::describle(std::ostream& stream) const
{
    stream << column.getOrCreate();
    if (columnTypeValid()) {
        stream << space << columnType;
    };
    for (const auto& constraint : constraints) {
        stream << space << constraint;
    }
    return true;
}

void ColumnDef::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, true, stop);
    recursiveIterate(column.getOrCreate(), iterator, stop);
    listIterate(constraints, iterator, stop);
    Identifier::iterate(iterator, false, stop);
}

#pragma mark - Utility
bool ColumnDef::isAutoIncrement() const
{
    bool isAutoIncrement = false;
    for (const auto& constraint : constraints) {
        if (constraint.isAutoIncrement()) {
            isAutoIncrement = true;
            break;
        }
    }
    return isAutoIncrement;
}

bool ColumnDef::isPrimaryKey() const
{
    bool isPrimary = false;
    for (const auto& constraint : constraints) {
        if (constraint.isPrimaryKey()) {
            isPrimary = true;
            break;
        }
    }
    return isPrimary;
}

bool ColumnDef::isUnique() const
{
    bool isUnique = false;
    for (const auto& constraint : constraints) {
        if (constraint.isUnique()) {
            isUnique = true;
            break;
        }
    }
    return isUnique;
}

} // namespace Syntax

} // namespace WCDB
