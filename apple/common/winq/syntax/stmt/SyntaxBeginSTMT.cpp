//
// Created by sanhuazhang on 2019/05/02
//

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

#include "Syntax.h"
#include "SyntaxAssertion.hpp"
#include "SyntaxEnum.hpp"

namespace WCDB {

template<>
constexpr const char* Enum::description(const Syntax::BeginSTMT::Switch& switcher)
{
    switch (switcher) {
    case Syntax::BeginSTMT::Switch::Deferred:
        return "DEFERRED";
    case Syntax::BeginSTMT::Switch::Immediate:
        return "IMMEDIATE";
    case Syntax::BeginSTMT::Switch::Exclusive:
        return "EXCLUSIVE";
    }
}

namespace Syntax {

BeginSTMT::~BeginSTMT() = default;

#pragma mark - Identifier
Identifier::Type BeginSTMT::getType() const
{
    return type;
}

bool BeginSTMT::describle(std::ostream& stream) const
{
    stream << "BEGIN " << switcher;
    return true;
}

} // namespace Syntax

} // namespace WCDB
