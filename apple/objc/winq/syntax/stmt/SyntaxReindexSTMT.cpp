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

#pragma mark - Identifier
Identifier::Type ReindexSTMT::getType() const
{
    return type;
}

StringView ReindexSTMT::getValidDescription() const
{
    std::ostringstream stream;
    stream << "REINDEX";
    switch (switcher) {
    case Switch::NotSet:
        break;
    case Switch::Collation:
        stream << space << collationOrTableOrIndex;
        break;
    case Switch::TableOrIndex:
        stream << space;
        if (!schema.empty()) {
            stream << schema << ".";
        }
        stream << collationOrTableOrIndex;
        break;
    }
    return StringView(stream.str());
}

void ReindexSTMT::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, stop);
    switch (switcher) {
    case Switch::NotSet:
    case Switch::Collation:
        break;
    case Switch::TableOrIndex:
        recursiveIterate(schema, iterator, stop);
        break;
    }
}

} // namespace Syntax

} // namespace WCDB
