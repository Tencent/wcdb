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

template<>
constexpr const char* Enum::description(const Syntax::ForeignKeyClause::Switch& switcher)
{
    switch (switcher) {
    case Syntax::ForeignKeyClause::Switch::OnDeleteSetNull:
        return "ON DELETE SET NULL";
    case Syntax::ForeignKeyClause::Switch::OnDeleteSetDefault:
        return "ON DELETE SET DEFAULT";
    case Syntax::ForeignKeyClause::Switch::OnDeleteCascade:
        return "ON DELETE CASCADE";
    case Syntax::ForeignKeyClause::Switch::OnDeleteRestrict:
        return "ON DELETE RESTRICT";
    case Syntax::ForeignKeyClause::Switch::OnDeleteNoAction:
        return "ON DELETE NO ACTION";
    case Syntax::ForeignKeyClause::Switch::OnUpdateSetNull:
        return "ON UPDATE SET NULL";
    case Syntax::ForeignKeyClause::Switch::OnUpdateSetDefault:
        return "ON UPDATE SET DEFAULT";
    case Syntax::ForeignKeyClause::Switch::OnUpdateCascade:
        return "ON UPDATE CASCADE";
    case Syntax::ForeignKeyClause::Switch::OnUpdateRestrict:
        return "ON UPDATE RESTRICT";
    case Syntax::ForeignKeyClause::Switch::OnUpdateNoAction:
        return "ON UPDATE NO ACTION";
    }
}

template<>
constexpr const char*
Enum::description(const Syntax::ForeignKeyClause::Deferrable& deferrable)
{
    switch (deferrable) {
    case Syntax::ForeignKeyClause::Deferrable::DeferrableInitiallyDeferred:
        return "DEFERRABLE INITIALLY DEFERRED";
    case Syntax::ForeignKeyClause::Deferrable::DeferrableInitiallyImmediate:
        return "DEFERRABLE INITIALLY IMMEDIATE";
    case Syntax::ForeignKeyClause::Deferrable::Deferrable:
        return "DEFERRABLE";
    case Syntax::ForeignKeyClause::Deferrable::NotDeferrableInitiallyDeferred:
        return "NOT DEFERRABLE INITIALLY DEFERRED";
    case Syntax::ForeignKeyClause::Deferrable::NotDeferrableInitiallyImmediate:
        return "NOT DEFERRABLE INITIALLY IMMEDIATE";
    case Syntax::ForeignKeyClause::Deferrable::NotDeferrable:
        return "NOT DEFERRABLE";
    }
}

namespace Syntax {

ForeignKeyClause::~ForeignKeyClause() = default;

bool ForeignKeyClause::isValid() const
{
    return !foreignTable.empty();
}

#pragma mark - Identifier
Identifier::Type ForeignKeyClause::getType() const
{
    return type;
}

bool ForeignKeyClause::describle(std::ostream& stream) const
{
    stream << "REFERENCES " << foreignTable;
    if (!columns.empty()) {
        stream << "(" << columns << ")";
    }
    for (const auto& switcher : switchers) {
        stream << space << switcher;
    }
    if (matchTypeValid()) {
        stream << space << "MATCH" << space << matchType;
    }
    if (deferrableValid()) {
        stream << space << deferrable;
    }
    return true;
}

void ForeignKeyClause::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, true, stop);
    listIterate(columns, iterator, stop);
    Identifier::iterate(iterator, false, stop);
}

} // namespace Syntax

} // namespace WCDB
