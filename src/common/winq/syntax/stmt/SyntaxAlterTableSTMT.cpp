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

namespace WCDB {

namespace Syntax {

AlterTableSTMT::~AlterTableSTMT() = default;

#pragma mark - Identifier
Identifier::Type AlterTableSTMT::getType() const
{
    return type;
}

bool AlterTableSTMT::describle(std::ostream& stream) const
{
    stream << "ALTER TABLE ";
    if (!schema.empty()) {
        stream << schema << ".";
    }
    stream << table;
    switch (switcher) {
    case Switch::RenameTable:
        stream << " RENAME TO " << newTable;
        break;
    case Switch::AddColumn:
        stream << " ADD COLUMN " << columnDef;
        break;
    case Switch::RenameColumn:
        stream << " RENAME COLUMN " << column << " TO " << newColumn;
        break;
    }
    return true;
}

void AlterTableSTMT::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, true, stop);
    recursiveIterate(schema, iterator, stop);
    switch (switcher) {
    case Switch::RenameTable:
        break;
    case Switch::AddColumn:
        recursiveIterate(columnDef, iterator, stop);
        break;
    case Switch::RenameColumn:
        recursiveIterate(column, iterator, stop);
        recursiveIterate(newColumn, iterator, stop);
        break;
    }
    Identifier::iterate(iterator, false, stop);
}

bool AlterTableSTMT::isTargetingSameTable(const AlterTableSTMT& other) const
{
    return table == other.table && schema.isTargetingSameSchema(other.schema);
}

} // namespace Syntax

} // namespace WCDB
