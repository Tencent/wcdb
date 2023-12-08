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
constexpr const char* Enum::description(const Syntax::FrameSpec::Switch& switcher)
{
    switch (switcher) {
    case Syntax::FrameSpec::Switch::Range:
        return "RANGE";
    case Syntax::FrameSpec::Switch::Rows:
        return "ROWS";
    }
}

template<>
constexpr const char* Enum::description(const Syntax::FrameSpec::FirstEvent& firstEvent)
{
    switch (firstEvent) {
    case Syntax::FrameSpec::FirstEvent::UnboundedPreceding:
        return "UNBOUNDED PRECEDING";
    case Syntax::FrameSpec::FirstEvent::Preceding:
        return "PRECEDING";
    case Syntax::FrameSpec::FirstEvent::CurrentRow:
        return "CURRENT ROW";
    case Syntax::FrameSpec::FirstEvent::Following:
        return "FOLLOWING";
    }
}

template<>
constexpr const char* Enum::description(const Syntax::FrameSpec::SecondEvent& secondEvent)
{
    switch (secondEvent) {
    case Syntax::FrameSpec::SecondEvent::UnboundedFollowing:
        return "UNBOUNDED FOLLOWING";
    case Syntax::FrameSpec::SecondEvent::Preceding:
        return "PRECEDING";
    case Syntax::FrameSpec::SecondEvent::CurrentRow:
        return "CURRENT ROW";
    case Syntax::FrameSpec::SecondEvent::Following:
        return "FOLLOWING";
    }
}

namespace Syntax {

FrameSpec::~FrameSpec() = default;

#pragma mark - Identifier
Identifier::Type FrameSpec::getType() const
{
    return type;
}

bool FrameSpec::describle(std::ostream& stream) const
{
    stream << switcher << space;
    if (between) {
        stream << "BETWEEN ";
    }
    switch (firstEvent) {
    case FirstEvent::Preceding:
    case FirstEvent::Following:
        stream << *expression1.get() << space;
        // fallthrough
    case FirstEvent::UnboundedPreceding:
    case FirstEvent::CurrentRow:
        stream << firstEvent;
        break;
    }
    if (between) {
        stream << " AND ";
        switch (secondEvent) {
        case SecondEvent::Preceding:
        case SecondEvent::Following:
            stream << *expression2.get() << space;
            // fallthrough
        case SecondEvent::UnboundedFollowing:
        case SecondEvent::CurrentRow:
            stream << secondEvent;
            break;
        }
    }
    return true;
}

void FrameSpec::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, true, stop);
    switch (firstEvent) {
    case FirstEvent::Preceding:
    case FirstEvent::Following:
        expression1.get()->iterate(iterator, stop);
        // fallthrough
    case FirstEvent::UnboundedPreceding:
    case FirstEvent::CurrentRow:
        break;
    }
    if (between) {
        switch (secondEvent) {
        case SecondEvent::Preceding:
        case SecondEvent::Following:
            expression2.get()->iterate(iterator, stop);
            // fallthrough
        case SecondEvent::UnboundedFollowing:
        case SecondEvent::CurrentRow:
            break;
        }
    }
    Identifier::iterate(iterator, false, stop);
}

} // namespace Syntax

} // namespace WCDB
