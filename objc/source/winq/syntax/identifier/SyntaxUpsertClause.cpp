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

#include <WCDB/SyntaxAssertion.hpp>
#include <WCDB/Syntax.h>

namespace WCDB {

namespace Syntax {

#pragma mark - Identifier
Identifier::Type UpsertClause::getType() const
{
    return type;
}

String UpsertClause::getDescription() const
{
    std::ostringstream stream;
    stream << "ON CONFLICT";
    if (!indexedColumns.empty()) {
        stream << "(" << indexedColumns << ")";
        if (useCondition) {
            stream << " WHERE " << condition;
        }
    }
    stream << " DO ";
    if (!columnsList.empty()) {
        WCTSyntaxRemedialAssert(columnsList.size() == expressions.size());
        stream << "UPDATE SET ";
        auto columns = columnsList.begin();
        auto expression = expressions.begin();
        bool comma = false;
        while (columns != columnsList.end() && expression != expressions.end()) {
            if (comma) {
                stream << ", ";
            } else {
                comma = true;
            }
            if (columns->size() > 1) {
                stream << "(" << *columns << ")";
            } else {
                stream << *columns;
            }
            stream << " = " << *expression;
            ++columns;
            ++expression;
        }
        if (useUpdateCondition) {
            stream << " WHERE " << updateCondition;
        }
    } else {
        stream << "NOTHING";
    }
    return stream.str();
}

void UpsertClause::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, stop);
    if (!indexedColumns.empty()) {
        listIterate(indexedColumns, iterator, stop);
        if (useCondition) {
            recursiveIterate(condition, iterator, stop);
        }
    }
    if (!columnsList.empty()) {
        WCTIterateRemedialAssert(columnsList.size() == expressions.size());
        auto columns = columnsList.begin();
        auto expression = expressions.begin();
        while (columns != columnsList.end() && expression != expressions.end()) {
            listIterate(*columns, iterator, stop);
            expression->iterate(iterator, stop);
            ++columns;
            ++expression;
        }
        if (useUpdateCondition) {
            recursiveIterate(updateCondition, iterator, stop);
        }
    }
}

} // namespace Syntax

} // namespace WCDB
