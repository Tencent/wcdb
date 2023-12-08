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

namespace WCDB {

namespace Syntax {

AnalyzeSTMT::~AnalyzeSTMT() = default;

#pragma mark - Identifier
Identifier::Type AnalyzeSTMT::getType() const
{
    return type;
}

bool AnalyzeSTMT::describle(std::ostream& stream) const
{
    stream << "ANALYZE";
    switch (switcher) {
    case Switch::SchemaOrTableOrIndex:
        stream << space << schema;
        if (!tableOrIndex.empty()) {
            if (schema.isValid()) {
                stream << ".";
            }
            stream << tableOrIndex;
        }
        break;
    default:
        WCTAssert(switcher == Switch::All);
        break;
    }
    return true;
}

void AnalyzeSTMT::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, true, stop);
    switch (switcher) {
    case Switch::SchemaOrTableOrIndex:
        recursiveIterate(schema, iterator, stop);
        break;
    default:
        WCTAssert(switcher == Switch::All);
        break;
    }
    Identifier::iterate(iterator, false, stop);
}

} // namespace Syntax

} // namespace WCDB
