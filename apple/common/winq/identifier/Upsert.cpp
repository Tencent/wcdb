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

Upsert::Upsert() = default;

Upsert::~Upsert() = default;

Upsert& Upsert::onConflict()
{
    return *this;
}

Upsert& Upsert::indexed(const IndexedColumn& indexedColumn)
{
    syntax().indexedColumns.push_back(indexedColumn);
    return *this;
}

Upsert& Upsert::where(const Expression& condition)
{
    if (syntax().columnsList.empty()) {
        syntax().condition = condition;
    } else {
        syntax().updateCondition = condition;
    }
    return *this;
}

Upsert& Upsert::doNothing()
{
    syntax().switcher = Syntax::UpsertClause::Switch::Nothing;
    syntax().columnsList.clear();
    syntax().expressions.clear();
    return *this;
}

Upsert& Upsert::doUpdate()
{
    syntax().switcher = Syntax::UpsertClause::Switch::Update;
    syntax().columnsList.clear();
    syntax().expressions.clear();
    return *this;
}

Upsert& Upsert::set(const Columns& columns)
{
    syntax().columnsList.push_back(columns);
    return *this;
}

Upsert& Upsert::to(const Expression& value)
{
    syntax().expressions.push_back(value);
    return *this;
}

} // namespace WCDB
