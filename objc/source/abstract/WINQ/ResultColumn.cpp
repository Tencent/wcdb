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

const ResultColumn ResultColumn::All(LangResultColumn::Type::Star);

ResultColumn ResultColumn::AllInTable(const std::string& tableName)
{
    return ResultColumn(tableName); 
}

ResultColumn::ResultColumn(const Expression& expression)
{
    LangResultColumn& lang = getMutableLang();
    lang.type = LangResultColumn::Type::Expr;
    lang.expr.assign(expression.getLang());
}

ResultColumn::ResultColumn(const LangResultColumn::Type& type)
{
    LangResultColumn& lang = getMutableLang();
    lang.type = LangResultColumn::Type::Star;
}

ResultColumn::ResultColumn(const std::string& tableName)
{
    LangResultColumn& lang = getMutableLang();
    lang.type = LangResultColumn::Type::Star;
    lang.tableName.assign(tableName);
}
