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

SelectCore::SelectCore()
    : switcher(Switch::Select)
    , fromSwitcher(FromSwitch::NotSet)
    , distinct(false)
{
}

copy_on_write_string SelectCore::SQL() const
{
    std::string description;
    switch (switcher) {
        case Switch::Select:
            description.append("SELECT ");
            if (distinct) {
                description.append("DISTINCT ");
            }
            assert(!resultColumns.empty());
            description.append(resultColumns.description().get());
            switch (fromSwitcher) {
                case FromSwitch::TableOrSubquery:
                    assert(!tableOrSubquerys.empty());
                    description.append(" FROM " +
                                       tableOrSubquerys.description().get());
                    break;
                case FromSwitch::JoinClause:
                    assert(!joinClause.empty());
                    description.append(" FROM " +
                                       joinClause.description().get());
                    break;
                default:
                    break;
            }
            if (!condition.empty()) {
                description.append(" WHERE " + condition.description().get());
            }
            if (!groups.empty()) {
                description.append(" GROUP BY " + groups.description().get());
                if (!having.empty()) {
                    description.append(" HAVING " + having.description().get());
                }
            }
            break;
        case Switch::Values:
            assert(!values.empty());
            description.append("VALUES(" + values.description().get() + ")");
            break;
    }
    return description;
}

} // namespace lang

} // namespace WCDB
