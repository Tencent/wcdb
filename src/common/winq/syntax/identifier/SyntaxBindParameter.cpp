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
constexpr const char* Enum::description(const Syntax::BindParameter::Switch& switcher)
{
    switch (switcher) {
    case Syntax::BindParameter::Switch::QuestionSign:
        return "?";
    case Syntax::BindParameter::Switch::ColonSign:
        return ":";
    case Syntax::BindParameter::Switch::DollarSign:
        return "$";
    case Syntax::BindParameter::Switch::AtSign:
        return "@";
    }
}

namespace Syntax {

BindParameter::~BindParameter() = default;

#pragma mark - Identifier
Identifier::Type BindParameter::getType() const
{
    return type;
}

bool BindParameter::describle(std::ostream& stream) const
{
    stream << switcher;
    switch (switcher) {
    case Switch::QuestionSign:
        if (n > 0) {
            stream << n;
        }
        break;
    case Switch::ColonSign:
    case Switch::AtSign:
    case Switch::DollarSign:
        stream << name;
        break;
    }
    return true;
}

} // namespace Syntax

} // namespace WCDB
