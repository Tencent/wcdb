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

JoinConstraint& JoinConstraint::on(const Expression& expression)
{
    LangJoinConstraint& lang = getMutableLang();
    lang.type = LangJoinConstraint::Type::On; 
    lang.expr.assign(expression.getLang());
    return *this;
}

JoinConstraint& JoinConstraint::usingColumn(const std::string columnName)
{
    LangJoinConstraint& lang = getMutableLang();
    lang.type = LangJoinConstraint::Type::Using; 
    lang.columnNames.append(columnName);
    return *this;
}

JoinConstraint& JoinConstraint::usingColumns(const std::list<std::string> columnNames)
{
    LangJoinConstraint& lang = getMutableLang();
    for (const std::string & columnName : columnNames) {
        lang.columnNames.append(columnName);
    }
    return *this;
}
