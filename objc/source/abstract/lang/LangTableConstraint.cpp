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
#include <WCDB/Lang.h>

namespace WCDB {

namespace Lang {

TableConstraint::TableConstraint()
: type(Type::NotSet), conflictClause(ConflictClause::NotSet)
{
}

CopyOnWriteString TableConstraint::SQL() const
{
    std::string description;
    if (!name.empty()) {
        description.append("CONSTRAINT " + name.get() + " ");
    }
    switch (type) {
    case Type::PrimaryKey:
        LangRemedialAssert(!indexedColumns.empty());
        description.append("PRIMARY KEY(" + indexedColumns.description().get() + ")");
        if (conflictClause != ConflictClause::NotSet) {
            description.append(" ");
            description.append(LangConflictClauseName(conflictClause));
        }
        break;
    case Type::Unique:
        LangRemedialAssert(!indexedColumns.empty());
        description.append("UNIQUE(" + indexedColumns.description().get() + ")");
        if (conflictClause != ConflictClause::NotSet) {
            description.append(" ");
            description.append(LangConflictClauseName(conflictClause));
        }
        break;
    case Type::Check:
        LangRemedialAssert(!expr.empty());
        description.append("CHECK(" + expr.description().get() + ")");
        break;
    case Type::ForeignKey:
        LangRemedialAssert(!columns.empty());
        LangRemedialAssert(!foreignKeyClause.empty());
        description.append("FOREIGN KEY(" + columns.description().get() + ") "
                           + foreignKeyClause.description().get());
        break;
    default:
        LangRemedialAssert(false);
        break;
    }
    return description;
}

} // namespace Lang

} // namespace WCDB
