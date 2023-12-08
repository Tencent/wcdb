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

CreateTableSTMT::~CreateTableSTMT() = default;

#pragma mark - Identifier
Identifier::Type CreateTableSTMT::getType() const
{
    return type;
}

bool CreateTableSTMT::describle(std::ostream& stream) const
{
    stream << "CREATE ";
    if (temp) {
        stream << "TEMP ";
    }
    stream << "TABLE ";
    if (ifNotExists) {
        stream << "IF NOT EXISTS ";
    }
    if (!temp && !schema.empty()) {
        stream << schema << ".";
    }
    stream << table;
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
        stream << " AS " << select.getOrCreate();
        break;
    }
    return true;
}

void CreateTableSTMT::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, true, stop);
    recursiveIterate(schema, iterator, stop);
    switch (switcher) {
    case Switch::ColumnDefs:
        listIterate(columnDefs, iterator, stop);
        listIterate(tableConstraints, iterator, stop);
        break;
    case Switch::Select:
        recursiveIterate(select.getOrCreate(), iterator, stop);
        break;
    }
    Identifier::iterate(iterator, false, stop);
}

} // namespace Syntax

} // namespace WCDB
