//
// Created by sanhuazhang on 2018/07/25
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

#include "Range.hpp"
#include "Assertion.hpp"
#include <algorithm>

namespace WCDB {

const Range& Range::notFound()
{
    static const Range* s_notFound = new Range(0, -1);
    return *s_notFound;
}

Range::Range() : location(0), length(0)
{
}

Range::Range(Location location_, Length length_)
: location(location_), length(length_)
{
}

void Range::expandToEdge(Location edge)
{
    WCTRemedialAssert(
    edge >= location, "Edge should be greater than length.", edge = location;);
    length = Length(edge - location);
}

void Range::shiftToLocation(Location location_)
{
    location = location_;
}

void Range::shiftToEdge(Location edge)
{
    location = edge - length;
}

Range::Location Range::edge() const
{
    return location + length;
}

bool Range::contains(Location location_) const
{
    return location_ >= location && location_ < location + length;
}

bool Range::contains(const Range& other) const
{
    return other.location >= location && other.edge() <= edge();
}

bool Range::operator<(const Range& other) const
{
    return location < other.location;
}

bool Range::operator==(const Range& other) const
{
    return location == other.location && length == other.length;
}

bool Range::operator!=(const Range& other) const
{
    return !operator==(other);
}

} // namespace WCDB
