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

namespace Syntax {

#pragma mark - Identifier
Identifier::Type SelectCore::getType() const
{
    return type;
}

String SelectCore::getDescription() const
{
    std::ostringstream stream;
    switch (switcher) {
    case Switch::Select:
        stream << "SELECT ";
        if (distinct) {
            stream << "DISTINCT ";
        }
        stream << resultColumns;
        if (useFrom) {
            stream << " FROM ";
            if (!tableOrSubqueries.empty()) {
                stream << tableOrSubqueries;
            } else {
                stream << joinClause;
            }
        }
        if (useCondition) {
            stream << " WHERE " << condition;
        }
        if (!groups.empty()) {
            stream << " GROUP BY " << groups;
            if (useHaving) {
                stream << " HAVING " << having;
            }
        }
        if (!windows.empty()) {
            SyntaxRemedialAssert(windows.size() == windowDefs.size());
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
        stream << "VALUES(";
        bool comma = false;
        for (const auto& values : valuesList) {
            if (comma) {
                stream << ", ";
            } else {
                comma = true;
            }
            stream << values;
        }
        stream << ")";
        break;
    }
    return stream.str();
}

void SelectCore::iterate(const Iterator& iterator, void* parameter)
{
    Identifier::iterate(iterator, parameter);
    switch (switcher) {
    case Switch::Select:
        listIterate(resultColumns, iterator, parameter);
        if (useFrom) {
            if (!tableOrSubqueries.empty()) {
                listIterate(tableOrSubqueries, iterator, parameter);
            } else {
                joinClause.iterate(iterator, parameter);
            }
        }
        if (useCondition) {
            condition.iterate(iterator, parameter);
        }
        if (!groups.empty()) {
            listIterate(groups, iterator, parameter);
            if (useHaving) {
                having.iterate(iterator, parameter);
            }
        }
        if (!windows.empty()) {
            IterateRemedialAssert(windows.size() == windowDefs.size());
            listIterate(windowDefs, iterator, parameter);
        }
        break;
    case Switch::Values:
        for (auto& values : valuesList) {
            listIterate(values, iterator, parameter);
        }
        break;
    }
}

} // namespace Syntax

} // namespace WCDB
