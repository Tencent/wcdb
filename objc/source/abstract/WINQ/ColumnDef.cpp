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

#include <WINQ/WINQ.h>

namespace WCDB {

ColumnDef::ColumnDef(const std::string &columnName)
{
    getMutableLang().columnName.assign(columnName);
}

ColumnDef &ColumnDef::withType(const ColumnType &columnType)
{
    lang::ColumnDef &lang = getMutableLang();
    lang.typed = true;
    lang.type = columnType;
    return *this;
}

ColumnDef &
ColumnDef::byAddingConstraint(const ColumnConstraint &columnConstraint)
{
    getMutableLang().columnConstraints.append(columnConstraint.getLang());
    return *this;
}

ColumnDef &ColumnDef::byAddingConstraints(
    const std::list<ColumnConstraint> &columnConstraints)
{
    lang::ColumnDef &lang = getMutableLang();
    for (const ColumnConstraint &columnConstraint : columnConstraints) {
        lang.columnConstraints.append(columnConstraint.getLang());
    }
    return *this;
}

} // namespace WCDB
