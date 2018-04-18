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

#include <WCDB/Lang.h>

namespace WCDB {

namespace Lang {

QualifiedTableName::QualifiedTableName() : indexSwitcher(IndexSwitch::NotSet)
{
}

CopyOnWriteString QualifiedTableName::SQL() const
{
    std::string description;
    if (!schemaName.empty()) {
        description.append(schemaName.get() + ".");
    }
    LangDebugAssert(!tableName.empty());
    description.append(tableName.get());
    switch (indexSwitcher) {
        case IndexSwitch::Indexed:
            LangDebugAssert(!indexName.empty());
            description.append(" INDEXED BY " + indexName.get());
            break;
        case IndexSwitch::NotIndexed:
            description.append(" NOT INDEXED");
            break;
        default:
            break;
    }
    return description;
}

} // namespace Lang

} // namespace WCDB
