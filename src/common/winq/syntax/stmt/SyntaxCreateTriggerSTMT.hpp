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

#pragma once

#include "SyntaxIdentifier.hpp"

namespace WCDB {

namespace Syntax {

class InsertSTMT;
class DeleteSTMT;
class UpdateSTMT;
class SelectSTMT;

class WCDB_API CreateTriggerSTMT final : public Identifier {
#pragma mark - Lang
public:
    ~CreateTriggerSTMT() override;

    bool temp = false;
    bool ifNotExists = false;
    Schema schema;
    StringView trigger;
    enum class Timing : signed char {
        Before = 1,
        After,
        InsteadOf,
    } timing;
    bool specifiedTiming = false;
    enum class Event : signed char {
        Delete = 1,
        Insert,
        Update,
    } event;
    std::list<Column> columns;
    StringView table;
    bool forEachFow = false;
    Optional<Expression> condition;

    enum class STMT : signed char {
        Insert = 1,
        Update,
        Delete,
        Select,
    };
    std::list<STMT> stmts;
    std::list<InsertSTMT> inserts;
    std::list<SelectSTMT> selects;
    std::list<UpdateSTMT> updates;
    std::list<DeleteSTMT> deletes;

    bool isValid() const override final;

#pragma mark - Identifier
public:
    static constexpr const Type type = Type::CreateTriggerSTMT;
    Type getType() const override final;
    bool describle(std::ostream& stream) const override final;
    void iterate(const Iterator& iterator, bool& stop) override final;
};

} // namespace Syntax

} // namespace WCDB
