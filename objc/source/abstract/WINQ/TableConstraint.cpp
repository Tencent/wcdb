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
    lang::TableConstraint &lang = getMutableLang();
    lang.name.assign(name);
}

TableConstraint &
TableConstraint::withPrimaryKey(const IndexedColumn &indexedColumn,
                                const ConflictClause &conflictClause)
{
    lang::TableConstraint &lang = getMutableLang();
    lang.type = lang::TableConstraint::Type::PrimaryKey;
    lang.indexedColumns.append(indexedColumn.getLang());
    lang.conflictClause = conflictClause;
    return *this;
}

TableConstraint &
TableConstraint::withPrimaryKey(const std::list<IndexedColumn> &indexedColumns,
                                const ConflictClause &conflictClause)
{
    lang::TableConstraint &lang = getMutableLang();
    lang.type = lang::TableConstraint::Type::PrimaryKey;
    for (const IndexedColumn &indexedColumn : indexedColumns) {
        lang.indexedColumns.append(indexedColumn.getLang());
    }
    lang.conflictClause = conflictClause;
    return *this;
}

TableConstraint &
TableConstraint::withUnique(const IndexedColumn &indexedColumn,
                            const ConflictClause &conflictClause)
{
    lang::TableConstraint &lang = getMutableLang();
    lang.type = lang::TableConstraint::Type::Unique;
    lang.indexedColumns.append(indexedColumn.getLang());
    lang.conflictClause = conflictClause;
    return *this;
}

TableConstraint &
TableConstraint::withUnique(const std::list<IndexedColumn> &indexedColumns,
                            const ConflictClause &conflictClause)
{
    lang::TableConstraint &lang = getMutableLang();
    lang.type = lang::TableConstraint::Type::Unique;
    for (const IndexedColumn &indexedColumn : indexedColumns) {
        lang.indexedColumns.append(indexedColumn.getLang());
    }
    lang.conflictClause = conflictClause;
    return *this;
}

TableConstraint &TableConstraint::withChecking(const Expression &expression)
{
    lang::TableConstraint &lang = getMutableLang();
    lang.type = lang::TableConstraint::Type::Check;
    lang.expr.assign(expression.getLang());
    return *this;
}

TableConstraint &
TableConstraint::withForeignKey(const std::string &columnName,
                                const ForeignKeyClause &foreignKeyClause)
{
    lang::TableConstraint &lang = getMutableLang();
    lang.type = lang::TableConstraint::Type::ForeignKey;
    lang.columnNames.append(columnName);
    lang.foreignKeyClause.assign(foreignKeyClause.getLang());
    return *this;
}

TableConstraint &
TableConstraint::withForeignKey(const std::list<std::string> &columnNames,
                                const ForeignKeyClause &foreignKeyClause)
{
    lang::TableConstraint &lang = getMutableLang();
    lang.type = lang::TableConstraint::Type::ForeignKey;
    for (const std::string &columnName : columnNames) {
        lang.columnNames.append(columnName);
    }
    lang.foreignKeyClause.assign(foreignKeyClause.getLang());
    return *this;
}

} // namespace WCDB
