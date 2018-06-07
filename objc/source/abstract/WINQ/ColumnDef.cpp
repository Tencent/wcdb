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

#include <WCDB/Assertion.hpp>
#include <WCDB/String.hpp>
#include <WCDB/WINQ.h>

namespace WCDB {

ColumnDef::ColumnDef(const Column &column)
{
    Lang::ColumnDef &lang = getMutableLang();
    lang.column.assign(column.getCOWLang());
}

ColumnDef &ColumnDef::withType(const ColumnType &columnType)
{
    Lang::ColumnDef &lang = getMutableLang();
    lang.typed = true;
    lang.type = columnType;
    return *this;
}

ColumnDef &
ColumnDef::byAddingConstraint(const ColumnConstraint &columnConstraint)
{
    getMutableLang().columnConstraints.append(columnConstraint.getCOWLang());
    return *this;
}

ColumnDef &ColumnDef::byAddingConstraints(
    const std::list<ColumnConstraint> &columnConstraints)
{
    Lang::ColumnDef &lang = getMutableLang();
    for (const ColumnConstraint &columnConstraint : columnConstraints) {
        lang.columnConstraints.append(columnConstraint.getCOWLang());
    }
    return *this;
}

bool ColumnDef::isAutoIncrement() const
{
    const auto &lang = getCOWLang();
    if (lang.empty()) {
        return false;
    }
    const auto &columnConstraints =
        lang.get<Lang::ColumnDef>().columnConstraints;
    if (columnConstraints.empty()) {
        return false;
    }
    for (const auto &columnConstraint : columnConstraints.get()) {
        if (columnConstraint.get().autoIncrement) {
            return true;
        }
    }
    return false;
}

bool ColumnDef::isPrimary() const
{
    const auto &lang = getCOWLang();
    if (lang.empty()) {
        return false;
    }
    const auto &columnConstraints =
        lang.get<Lang::ColumnDef>().columnConstraints;
    if (columnConstraints.empty()) {
        return false;
    }
    for (const auto &columnConstraint : columnConstraints.get()) {
        if (columnConstraint.get().type ==
            Lang::ColumnConstraint::Type::PrimaryKey) {
            return true;
        }
    }
    return false;
}

const std::string &ColumnDef::getColumnName() const
{
    const auto &lang = getCOWLang();
    if (lang.empty()) {
        return String::empty();
    }
    const auto &column = lang.get<Lang::ColumnDef>().column;
    if (column.empty()) {
        return String::empty();
    }
    const auto &name = column.get().name;
    if (name.empty()) {
        return String::empty();
    }
    return name.get();
}

} // namespace WCDB
