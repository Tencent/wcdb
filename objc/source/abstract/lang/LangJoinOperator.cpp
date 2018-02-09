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

JoinOperator::JoinOperator() : join(false), natural(false), type(Type::NotSet)
{
}

copy_on_write_string JoinOperator::SQL() const
{
    std::string description;
    if (join) {
        description.append(", ");
    } else {
        if (natural) {
            description.append("NATURAL ");
        }
        if (type != Type::NotSet) {
            description.append(JoinOperator::JoinTypeName(type));
            description.append(" ");
        }
        description.append("JOIN");
    }
    return description;
}

constexpr const char *JoinOperator::JoinTypeName(const Type &type)
{
    switch (type) {
        case Type::NotSet:
            return "";
        case Type::Left:
            return "LEFT";
        case Type::LeftOuter:
            return "LEFT OUTER";
        case Type::Inner:
            return "INNER";
        case Type::Cross:
            return "CROSS";
    }
}

} // namespace lang

} // namespace WCDB
