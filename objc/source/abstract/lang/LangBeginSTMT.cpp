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

BeginSTMT::BeginSTMT() : type(Type::NotSet)
{
}

copy_on_write_string BeginSTMT::SQL() const
{
    std::string description("BEGIN");
    if (type != Type::NotSet) {
        description.append(" ");
        description.append(BeginSTMT::TypeName(type));
    }
    return description;
}

constexpr const char *BeginSTMT::TypeName(const Type &type)
{
    switch (type) {
        case Type::Deferred:
            return "DEFERRED";
        case Type::Immediate:
            return "IMMEDIATE";
        case Type::Exclusive:
            return "EXCLUSIVE";
        default:
            assert(false);
            break;
    }
}

} // namespace lang

} // namespace WCDB
