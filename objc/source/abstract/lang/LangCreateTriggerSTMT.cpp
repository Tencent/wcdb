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

#include <WCDB/lang.h>

namespace WCDB {

namespace lang {

copy_on_write_string CreateTriggerSTMT::SQL() const
{
    std::string description("CREATE ");
    if (temp) {
        description.append("TEMP ");
    }
    description.append("TRIGGER ");
    if (ifNotExists) {
        description.append("IF NOT EXISTS ");
    }
    if (!schemaName.empty()) {
        description.append(schemaName.get() + ".");
    }
    assert(!triggerName.empty());
    description.append(triggerName.get() + " ");
    description.append(CreateTriggerSTMT::TypeName(type));
    description.append(" ");
    description.append(CreateTriggerSTMT::OperationName(operation));
    if (operation == Operation::Update && !columnNames.empty()) {
        description.append(" ON " + columnNames.description().get());
    }
    if (forEachRow) {
        description.append(" FOR EACH ROW");
    }
    if (!expr.empty()) {
        description.append(" WHERE " + expr.description().get());
    }
    description.append(" BEGIN ");
    assert(!STMTs.empty());
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
    }
}

} // namespace lang

} // namespace WCDB
