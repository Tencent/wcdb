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

#include "Tag.hpp"
#include "Assertion.hpp"
#include "CoreConst.h"

namespace WCDB {

const Tag& Tag::invalid()
{
    static const Tag* s_invalid = new Tag(nullptr);
    return *s_invalid;
}

Tag::Tag(long tag) : m_value(tag)
{
    WCTRemedialAssert(m_value != TagInvalidValue, "Tag can't be 0.", return;);
}

Tag::Tag(const std::nullptr_t&) : m_value(TagInvalidValue)
{
}

bool Tag::operator==(const Tag& other) const
{
    return m_value == other.m_value;
}

bool Tag::operator!=(const Tag& other) const
{
    return m_value != other.m_value;
}

Tag::operator long() const
{
    return value();
}

long Tag::value() const
{
    return m_value;
}

bool Tag::isValid() const
{
    return m_value != TagInvalidValue;
}

} // namespace WCDB
