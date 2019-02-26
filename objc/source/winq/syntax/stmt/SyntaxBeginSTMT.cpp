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

#include <WCDB/SyntaxAssertion.hpp>
#include <WCDB/Enum.hpp>
#include <WCDB/Syntax.h>

namespace WCDB {

template<>
constexpr const char* Enum::description(const Syntax::BeginSTMT::Transaction& transaction)
{
    switch (transaction) {
    case Syntax::BeginSTMT::Transaction::Deferred:
        return "DEFERRED";
    case Syntax::BeginSTMT::Transaction::Immediate:
        return "IMMEDIATE";
    case Syntax::BeginSTMT::Transaction::Exclusive:
        return "EXCLUSIVE";
    }
}

namespace Syntax {

#pragma mark - Identifier
Identifier::Type BeginSTMT::getType() const
{
    return type;
}

String BeginSTMT::getDescription() const
{
    std::ostringstream stream;
    stream << "BEGIN " << transaction;
    return stream.str();
}

} // namespace Syntax

} // namespace WCDB
