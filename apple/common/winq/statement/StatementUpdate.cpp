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

StatementUpdate::StatementUpdate() = default;

StatementUpdate::~StatementUpdate() = default;

StatementUpdate& StatementUpdate::with(const CommonTableExpressions& commonTableExpressions)
{
    syntax().commonTableExpressions = commonTableExpressions;
    return *this;
}

StatementUpdate& StatementUpdate::recursive()
{
    syntax().recursive = true;
    return *this;
}

StatementUpdate& StatementUpdate::update(const QualifiedTable& table)
{
    syntax().table = table;
    return *this;
}

StatementUpdate& StatementUpdate::orRollback()
{
    syntax().conflictAction = Syntax::ConflictAction::Rollback;
    return *this;
}

StatementUpdate& StatementUpdate::orAbort()
{
    syntax().conflictAction = Syntax::ConflictAction::Abort;
    return *this;
}

StatementUpdate& StatementUpdate::orReplace()
{
    syntax().conflictAction = Syntax::ConflictAction::Replace;
    return *this;
}

StatementUpdate& StatementUpdate::orFail()
{
    syntax().conflictAction = Syntax::ConflictAction::Fail;
    return *this;
}

StatementUpdate& StatementUpdate::orIgnore()
{
    syntax().conflictAction = Syntax::ConflictAction::Ignore;
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
    syntax().condition = condition;
    return *this;
}

StatementUpdate& StatementUpdate::order(const OrderingTerm& order)
{
    syntax().orderingTerms.push_back(order);
    return *this;
}

StatementUpdate& StatementUpdate::orders(const OrderingTerms& orders)
{
    syntax().orderingTerms = orders;
    return *this;
}

StatementUpdate& StatementUpdate::limit(const Expression& from, const Expression& to)
{
    syntax().limitParameterType = Syntax::LimitParameterType::End;
    syntax().limit = from;
    syntax().limitParameter = to;
    return *this;
}

StatementUpdate& StatementUpdate::limit(const Expression& limit)
{
    syntax().limitParameterType = Syntax::LimitParameterType::NotSet;
    syntax().limit = limit;
    return *this;
}

StatementUpdate& StatementUpdate::offset(const Expression& offset)
{
    syntax().limitParameterType = Syntax::LimitParameterType::Offset;
    syntax().limitParameter = offset;
    return *this;
}

} // namespace WCDB
