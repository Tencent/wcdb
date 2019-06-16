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

bool WithClause::isValid() const
{
    return !tables.empty() && tables.begin()->isValid();
}

#pragma mark - Identifier
Identifier::Type WithClause::getType() const
{
    return type;
}

bool WithClause::describle(std::ostringstream& stream) const
{
    WCTSyntaxRemedialAssert(tables.size() == selects.size());
    stream << "WITH ";
    if (recursive) {
        stream << "RECURSIVE ";
    }
    auto table = tables.begin();
    auto select = selects.begin();
    bool comma = false;
    while (table != tables.end() && select != selects.end()) {
        if (comma) {
            stream << ", ";
        } else {
            comma = true;
        }
        stream << *table << " AS(" << *select << ")";
        ++table;
        ++select;
    }
    return true;
}

void WithClause::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, stop);
    auto table = tables.begin();
    auto select = selects.begin();
    while (table != tables.end() && select != selects.end()) {
        table->iterate(iterator, stop);
        select->iterate(iterator, stop);
        ++table;
        ++select;
    }
}

} // namespace Syntax

} // namespace WCDB
