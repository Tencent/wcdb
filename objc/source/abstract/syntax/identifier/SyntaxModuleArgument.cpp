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
Identifier::Type ModuleArgument::getType() const
{
    return type;
}

String ModuleArgument::getDescription() const
{
    std::ostringstream stream;
    switch (switcher) {
    case Switcher::TableConstraint:
        stream << *tableConstraint.get();
        break;
    case Switcher::ColumnDef:
        stream << columnDef;
        break;
    case Switcher::LeftRight:
        stream << left << " = " << right;
        break;
    }
    return stream.str();
}

void ModuleArgument::iterate(const Iterator& iterator, void* parameter)
{
    Identifier::iterate(iterator, parameter);
    switch (switcher) {
    case Switcher::TableConstraint:
        tableConstraint->iterate(iterator, parameter);
        break;
    case Switcher::ColumnDef:
        columnDef.iterate(iterator, parameter);
        break;
    case Switcher::LeftRight:
        left.iterate(iterator, parameter);
        right.iterate(iterator, parameter);
        break;
    }
}

} // namespace Syntax

} // namespace WCDB
