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

#include "WINQ.h"

namespace WCDB {

FrameSpec::FrameSpec() = default;

FrameSpec::~FrameSpec() = default;

FrameSpec& FrameSpec::range()
{
    syntax().switcher = SyntaxType::Switch::Range;
    return *this;
}

FrameSpec& FrameSpec::rows()
{
    syntax().switcher = SyntaxType::Switch::Rows;
    return *this;
}

FrameSpec& FrameSpec::unboundedPreceding()
{
    syntax().between = false;
    syntax().firstEvent = SyntaxType::FirstEvent::UnboundedPreceding;
    return *this;
}

FrameSpec& FrameSpec::preceding(const Expression& expression)
{
    syntax().between = false;
    syntax().firstEvent = SyntaxType::FirstEvent::Preceding;
    syntax().expression1 = expression;
    return *this;
}

FrameSpec& FrameSpec::currentRow()
{
    syntax().between = false;
    syntax().firstEvent = SyntaxType::FirstEvent::CurrentRow;
    return *this;
}

FrameSpec& FrameSpec::betweenUnboundedPreceding()
{
    syntax().between = true;
    syntax().firstEvent = SyntaxType::FirstEvent::UnboundedPreceding;
    return *this;
}

FrameSpec& FrameSpec::betweenPreceding(const Expression& expression)
{
    syntax().between = true;
    syntax().firstEvent = SyntaxType::FirstEvent::Preceding;
    syntax().expression1 = expression;
    return *this;
}

FrameSpec& FrameSpec::betweenCurrentRow()
{
    syntax().between = true;
    syntax().firstEvent = SyntaxType::FirstEvent::CurrentRow;
    return *this;
}

FrameSpec& FrameSpec::betweenFollowing(const Expression& expression)
{
    syntax().between = true;
    syntax().firstEvent = SyntaxType::FirstEvent::Following;
    syntax().expression1 = expression;
    return *this;
}

FrameSpec& FrameSpec::andPreceding(const Expression& expression)
{
    syntax().between = true;
    syntax().secondEvent = SyntaxType::SecondEvent::Preceding;
    syntax().expression2 = expression;
    return *this;
}

FrameSpec& FrameSpec::andCurrentRow()
{
    syntax().between = true;
    syntax().secondEvent = SyntaxType::SecondEvent::CurrentRow;
    return *this;
}

FrameSpec& FrameSpec::andFollowing(const Expression& expression)
{
    syntax().between = true;
    syntax().secondEvent = SyntaxType::SecondEvent::Following;
    syntax().expression2 = expression;
    return *this;
}

FrameSpec& FrameSpec::andUnboundedFollowing()
{
    syntax().between = true;
    syntax().secondEvent = SyntaxType::SecondEvent::UnboundedFollowing;
    return *this;
}
} // namespace WCDB
