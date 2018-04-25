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

TableConstraint::TableConstraint()
{
}

TableConstraint::TableConstraint(const std::string &name)
{
    Lang::TableConstraint &lang = getMutableLang();
    lang.name.assign(name);
}

TableConstraint &
TableConstraint::withPrimaryKey(const IndexedColumn &indexedColumn)
{
    Lang::TableConstraint &lang = getMutableLang();
    lang.type = Lang::TableConstraint::Type::PrimaryKey;
    lang.indexedColumns.append(indexedColumn.getCOWLang());
    return *this;
}

TableConstraint &
TableConstraint::withPrimaryKey(const std::list<IndexedColumn> &indexedColumns)
{
    Lang::TableConstraint &lang = getMutableLang();
    lang.type = Lang::TableConstraint::Type::PrimaryKey;
    for (const IndexedColumn &indexedColumn : indexedColumns) {
        lang.indexedColumns.append(indexedColumn.getCOWLang());
    }
    return *this;
}

TableConstraint &TableConstraint::withUnique(const IndexedColumn &indexedColumn)
{
    Lang::TableConstraint &lang = getMutableLang();
    lang.type = Lang::TableConstraint::Type::Unique;
    lang.indexedColumns.append(indexedColumn.getCOWLang());
    return *this;
}

TableConstraint &
TableConstraint::withUnique(const std::list<IndexedColumn> &indexedColumns)
{
    Lang::TableConstraint &lang = getMutableLang();
    lang.type = Lang::TableConstraint::Type::Unique;
    for (const IndexedColumn &indexedColumn : indexedColumns) {
        lang.indexedColumns.append(indexedColumn.getCOWLang());
    }
    return *this;
}

TableConstraint &
TableConstraint::onConflict(const ConflictClause &conflictClause)
{
    Lang::TableConstraint &lang = getMutableLang();
    lang.conflictClause = conflictClause;
    return *this;
}

TableConstraint &TableConstraint::check(const Expression &expression)
{
    Lang::TableConstraint &lang = getMutableLang();
    lang.type = Lang::TableConstraint::Type::Check;
    lang.expr.assign(expression.getCOWLang());
    return *this;
}

TableConstraint &
TableConstraint::withForeignKey(const Column &column,
                                const ForeignKeyClause &foreignKeyClause)
{
    Lang::TableConstraint &lang = getMutableLang();
    lang.type = Lang::TableConstraint::Type::ForeignKey;
    lang.columns.append(column.getCOWLang());
    lang.foreignKeyClause.assign(foreignKeyClause.getCOWLang());
    return *this;
}

TableConstraint &
TableConstraint::withForeignKey(const std::list<Column> &columns,
                                const ForeignKeyClause &foreignKeyClause)
{
    Lang::TableConstraint &lang = getMutableLang();
    lang.type = Lang::TableConstraint::Type::ForeignKey;
    for (const Column &column : columns) {
        lang.columns.append(column.getCOWLang());
    }
    lang.foreignKeyClause.assign(foreignKeyClause.getCOWLang());
    return *this;
}

} // namespace WCDB
