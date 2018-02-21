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

UpdateSTMT::UpdateSTMT() : type(Type::NotSet), offset(false)
{
}

copy_on_write_string UpdateSTMT::SQL() const
{
    std::string description;
    if (!withClause.empty()) {
        description.append(withClause.description().get() + " ");
    }
    description.append(UpdateSTMT::TypeName(type));
    description.append(" ");
    assert(!qualifiedTableName.empty());
    description.append(qualifiedTableName.description().get());

    assert(!keyValues.empty());
    description.append(" SET ");
    description.append(keyValues.description().get());

    if (!condition.empty()) {
        description.append(" WHERE " + condition.description().get());
    }
    if (!orderingTerm.empty()) {
        description.append(" ORDER BY " + orderingTerm.description().get());
        assert(!limit.empty());
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

constexpr const char *UpdateSTMT::TypeName(const Type &type)
{
    switch (type) {
        case Type::Update:
            return "UPDATE";
        case Type::UpdateOrRollback:
            return "UPDATE OR ROLLBACK";
        case Type::UpdateOrAbort:
            return "UPDATE OR ABORT";
        case Type::UpdateOrReplace:
            return "UPDATE OR REPLACE";
        case Type::UpdateOrFail:
            return "UPDATE OR FAIL";
        case Type::UpdateOrIgnore:
            return "UPDATE OR IGNORE";
        default:
            assert(false);
            break;
    }
}

copy_on_write_string UpdateSTMT::KeyValue::SQL() const
{
    return keys.description().get() + " = " + value.description().get();
}

} // namespace lang

} // namespace WCDB
