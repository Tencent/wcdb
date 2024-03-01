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

Column::~Column() = default;

bool Column::isValid() const
{
    return wildcard || !name.empty();
}

#pragma mark - Identifier
Identifier::Type Column::getType() const
{
    return type;
}

const BaseBinding* Column::getTableBinding() const
{
    if (tableBinding != nullptr) {
        return tableBinding;
    } else if (tableBindingRetrive != nullptr) {
        return tableBindingRetrive();
    }
    return nullptr;
}

bool Column::describle(std::ostream& stream) const
{
    if (!table.empty()) {
        if (!schema.empty()) {
            stream << schema << ".";
        }
        stream << table << ".";
    }
    if (wildcard) {
        stream << "*";
    } else {
        stream << name;
    }
    return true;
}

void Column::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, true, stop);
    if (!table.empty()) {
        recursiveIterate(schema, iterator, stop);
    }
    Identifier::iterate(iterator, false, stop);
}

} // namespace Syntax

} // namespace WCDB
