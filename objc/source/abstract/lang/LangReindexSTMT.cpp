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

ReindexSTMT::ReindexSTMT() : switcher(Switch::NotSet)
{
}

CopyOnWriteString ReindexSTMT::SQL() const
{
    std::string description("REINDEX");
    switch (switcher) {
    case Switch::Collation:
        LangRemedialAssert(!collationName.empty());
        description.append(" " + collationName.get());
        break;
    case Switch::TableOrIndex:
        description.append(" ");
        if (schemaName.isNull()) {
            description.append(mainSchema() + ".");
        } else if (!schemaName.get().empty()) {
            description.append(schemaName.get() + ".");
        }
        LangRemedialAssert(!tableOrIndexName.empty());
        description.append(tableOrIndexName.get());
        break;
    default:
        break;
    }
    return description;
}

STMT::Type ReindexSTMT::getSTMTType() const
{
    return STMT::Type::Reindex;
}

STMT::Type ReindexSTMT::getType()
{
    return STMT::Type::Reindex;
}

} // namespace Lang

} // namespace WCDB
