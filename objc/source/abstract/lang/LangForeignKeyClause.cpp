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

#include <WINQ/lang.h>

namespace WCDB {

namespace lang {

ForeignKeyClause::ForeignKeyClause()
    : doDeferrable(false)
    , notDeferrable(false)
    , initiallySwitcher(InitiallySwitch::NotSet)
{
}

constexpr const char *
ForeignKeyClause::InitiallySwitchName(const InitiallySwitch &initiallySwitcher)
{
    switch (initiallySwitcher) {
        case InitiallySwitch::Deferred:
            return "INITIALLY DEFERRED";
        case InitiallySwitch::Immediate:
            return "INITIALLY IMMEDIATE";
        default:
            assert(false);
            break;
    }
}

ForeignKeyClause::Trigger::Trigger()
    : operation(Operation::NotSet), match(false), update(false)
{
}

constexpr const char *
ForeignKeyClause::Trigger::OperationName(const Operation &operation)
{
    switch (operation) {
        case Operation::SetNull:
            return "SET NULL";
        case Operation::SetDefault:
            return "SET DEFAULT";
        case Operation::Cascade:
            return "CASCADE";
        case Operation::Restrict:
            return "RESTRICT";
        case Operation::NoAction:
            return "NO ACTION";
        default:
            assert(false);
            break;
    }
}

copy_on_write_string ForeignKeyClause::SQL() const
{
    std::string description("REFERENCES ");
    assert(!foreignTable.empty());
    description.append(foreignTable.get());
    if (!columnNames.empty()) {
        description.append("(" + columnNames.description().get() + ")");
    }
    if (!triggers.empty()) {
        description.append(" " + triggers.description().get());
    }
    if (doDeferrable) {
        if (notDeferrable) {
            description.append(" NOT");
        }
        description.append(" DEFERRABLE");
        if (initiallySwitcher != InitiallySwitch::NotSet) {
            description.append(" ");
            description.append(
                ForeignKeyClause::InitiallySwitchName(initiallySwitcher));
        }
    }
    return description;
}

copy_on_write_string ForeignKeyClause::Trigger::SQL() const
{
    std::string description;
    if (match) {
        assert(!name.empty());
        description.append("MATCH " + name.get());
    } else {
        description.append("ON ");
        if (update) {
            description.append("UPDATE ");
        } else {
            description.append("DELETE ");
        }
        description.append(Trigger::OperationName(operation));
    }
    return description;
}

template <>
copy_on_write_string
copy_on_write_lazy_lang_list<ForeignKeyClause::Trigger>::calculatedDescription()
    const
{
    std::string description;
    bool space = false;
    for (const auto &element : this->get()) {
        if (space) {
            description.append(" ");
        } else {
            space = true;
        }
        assert(!element.empty());
        description.append(element.description().get());
    }
    return description;
}

} // namespace lang

} // namespace WCDB
