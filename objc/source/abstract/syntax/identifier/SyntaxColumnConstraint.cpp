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
Identifier::Type ColumnConstraint::getType() const
{
    return type;
}

std::string ColumnConstraint::getDescription() const
{
    std::ostringstream stream;
    if (!name.empty()) {
        stream << "CONSTRAINT " << name << space;
    }
    switch (switcher) {
    case Switch::PrimaryKey:
        stream << "PRIMARY KEY";
        if (useOrder) {
            stream << space << order;
        }
        if (useConflict) {
            stream << space << conflict;
        }
        if (autoIncrement) {
            stream << " AUTOINCREMENT";
        }
        break;
    case Switch::NotNull:
        stream << "NOT NULL";
        if (useConflict) {
            stream << space << conflict;
        }
        break;
    case Switch::Unique:
        stream << "UNIQUE";
        if (useConflict) {
            stream << space << conflict;
        }
        break;
    case Switch::Check:
        stream << "CHECK(" << expression << ")";
        break;
    case Switch::Default:
        stream << "DEFAULT ";
        switch (defaultType) {
        case DefaultType::LiteralValue:
            stream << literalValue;
            break;
        case DefaultType::Expression:
            stream << expression;
            break;
        }
        break;
    case Switch::Collate:
        stream << "COLLATE " << collation;
        break;
    case Switch::ForeignKey:
        stream << foreignKeyClause;
        break;
    }
    return stream.str();
}

void ColumnConstraint::iterate(const Iterator& iterator, void* parameter)
{
    Identifier::iterate(iterator, parameter);
    switch (switcher) {
    case Switch::PrimaryKey:
    case Switch::NotNull:
    case Switch::Unique:
    case Switch::Collate:
        break;
    case Switch::Check:
        expression.iterate(iterator, parameter);
        break;
    case Switch::Default:
        switch (defaultType) {
        case DefaultType::LiteralValue:
            literalValue.iterate(iterator, parameter);
            break;
        case DefaultType::Expression:
            expression.iterate(iterator, parameter);
            break;
        }
        break;
    case Switch::ForeignKey:
        foreignKeyClause.iterate(iterator, parameter);
        break;
    }
}

} // namespace Syntax

} // namespace WCDB
