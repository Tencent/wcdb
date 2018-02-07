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

ColumnConstraint::ColumnConstraint()
{
}

ColumnConstraint::ColumnConstraint(const std::string &name)
{
    LangColumnConstraint &lang = getMutableLang();
    lang.name.assign(name);
}

ColumnConstraint &ColumnConstraint::named(const std::string &name)
{
    LangColumnConstraint &lang = getMutableLang();
    lang.name.assign(name);
    return *this;
}

ColumnConstraint &
ColumnConstraint::withPrimaryKey(const Order &order,
                                 const ConflictClause &conflictClause,
                                 bool autoIncrement)
{
    LangColumnConstraint &lang = getMutableLang();
    lang.type = LangColumnConstraint::Type::PrimaryKey;
    lang.order = order;
    lang.conflictClause = conflictClause;
    lang.autoIncrement = autoIncrement;
    return *this;
}

ColumnConstraint &
ColumnConstraint::withNotNull(const ConflictClause &conflictClause)
{
    LangColumnConstraint &lang = getMutableLang();
    lang.type = LangColumnConstraint::Type::NotNull;
    lang.conflictClause = conflictClause;
    return *this;
}

ColumnConstraint &
ColumnConstraint::withUnique(const ConflictClause &conflictClause)
{
    LangColumnConstraint &lang = getMutableLang();
    lang.type = LangColumnConstraint::Type::Unique;
    lang.conflictClause = conflictClause;
    return *this;
}

ColumnConstraint &ColumnConstraint::withChecking(const Expression &expression)
{
    LangColumnConstraint &lang = getMutableLang();
    lang.type = LangColumnConstraint::Type::Check;
    lang.expr.assign(expression.getLang());
    return *this;
}

ColumnConstraint &
ColumnConstraint::withDefault(const LiteralValue &literalValue)
{
    LangColumnConstraint &lang = getMutableLang();
    lang.type = LangColumnConstraint::Type::Default;
    lang.defaultSwitcher = LangColumnConstraint::DefaultSwitch::LiteralValue;
    lang.literalValue.assign(literalValue.getLang());
    return *this;
}

ColumnConstraint &ColumnConstraint::withDefault(const Expression &expression)
{
    LangColumnConstraint &lang = getMutableLang();
    lang.type = LangColumnConstraint::Type::Default;
    lang.defaultSwitcher = LangColumnConstraint::DefaultSwitch::Expr;
    lang.expr.assign(expression.getLang());
    return *this;
}

ColumnConstraint &
ColumnConstraint::withCollate(const std::string &collationName)
{
    LangColumnConstraint &lang = getMutableLang();
    lang.type = LangColumnConstraint::Type::Collate;
    lang.collationName.assign(collationName);
    return *this;
}

ColumnConstraint &
ColumnConstraint::withForeignKeyClause(const ForeignKeyClause &foreignKeyClause)
{
    LangColumnConstraint &lang = getMutableLang();
    lang.type = LangColumnConstraint::Type::ForeignKeyClause;
    lang.foreignKeyClause.assign(foreignKeyClause.getLang());
    return *this;
}
