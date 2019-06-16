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

#include <WCDB/Syntax.h>
#include <WCDB/SyntaxAssertion.hpp>
#include <WCDB/SyntaxEnum.hpp>

namespace WCDB {

template<>
constexpr const char* Enum::description(const Syntax::RaiseFunction::Switch& switcher)
{
    switch (switcher) {
    case Syntax::RaiseFunction::Switch::Ignore:
        return "IGNORE";
    case Syntax::RaiseFunction::Switch::Rollback:
        return "ROLLBACK";
    case Syntax::RaiseFunction::Switch::Abort:
        return "ABORT";
    case Syntax::RaiseFunction::Switch::Fail:
        return "FAIL";
    }
}

namespace Syntax {

#pragma mark - Identifier
Identifier::Type RaiseFunction::getType() const
{
    return type;
}

StringView RaiseFunction::getValidDescription() const
{
    std::ostringstream stream;
    stream << "RAISE(" << switcher;
    switch (switcher) {
    case Switch::Ignore:
        break;
    case Switch::Abort:
    case Switch::Rollback:
    case Switch::Fail:
        stream << ", " << errorMessage;
        break;
    }
    stream << ")";
    return StringView(stream.str());
}

} // namespace Syntax

} // namespace WCDB
