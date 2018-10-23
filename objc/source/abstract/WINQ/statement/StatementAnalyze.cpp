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

StatementAnalyze& StatementAnalyze::analyze()
{
    return *this;
}

StatementAnalyze& StatementAnalyze::schema(const Schema& schema)
{
    syntax.useSchema = true;
    syntax.schema = schema;
    return *this;
}

StatementAnalyze& StatementAnalyze::table(const SyntaxString& table)
{
    syntax.useSchema = true;
    syntax.tableOrIndex = table;
    return *this;
}

StatementAnalyze& StatementAnalyze::index(const SyntaxString& index)
{
    syntax.useSchema = true;
    syntax.tableOrIndex = index;
    return *this;
}

} // namespace WCDB
