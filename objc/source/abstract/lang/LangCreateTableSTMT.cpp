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

#include <WINQ/lang.h>

namespace WCDB {

namespace lang {

CreateTableSTMT::CreateTableSTMT()
    : temp(false)
    , ifNotExists(false)
    , switcher(Switch::NotSet)
    , withoutRowID(false)
{
}

copy_on_write_string CreateTableSTMT::SQL() const
{
    std::string description("CREATE ");
    if (temp) {
        description.append("TEMP ");
    }
    description.append("TABLE ");
    if (ifNotExists) {
        description.append("IF NOT EXISTS ");
    }
    if (!schemaName.empty()) {
        description.append(schemaName.get() + ".");
    }
    assert(!tableName.empty());
    description.append(tableName.get());
    switch (switcher) {
        case Switch::ColumnDef:
            assert(!columnDefs.empty());
            description.append("(" + columnDefs.description().get());
            if (!tableConstraints.empty()) {
                description.append(", " + tableConstraints.description().get());
            }
            description.append(")");
            if (withoutRowID) {
                description.append(" WITHOUT ROWID");
            }
        case Switch::Select:
            assert(!selectSTMT.empty());
            description.append(" AS " + selectSTMT.description().get());
            break;
        default:
            assert(false);
            break;
    }
    return description;
}

} // namespace lang

} // namespace WCDB
