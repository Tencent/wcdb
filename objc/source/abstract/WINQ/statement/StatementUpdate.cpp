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

StatementUpdate& StatementUpdate::with(const With& with)
{
    syntax().useWithClause = true;
    syntax().withClause = with;
    return *this;
}

StatementUpdate& StatementUpdate::update(const QualifiedTable& table)
{
    syntax().switcher = SyntaxType::Switch::Update;
    syntax().table = table;
    return *this;
}

StatementUpdate& StatementUpdate::orRollback()
{
    syntax().switcher = SyntaxType::Switch::UpdateOrRollback;
    return *this;
}

StatementUpdate& StatementUpdate::orAbort()
{
    syntax().switcher = SyntaxType::Switch::UpdateOrAbort;
    return *this;
}

StatementUpdate& StatementUpdate::orReplace()
{
    syntax().switcher = SyntaxType::Switch::UpdateOrReplace;
    return *this;
}

StatementUpdate& StatementUpdate::orFail()
{
    syntax().switcher = SyntaxType::Switch::UpdateOrFail;
    return *this;
}

StatementUpdate& StatementUpdate::orIgnore()
{
    syntax().switcher = SyntaxType::Switch::UpdateOrIgnore;
    return *this;
}

StatementUpdate& StatementUpdate::set(const Columns& columns)
{
    syntax().columnsList.push_back(columns);
    return *this;
}

StatementUpdate& StatementUpdate::to(const Expression& value)
{
    syntax().expressions.push_back(value);
    return *this;
}

StatementUpdate& StatementUpdate::where(const Expression& condition)
{
    syntax().useCondition = true;
    syntax().condition = condition;
    return *this;
}

StatementUpdate& StatementUpdate::order(const OrderingTerms& orders)
{
    syntax().orderingTerms = orders;
    return *this;
}

StatementUpdate& StatementUpdate::limit(const Expression& from, const Expression& to)
{
    syntax().useLimit = true;
    syntax().limitParameterType = SyntaxType::LimitParameterType::End;
    syntax().limit = from;
    syntax().limitParameter = to;
    return *this;
}

StatementUpdate& StatementUpdate::limit(const Expression& limit)
{
    syntax().useLimit = true;
    syntax().limitParameterType = SyntaxType::LimitParameterType::NotSet;
    syntax().limit = limit;
    return *this;
}

StatementUpdate& StatementUpdate::offset(const Expression& offset)
{
    syntax().useLimit = true;
    syntax().limitParameterType = SyntaxType::LimitParameterType::Offset;
    syntax().limitParameter = offset;
    return *this;
}

} // namespace WCDB
