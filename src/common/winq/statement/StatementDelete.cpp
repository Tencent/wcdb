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

StatementDelete::StatementDelete() = default;

StatementDelete::~StatementDelete() = default;

StatementDelete& StatementDelete::with(const CommonTableExpressions& commonTableExpressions)
{
    syntax().commonTableExpressions = commonTableExpressions;
    return *this;
}

StatementDelete& StatementDelete::recursive()
{
    syntax().recursive = true;
    return *this;
}

StatementDelete& StatementDelete::deleteFrom(const QualifiedTable& table)
{
    syntax().table = table;
    return *this;
}

StatementDelete& StatementDelete::where(const Expression& condition)
{
    syntax().condition = condition;
    return *this;
}

StatementDelete& StatementDelete::orders(const OrderingTerms& orderingTerms)
{
    syntax().orderingTerms = orderingTerms;
    return *this;
}

StatementDelete& StatementDelete::limit(const Expression& from, const Expression& to)
{
    syntax().limitParameterType = Syntax::LimitParameterType::End;
    syntax().limit = from;
    syntax().limitParameter = to;
    return *this;
}

StatementDelete& StatementDelete::limit(const Expression& limit)
{
    syntax().limitParameterType = Syntax::LimitParameterType::NotSet;
    syntax().limit = limit;
    return *this;
}

StatementDelete& StatementDelete::offset(const Expression& offset)
{
    syntax().limitParameterType = Syntax::LimitParameterType::Offset;
    syntax().limitParameter = offset;
    return *this;
}

} // namespace WCDB
