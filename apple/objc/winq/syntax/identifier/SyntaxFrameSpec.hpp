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

#pragma once

#include <WCDB/SyntaxIdentifier.hpp>

namespace WCDB {

namespace Syntax {

class Expression;

class FrameSpec final : public Identifier {
#pragma mark - Lang
public:
    ~FrameSpec() override final;

    WCDB_SYNTAX_MAIN_UNION_ENUM(Range, Rows, );

    enum class Event {
        Single = 1,
        Between,
    } event;
    enum class FirstEvent {
        UnboundedPreceding = 1,
        Preceding,
        Following,
        CurrentRow,
    } firstEvent;
    enum class SecondEvent {
        UnboundedFollowing = 1,
        Preceding,
        Following,
        CurrentRow,
    } secondEvent;
    bool between = false;
    Shadow<Expression> expression1;
    Shadow<Expression> expression2;

#pragma mark - Identifier
public:
    static constexpr const Type type = Type::FrameSpec;
    Type getType() const override final;
    bool describle(std::ostringstream& stream) const override final;
    void iterate(const Iterator& iterator, bool& stop) override final;
};

} // namespace Syntax

} // namespace WCDB
