//
// Created by sanhuazhang on 2019/08/30
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

StatementExplain::StatementExplain() = default;

StatementExplain::~StatementExplain() = default;

StatementExplain& StatementExplain::explain(const Statement& statement)
{
    explain(statement, false);
    return *this;
}

StatementExplain& StatementExplain::explainQueryPlan(const Statement& statement)
{
    explain(statement, true);
    return *this;
}

void StatementExplain::explain(const Statement& statement, bool queryPlan)
{
    syntax().queryPlan = queryPlan;
    syntax().stmt = statement.syntax();
}

} // namespace WCDB
