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

BeginSTMT::BeginSTMT() : type(Type::Deferred)
{
}

CopyOnWriteString BeginSTMT::SQL() const
{
    std::string description("BEGIN");
    description.append(" ");
    description.append(BeginSTMT::typeName(type));
    return description;
}

constexpr const char *BeginSTMT::typeName(const Type &type)
{
    switch (type) {
        case Type::Deferred:
            return "DEFERRED";
        case Type::Immediate:
            return "IMMEDIATE";
        case Type::Exclusive:
            return "EXCLUSIVE";
    }
}

STMT::Type BeginSTMT::getSTMTType() const
{
    return STMT::Type::Begin;
}

STMT::Type BeginSTMT::getType()
{
    return STMT::Type::Begin;
}

} // namespace Lang

} // namespace WCDB
