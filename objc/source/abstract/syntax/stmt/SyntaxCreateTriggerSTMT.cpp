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

#include <WCDB/Assertion.hpp>
#include <WCDB/Syntax.h>

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

#pragma mark - Identifier
Identifier::Type CreateTriggerSTMT::getType() const
{
    return type;
}

String CreateTriggerSTMT::getDescription() const
{
    std::ostringstream stream;
    stream << "CREATE ";
    if (temp) {
        stream << "TEMP ";
    }
    stream << "TRIGGER ";
    if (ifNotExists) {
        stream << "IF NOT EXISTS ";
    }
    stream << schema << "." << trigger << space;
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
    if (useCondition) {
        stream << "WHEN " << condition << space;
    }
    stream << "BEGIN ";
    auto insert = inserts.begin();
    auto delete_ = deletes.begin();
    auto update = updates.begin();
    auto select = selects.begin();
    for (const auto& stmt : stmts) {
        switch (stmt) {
        case STMT::Insert:
            SyntaxRemedialAssert(insert != inserts.end());
            stream << *insert << "; ";
            ++insert;
            break;
        case STMT::Update:
            SyntaxRemedialAssert(update != updates.end());
            stream << *update << "; ";
            ++update;
            break;
        case STMT::Delete:
            SyntaxRemedialAssert(delete_ != deletes.end());
            stream << *delete_ << "; ";
            ++delete_;
            break;
        case STMT::Select:
            SyntaxRemedialAssert(select != selects.end());
            stream << *select << "; ";
            ++select;
            break;
        }
    }
    stream << "END";
    return stream.str();
}

void CreateTriggerSTMT::iterate(const Iterator& iterator, void* parameter)
{
    Identifier::iterate(iterator, parameter);
    schema.iterate(iterator, parameter);
    if (event == Event::Update) {
        listIterate(columns, iterator, parameter);
    }
    if (useCondition) {
        condition.iterate(iterator, parameter);
    }
    auto insert = inserts.begin();
    auto delete_ = deletes.begin();
    auto update = updates.begin();
    auto select = selects.begin();
    for (const auto& stmt : stmts) {
        switch (stmt) {
        case STMT::Insert:
            IterateRemedialAssert(insert != inserts.end());
            insert->iterate(iterator, parameter);
            ++insert;
            break;
        case STMT::Update:
            IterateRemedialAssert(update != updates.end());
            update->iterate(iterator, parameter);
            ++update;
            break;
        case STMT::Delete:
            IterateRemedialAssert(delete_ != deletes.end());
            delete_->iterate(iterator, parameter);
            ++delete_;
            break;
        case STMT::Select:
            IterateRemedialAssert(select != selects.end());
            select->iterate(iterator, parameter);
            ++select;
            break;
        }
    }
}

} // namespace Syntax

} // namespace WCDB
