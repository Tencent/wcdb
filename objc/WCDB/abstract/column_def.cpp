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

#include <WCDB/column.hpp>
#include <WCDB/column_def.hpp>
#include <WCDB/expr.hpp>

namespace WCDB {

ColumnDef::ColumnDef(const Column &column, ColumnType type)
    : Describable(column.getName() + " " + ColumnTypeName(type))
{
}

ColumnDef &
ColumnDef::makePrimary(OrderTerm term, bool autoIncrement, Conflict conflict)
{
    m_description = m_description + " PRIMARY KEY";
    if (term != OrderTerm::NotSet) {
        m_description.append(" ");
        m_description.append(OrderTermName(term));
    }
    if (autoIncrement) {
        m_description.append(" AUTOINCREMENT");
    }
    if (conflict != Conflict::NotSet) {
        switch (conflict) {
            case Conflict::Rollback:
                m_description.append(" ON CONFLICT ROLLBACK");
                break;
            case Conflict::Abort:
                m_description.append(" ON CONFLICT ABORT");
                break;
            case Conflict::Fail:
                m_description.append(" ON CONFLICT FAIL");
                break;
            case Conflict::Ignore:
                m_description.append(" ON CONFLICT IGNORE");
                break;
            case Conflict::Replace:
                m_description.append(" ON CONFLICT REPLACE");
                break;
            default:
                break;
        }
    }
    return *this;
}

ColumnDef &ColumnDef::makeDefault(const char *value)
{
    m_description.append(" DEFAULT '" + std::string(value) + "'");
    return *this;
}

ColumnDef &ColumnDef::makeDefault(const std::string &value)
{
    m_description.append(" DEFAULT '" + value + "'");
    return *this;
}

ColumnDef &ColumnDef::makeDefault(const std::nullptr_t &value)
{
    m_description.append(" DEFAULT NULL");
    return *this;
}

ColumnDef &ColumnDef::makeDefault(
    const typename ColumnTypeInfo<ColumnType::BLOB>::CType &value, int size)
{
    m_description.append(" DEFAULT '" +
                         std::string((const char *) value, size) + "'");
    return *this;
}

ColumnDef &ColumnDef::makeDefault(const Expr &expr)
{
    m_description.append(" DEFAULT " + expr.getDescription());
    return *this;
}

ColumnDef &ColumnDef::makeDefault(DefaultType defaultType)
{
    m_description.append(" DEFAULT ");
    switch (defaultType) {
        case DefaultType::CurrentDate:
            m_description.append("CURRENT_DATE");
            break;
        case DefaultType::CurrentTime:
            m_description.append("CURRENT_TIME");
            break;
        case DefaultType::CurrentTimestamp:
            m_description.append("CURRENT_TIMESTAMP");
            break;
    }
    return *this;
}

ColumnDef &ColumnDef::makeNotNull()
{
    m_description.append(" NOT NULL");
    return *this;
}

ColumnDef &ColumnDef::makeUnique()
{
    m_description.append(" UNIQUE");
    return *this;
}

ColumnDef &ColumnDef::makeForeignKey(const ForeignKey &foreignKey)
{
    m_description.append(" " + foreignKey.getDescription());
    return *this;
}

} //namespace WCDB
