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

#include "WINQ.h"

namespace WCDB {

ColumnConstraint::ColumnConstraint() = default;

ColumnConstraint::ColumnConstraint(const UnsafeStringView& name)
{
    syntax().name = name;
}

ColumnConstraint::~ColumnConstraint() = default;

ColumnConstraint& ColumnConstraint::primaryKey()
{
    syntax().switcher = SyntaxType::Switch::PrimaryKey;
    return *this;
}

ColumnConstraint& ColumnConstraint::order(const Order& order)
{
    syntax().order = order;
    return *this;
}

ColumnConstraint& ColumnConstraint::conflict(const Conflict& conflict)
{
    syntax().conflict = conflict;
    return *this;
}

ColumnConstraint& ColumnConstraint::autoIncrement()
{
    syntax().autoIncrement = true;
    return *this;
}

ColumnConstraint& ColumnConstraint::notNull()
{
    syntax().switcher = SyntaxType::Switch::NotNull;
    return *this;
}

ColumnConstraint& ColumnConstraint::unique()
{
    syntax().switcher = SyntaxType::Switch::Unique;
    return *this;
}

ColumnConstraint& ColumnConstraint::check(const Expression& expression)
{
    syntax().switcher = SyntaxType::Switch::Check;
    syntax().expression = expression;
    return *this;
}

ColumnConstraint& ColumnConstraint::default_(const Expression& defaultValue)
{
    syntax().switcher = SyntaxType::Switch::Default;
    syntax().expression = defaultValue;
    return *this;
}

ColumnConstraint& ColumnConstraint::collate(const UnsafeStringView& collation)
{
    syntax().switcher = SyntaxType::Switch::Collate;
    syntax().collation = collation;
    return *this;
}

ColumnConstraint& ColumnConstraint::foreignKey(const ForeignKey& foreignKey)
{
    syntax().switcher = SyntaxType::Switch::ForeignKey;
    syntax().foreignKeyClause = foreignKey;
    return *this;
}

ColumnConstraint& ColumnConstraint::unIndexed()
{
    syntax().switcher = SyntaxType::Switch::UnIndexed;
    return *this;
}

} // namespace WCDB
