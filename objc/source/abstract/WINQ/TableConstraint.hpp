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

#ifndef TableConstraint_hpp
#define TableConstraint_hpp

#include <WCDB/Describable.hpp>

namespace WCDB {

class TableConstraint : public DescribableWithLang<lang::TableConstraint> {
public:
    TableConstraint();
    TableConstraint(const std::string &name);

    TableConstraint &withPrimaryKey(
        const IndexedColumn &indexedColumn,
        const ConflictClause &conflictClause = ConflictClause::NotSet);
    TableConstraint &withPrimaryKey(
        const std::list<IndexedColumn> &indexedColumns,
        const ConflictClause &conflictClause = ConflictClause::NotSet);

    TableConstraint &
    withUnique(const IndexedColumn &indexedColumn,
               const ConflictClause &conflictClause = ConflictClause::NotSet);
    TableConstraint &
    withUnique(const std::list<IndexedColumn> &indexedColumns,
               const ConflictClause &conflictClause = ConflictClause::NotSet);

    TableConstraint &withChecking(const Expression &expression);

    TableConstraint &withForeignKey(const std::string &columnName,
                                    const ForeignKeyClause &foreignKeyClause);
    TableConstraint &withForeignKey(const std::list<std::string> &columnNames,
                                    const ForeignKeyClause &foreignKeyClause);
};

} // namespace WCDB

#endif /* TableConstraint_hpp */
