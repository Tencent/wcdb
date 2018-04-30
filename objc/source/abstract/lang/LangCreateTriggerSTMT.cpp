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

CreateTriggerSTMT::CreateTriggerSTMT()
    : temp(false)
    , ifNotExists(false)
    , type(Type::NotSet)
    , operation(Operation::NotSet)
    , forEachRow(false)
{
}

CopyOnWriteString CreateTriggerSTMT::SQL() const
{
    std::string description("CREATE ");
    if (temp) {
        description.append("TEMP ");
    }
    description.append("TRIGGER ");
    if (ifNotExists) {
        description.append("IF NOT EXISTS ");
    }
    if (schemaName.isNull()) {
        if (!temp) {
            description.append(mainSchema() + ".");
        } else {
            description.append(tempSchema() + ".");
        }
    } else if (!schemaName.get().empty()) {
        description.append(schemaName.get() + ".");
    }
    LangRemedialAssert(!triggerName.empty());
    description.append(triggerName.get() + " ");
    if (type != CreateTriggerSTMT::Type::NotSet) {
        description.append(CreateTriggerSTMT::TypeName(type));
        description.append(" ");
    }
    description.append(CreateTriggerSTMT::OperationName(operation));
    if (operation == Operation::Update && !columns.empty()) {
        description.append(" OF " + columns.description().get());
    }
    description.append(" ON " + tableName.get());
    if (forEachRow) {
        description.append(" FOR EACH ROW");
    }
    if (!expr.empty()) {
        description.append(" WHEN " + expr.description().get());
    }
    description.append(" BEGIN ");
    LangRemedialAssert(!STMTs.empty());
    description.append(STMTs.description().get());
    description.append(" END");
    return description;
}

constexpr const char *CreateTriggerSTMT::TypeName(const Type &type)
{
    switch (type) {
        case Type::Before:
            return "BEFORE";
        case Type::After:
            return "AFTER";
        case Type::InsteadOf:
            return "INSTEAD OF";
        default:
            return "";
    }
}

constexpr const char *
CreateTriggerSTMT::OperationName(const Operation &operation)
{
    switch (operation) {
        case Operation::Delete:
            return "DELETE";
        case Operation::Insert:
            return "INSERT";
        case Operation::Update:
            return "UPDATE";
        default:
            return "";
    }
}

template <>
CopyOnWriteString
CopyOnWriteLazyLangList<CRUDSTMT>::calculatedDescription() const
{
    std::string description;
    bool space = false;
    for (const auto &element : this->get()) {
        if (space) {
            description.append(" ");
        } else {
            space = true;
        }
        LangRemedialAssert(!element.empty());
        description.append(element.description().get() + ";");
    }
    return description;
}

STMT::Type CreateTriggerSTMT::getSTMTType() const
{
    return STMT::Type::CreateTrigger;
}

STMT::Type CreateTriggerSTMT::getType()
{
    return STMT::Type::CreateTrigger;
}

} // namespace Lang

} // namespace WCDB
