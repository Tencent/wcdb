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

TableConstraint::TableConstraint() = default;

TableConstraint::~TableConstraint() = default;

TableConstraint::TableConstraint(const UnsafeStringView& name)
{
    syntax().name = name;
}

TableConstraint& TableConstraint::primaryKey()
{
    syntax().switcher = SyntaxType::Switch::PrimaryKey;
    return *this;
}

TableConstraint& TableConstraint::unique()
{
    syntax().switcher = SyntaxType::Switch::Unique;
    return *this;
}

TableConstraint& TableConstraint::indexed(const IndexedColumn& indexedColumn)
{
    syntax().indexedColumns.push_back(indexedColumn);
    return *this;
}

TableConstraint& TableConstraint::conflict(const Conflict& conflict)
{
    syntax().conflict = conflict;
    return *this;
}

TableConstraint& TableConstraint::check(const Expression& condition)
{
    syntax().switcher = SyntaxType::Switch::Check;
    syntax().expression = condition;
    return *this;
}

TableConstraint&
TableConstraint::foreignKey(const Columns& columns, const ForeignKey& foreignKey)
{
    syntax().switcher = SyntaxType::Switch::ForeignKey;
    syntax().columns = columns;
    syntax().foreignKeyClause = foreignKey;
    return *this;
}

} // namespace WCDB
