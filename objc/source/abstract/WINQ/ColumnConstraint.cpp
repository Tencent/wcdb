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

#include <WCDB/WINQ.h>

namespace WCDB {

ColumnConstraint::ColumnConstraint()
{
}

ColumnConstraint &ColumnConstraint::named(const std::string &name)
{
    Lang::ColumnConstraint &lang = getMutableLang();
    lang.name.assign(name);
    return *this;
}

ColumnConstraint &ColumnConstraint::withPrimaryKey(const Order &order,
                                                   bool autoIncrement)
{
    Lang::ColumnConstraint &lang = getMutableLang();
    lang.type = Lang::ColumnConstraint::Type::PrimaryKey;
    lang.order = order;
    lang.autoIncrement = autoIncrement;
    return *this;
}

ColumnConstraint &ColumnConstraint::withNotNull()
{
    Lang::ColumnConstraint &lang = getMutableLang();
    lang.type = Lang::ColumnConstraint::Type::NotNull;
    return *this;
}

ColumnConstraint &ColumnConstraint::withUnique()
{
    Lang::ColumnConstraint &lang = getMutableLang();
    lang.type = Lang::ColumnConstraint::Type::Unique;
    return *this;
}

ColumnConstraint &
ColumnConstraint::onConflict(const ConflictClause &conflictClause)
{
    Lang::ColumnConstraint &lang = getMutableLang();
    lang.conflictClause = conflictClause;
    return *this;
}

ColumnConstraint &ColumnConstraint::withChecking(const Expression &expression)
{
    Lang::ColumnConstraint &lang = getMutableLang();
    lang.type = Lang::ColumnConstraint::Type::Check;
    lang.expr.assign(expression.getCOWLang());
    return *this;
}

ColumnConstraint &
ColumnConstraint::withDefaultValue(const LiteralValue &literalValue)
{
    Lang::ColumnConstraint &lang = getMutableLang();
    lang.type = Lang::ColumnConstraint::Type::Default;
    lang.defaultSwitcher = Lang::ColumnConstraint::DefaultSwitch::LiteralValue;
    lang.literalValue.assign(literalValue.getCOWLang());
    return *this;
}

ColumnConstraint &
ColumnConstraint::withDefaultExpression(const Expression &expression)
{
    Lang::ColumnConstraint &lang = getMutableLang();
    lang.type = Lang::ColumnConstraint::Type::Default;
    lang.defaultSwitcher = Lang::ColumnConstraint::DefaultSwitch::Expr;
    lang.expr.assign(expression.getCOWLang());
    return *this;
}

ColumnConstraint &
ColumnConstraint::withCollate(const std::string &collationName)
{
    Lang::ColumnConstraint &lang = getMutableLang();
    lang.type = Lang::ColumnConstraint::Type::Collate;
    lang.collationName.assign(collationName);
    return *this;
}

ColumnConstraint &
ColumnConstraint::withForeignKeyClause(const ForeignKeyClause &foreignKeyClause)
{
    Lang::ColumnConstraint &lang = getMutableLang();
    lang.type = Lang::ColumnConstraint::Type::ForeignKeyClause;
    lang.foreignKeyClause.assign(foreignKeyClause.getCOWLang());
    return *this;
}

} // namespace WCDB
