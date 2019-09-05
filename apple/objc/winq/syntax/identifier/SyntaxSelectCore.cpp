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

#include <WCDB/Syntax.h>
#include <WCDB/SyntaxAssertion.hpp>

namespace WCDB {

namespace Syntax {

SelectCore::~SelectCore() = default;

#pragma mark - Identifier
Identifier::Type SelectCore::getType() const
{
    return type;
}

bool SelectCore::describle(std::ostringstream& stream) const
{
    switch (switcher) {
    case Switch::Select:
        stream << "SELECT ";
        if (distinct) {
            stream << "DISTINCT ";
        }
        stream << resultColumns;
        if (!tableOrSubqueries.empty() || joinClause.isValid()) {
            stream << " FROM ";
            if (!tableOrSubqueries.empty()) {
                stream << tableOrSubqueries;
            } else {
                stream << joinClause;
            }
        }
        if (condition.isValid()) {
            stream << " WHERE " << condition;
        }
        if (!groups.empty()) {
            stream << " GROUP BY " << groups;
            if (having.isValid()) {
                stream << " HAVING " << having;
            }
        }
        if (!windows.empty()) {
            WCTSyntaxRemedialAssert(windows.size() == windowDefs.size());
            stream << " WINDOW ";
            auto window = windows.begin();
            auto windowDef = windowDefs.begin();
            bool comma = false;
            while (window != windows.end() && windowDef != windowDefs.end()) {
                if (comma) {
                    stream << ", ";
                } else {
                    comma = true;
                }
                stream << *window << " AS " << *windowDef;
                ++window;
                ++windowDef;
            }
        }
        break;
    case Switch::Values:
        stream << "VALUES";
        bool comma = false;
        for (const auto& values : valuesList) {
            if (comma) {
                stream << ", ";
            } else {
                comma = true;
            }
            stream << "(" << values << ")";
        }
        break;
    }
    return true;
}

void SelectCore::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, stop);
    switch (switcher) {
    case Switch::Select:
        listIterate(resultColumns, iterator, stop);
        if (!tableOrSubqueries.empty()) {
            listIterate(tableOrSubqueries, iterator, stop);
        } else if (joinClause.isValid()) {
            recursiveIterate(joinClause, iterator, stop);
        }
        if (condition.isValid()) {
            recursiveIterate(condition, iterator, stop);
        }
        if (!groups.empty()) {
            listIterate(groups, iterator, stop);
            if (having.isValid()) {
                recursiveIterate(having, iterator, stop);
            }
        }
        if (!windows.empty()) {
            WCTIterateRemedialAssert(windows.size() == windowDefs.size());
            listIterate(windowDefs, iterator, stop);
        }
        break;
    case Switch::Values:
        for (auto& values : valuesList) {
            listIterate(values, iterator, stop);
        }
        break;
    }
}

} // namespace Syntax

} // namespace WCDB
