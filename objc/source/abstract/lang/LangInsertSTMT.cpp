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

InsertSTMT::InsertSTMT() : type(Type::NotSet), switcher(Switch::NotSet)
{
}

CopyOnWriteString InsertSTMT::SQL() const
{
    std::string description;
    if (!withClause.empty()) {
        description.append(withClause.description().get() + " ");
    }
    description.append(InsertSTMT::TypeName(type));
    description.append(" INTO ");
    if (!schemaName.empty()) {
        description.append(schemaName.get() + ".");
    }
    LangRemedialAssert(!tableName.empty());
    description.append(tableName.get());
    if (!columns.empty()) {
        description.append("(" + columns.description().get() + ")");
    }
    switch (switcher) {
        case Switch::Values:
            LangRemedialAssert(!exprs.empty());
            description.append(" VALUES(" + exprs.description().get() + ")");
            break;
        case Switch::Select:
            LangRemedialAssert(!selectSTMT.empty());
            description.append(" " + selectSTMT.description().get());
            break;
        case Switch::DefaultValues:
            description.append(" DEFAULT VALUES");
            break;
        default:
            LangRemedialFatalError();
            break;
    }
    return description;
}

constexpr const char *InsertSTMT::TypeName(const Type &type)
{
    switch (type) {
        case Type::Insert:
            return "INSERT";
        case Type::InsertOrReplace:
            return "INSERT OR REPLACE";
        case Type::InsertOrRollback:
            return "INSERT OR ROLLBACK";
        case Type::InsertOrAbort:
            return "INSERT OR ABORT";
        case Type::InsertOrFail:
            return "INSERT OR FAIL";
        case Type::InsertOrIgnore:
            return "INSERT OR IGNORE";
        default:
            return "";
    }
}

STMT::Type InsertSTMT::getSTMTType() const
{
    return STMT::Type::Insert;
}

STMT::Type InsertSTMT::getType()
{
    return STMT::Type::Insert;
}

} // namespace Lang

} // namespace WCDB
