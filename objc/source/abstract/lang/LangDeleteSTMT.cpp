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

DeleteSTMT::DeleteSTMT() : offset(false)
{
}

CopyOnWriteString DeleteSTMT::SQL() const
{
    std::string description;
    if (!withClause.empty()) {
        description.append(withClause.description().get() + " ");
    }
    LangRemedialAssert(!qualifiedTableName.empty());
    auto qualified = qualifiedTableName;
    if (!withClause.empty() && !withClause.get().pairs.empty() &&
        !qualified.get().tableName.empty() &&
        qualified.get().schemaName.isNull()) {
        const CopyOnWriteString &tableName = qualified.get().tableName;
        for (const auto &pair : withClause.get().pairs.get()) {
            if (pair.empty()) {
                continue;
            }
            const CopyOnWriteLazyLang<CTETableName> &cteTableName =
                pair.get().cteTableName;
            if (!cteTableName.empty() &&
                cteTableName.get().tableName.equal(tableName.get())) {
                //set schema to empty string to avoid default main schema.x
                qualified.get_or_copy().schemaName.assign(anySchema());
            }
        }
    }
    description.append("DELETE FROM " + qualified.description().get());
    if (!condition.empty()) {
        description.append(" WHERE " + condition.description().get());
    }
    if (!orderingTerms.empty()) {
        description.append(" ORDER BY " + orderingTerms.description().get());
        LangRemedialAssert(!limit.empty());
    }
    if (!limit.empty()) {
        description.append(" LIMIT " + limit.description().get());
        if (!limitParameter.empty()) {
            if (offset) {
                description.append(" OFFSET ");
            } else {
                description.append(", ");
            }
            description.append(limitParameter.description().get());
        }
    }
    return description;
}

STMT::Type DeleteSTMT::getSTMTType() const
{
    return STMT::Type::Delete;
}

STMT::Type DeleteSTMT::getType()
{
    return STMT::Type::Delete;
}

} // namespace Lang

} // namespace WCDB
