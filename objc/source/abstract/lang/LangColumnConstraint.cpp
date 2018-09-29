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
#include <WCDB/Lang.h>

namespace WCDB {

namespace Lang {

ColumnConstraint::ColumnConstraint()
: type(Type::NotSet)
, order(Order::NotSet)
, conflictClause(ConflictClause::NotSet)
, autoIncrement(false)
, defaultSwitcher(DefaultSwitch::NotSet)
{
}

CopyOnWriteString ColumnConstraint::SQL() const
{
    std::string description;
    if (!name.empty()) {
        description.append("CONSTRAINT " + name.get() + " ");
    }
    switch (type) {
    case Type::PrimaryKey:
        description.append("PRIMARY KEY");
        if (order != Order::NotSet) {
            description.append(" ");
            description.append(LangOrderName(order));
        }
        if (conflictClause != ConflictClause::NotSet) {
            description.append(" ");
            description.append(LangConflictClauseName(conflictClause));
        }
        if (autoIncrement) {
            description.append(" AUTOINCREMENT");
        }
        break;
    case Type::NotNull:
        description.append("NOT NULL");
        if (conflictClause != ConflictClause::NotSet) {
            description.append(" ");
            description.append(LangConflictClauseName(conflictClause));
        }
        break;
    case Type::Unique:
        description.append("UNIQUE");
        if (conflictClause != ConflictClause::NotSet) {
            description.append(" ");
            description.append(LangConflictClauseName(conflictClause));
        }
        break;
    case Type::Check:
        LangRemedialAssert(!expr.empty());
        description.append("CHECK(" + expr.description().get() + ")");
        break;
    case Type::Default:
        description.append("DEFAULT");
        switch (defaultSwitcher) {
        case DefaultSwitch::LiteralValue:
            LangRemedialAssert(!literalValue.empty());
            description.append(" " + literalValue.description().get());
            break;
        case DefaultSwitch::Expr:
            LangRemedialAssert(!expr.empty());
            description.append("(" + expr.description().get() + ")");
            break;
        default:
            LangRemedialAssert(false);
            break;
        }
        break;
    case Type::Collate:
        LangRemedialAssert(!collationName.empty());
        description.append("COLLATE " + collationName.get());
        break;
    case Type::ForeignKeyClause:
        LangRemedialAssert(!foreignKeyClause.empty());
        description.append(foreignKeyClause.description().get());
        break;
    default:
        LangRemedialAssert(false);
        break;
    }
    return description;
}

template<>
CopyOnWriteString CopyOnWriteLazyLangList<ColumnConstraint>::calculatedDescription() const
{
    std::string description;
    bool space = false;
    for (const auto &element : this->get()) {
        if (space) {
            description.append(" ");
        } else {
            space = true;
        }
        LangRemedialAssert(!element.empty());
        description.append(element.description().get());
    }
    return description;
}

} // namespace Lang

} // namespace WCDB
