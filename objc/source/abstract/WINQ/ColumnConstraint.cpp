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

#include <WINQ/WINQ.h>

namespace WCDB {

ColumnConstraint::ColumnConstraint()
{
}

ColumnConstraint &ColumnConstraint::named(const std::string &name)
{
    lang::ColumnConstraint &lang = getMutableLang();
    lang.name.assign(name);
    return *this;
}

ColumnConstraint &
ColumnConstraint::withPrimaryKey(const Order &order,
                                 const ConflictClause &conflictClause,
                                 bool autoIncrement)
{
    lang::ColumnConstraint &lang = getMutableLang();
    lang.type = lang::ColumnConstraint::Type::PrimaryKey;
    lang.order = order;
    lang.conflictClause = conflictClause;
    lang.autoIncrement = autoIncrement;
    return *this;
}

ColumnConstraint &
ColumnConstraint::withNotNull(const ConflictClause &conflictClause)
{
    lang::ColumnConstraint &lang = getMutableLang();
    lang.type = lang::ColumnConstraint::Type::NotNull;
    lang.conflictClause = conflictClause;
    return *this;
}

ColumnConstraint &
ColumnConstraint::withUnique(const ConflictClause &conflictClause)
{
    lang::ColumnConstraint &lang = getMutableLang();
    lang.type = lang::ColumnConstraint::Type::Unique;
    lang.conflictClause = conflictClause;
    return *this;
}

ColumnConstraint &ColumnConstraint::withChecking(const Expression &expression)
{
    lang::ColumnConstraint &lang = getMutableLang();
    lang.type = lang::ColumnConstraint::Type::Check;
    lang.expr.assign(expression.getLang());
    return *this;
}

ColumnConstraint &
ColumnConstraint::withDefault(const LiteralValue &literalValue)
{
    lang::ColumnConstraint &lang = getMutableLang();
    lang.type = lang::ColumnConstraint::Type::Default;
    lang.defaultSwitcher = lang::ColumnConstraint::DefaultSwitch::LiteralValue;
    lang.literalValue.assign(literalValue.getLang());
    return *this;
}

ColumnConstraint &ColumnConstraint::withDefault(const Expression &expression)
{
    lang::ColumnConstraint &lang = getMutableLang();
    lang.type = lang::ColumnConstraint::Type::Default;
    lang.defaultSwitcher = lang::ColumnConstraint::DefaultSwitch::Expr;
    lang.expr.assign(expression.getLang());
    return *this;
}

ColumnConstraint &
ColumnConstraint::withCollate(const std::string &collationName)
{
    lang::ColumnConstraint &lang = getMutableLang();
    lang.type = lang::ColumnConstraint::Type::Collate;
    lang.collationName.assign(collationName);
    return *this;
}

ColumnConstraint &
ColumnConstraint::withForeignKeyClause(const ForeignKeyClause &foreignKeyClause)
{
    lang::ColumnConstraint &lang = getMutableLang();
    lang.type = lang::ColumnConstraint::Type::ForeignKeyClause;
    lang.foreignKeyClause.assign(foreignKeyClause.getLang());
    return *this;
}

} // namespace WCDB
