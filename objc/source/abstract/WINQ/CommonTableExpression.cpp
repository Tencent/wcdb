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

CommonTableExpression::CommonTableExpression(const std::string &tableName)
{
    lang::CommonTableExpression &lang = getMutableLang();
    lang.tableName.assign(tableName);
}

CommonTableExpression &
CommonTableExpression::byAddingColumn(const std::string &columnName)
{
    lang::CommonTableExpression &lang = getMutableLang();
    lang.columnNames.append(columnName);
    return *this;
}

CommonTableExpression &CommonTableExpression::byAddingColumns(
    const std::list<std::string> &columnNames)
{
    lang::CommonTableExpression &lang = getMutableLang();
    for (const auto &columnName : columnNames) {
        lang.columnNames.append(columnName);
    }
    return *this;
}

CommonTableExpression &
CommonTableExpression::as(const StatementSelect &statementSelect)
{
    lang::CommonTableExpression &lang = getMutableLang();
    lang.selectSTMT.assign(statementSelect.getLang());
    return *this;
}

} // namespace WCDB
