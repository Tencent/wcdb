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
constexpr const char* Enum::description(const Syntax::CreateTriggerSTMT::Timing& timing)
{
    switch (timing) {
    case Syntax::CreateTriggerSTMT::Timing::Before:
        return "BEFORE";
    case Syntax::CreateTriggerSTMT::Timing::After:
        return "AFTER";
    case Syntax::CreateTriggerSTMT::Timing::InsteadOf:
        return "INSTEAD OF";
    }
}

template<>
constexpr const char* Enum::description(const Syntax::CreateTriggerSTMT::Event& event)
{
    switch (event) {
    case Syntax::CreateTriggerSTMT::Event::Delete:
        return "DELETE";
    case Syntax::CreateTriggerSTMT::Event::Insert:
        return "INSERT";
    case Syntax::CreateTriggerSTMT::Event::Update:
        return "UPDATE";
    }
}

namespace Syntax {

CreateTriggerSTMT::~CreateTriggerSTMT() = default;

bool CreateTriggerSTMT::isValid() const
{
    return !trigger.empty();
}

#pragma mark - Identifier
Identifier::Type CreateTriggerSTMT::getType() const
{
    return type;
}

bool CreateTriggerSTMT::describle(std::ostream& stream) const
{
    stream << "CREATE ";
    if (temp) {
        stream << "TEMP ";
    }
    stream << "TRIGGER ";
    if (ifNotExists) {
        stream << "IF NOT EXISTS ";
    }
    if (!temp && !schema.empty()) {
        stream << schema << ".";
    }
    stream << trigger << space;
    if (specifiedTiming) {
        stream << timing << space;
    }
    stream << event << space;
    if (event == Event::Update && !columns.empty()) {
        stream << "OF " << columns << space;
    }
    stream << "ON " << table << space;
    if (forEachFow) {
        stream << "FOR EACH ROW ";
    }
    if (WCDB_SYNTAX_CHECK_OPTIONAL_VALID(condition)) {
        stream << "WHEN " << condition.value() << space;
    }
    stream << "BEGIN ";
    auto insert = inserts.begin();
    auto delete_ = deletes.begin();
    auto update = updates.begin();
    auto select = selects.begin();
    for (const auto& stmt : stmts) {
        switch (stmt) {
        case STMT::Insert:
            WCTSyntaxRemedialAssert(insert != inserts.end());
            if (!insert->describle(stream, true)) {
                return false;
            }
            ++insert;
            break;
        case STMT::Update:
            WCTSyntaxRemedialAssert(update != updates.end());
            if (!update->describle(stream, true)) {
                return false;
            }
            ++update;
            break;
        case STMT::Delete:
            WCTSyntaxRemedialAssert(delete_ != deletes.end());
            if (!delete_->describle(stream, true)) {
                return false;
            }
            ++delete_;
            break;
        case STMT::Select:
            WCTSyntaxRemedialAssert(select != selects.end());
            stream << *select;
            ++select;
            break;
        }
        stream << "; ";
    }
    stream << "END";
    return true;
}

void CreateTriggerSTMT::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, true, stop);
    recursiveIterate(schema, iterator, stop);
    if (event == Event::Update) {
        listIterate(columns, iterator, stop);
    }
    if (WCDB_SYNTAX_CHECK_OPTIONAL_VALID(condition)) {
        recursiveIterate(condition.value(), iterator, stop);
    }
    auto insert = inserts.begin();
    auto delete_ = deletes.begin();
    auto update = updates.begin();
    auto select = selects.begin();
    for (const auto& stmt : stmts) {
        switch (stmt) {
        case STMT::Insert:
            WCTIterateRemedialAssert(insert != inserts.end());
            insert->iterate(iterator, stop);
            ++insert;
            break;
        case STMT::Update:
            WCTIterateRemedialAssert(update != updates.end());
            update->iterate(iterator, stop);
            ++update;
            break;
        case STMT::Delete:
            WCTIterateRemedialAssert(delete_ != deletes.end());
            delete_->iterate(iterator, stop);
            ++delete_;
            break;
        case STMT::Select:
            WCTIterateRemedialAssert(select != selects.end());
            select->iterate(iterator, stop);
            ++select;
            break;
        }
    }
    Identifier::iterate(iterator, false, stop);
}

} // namespace Syntax

} // namespace WCDB
