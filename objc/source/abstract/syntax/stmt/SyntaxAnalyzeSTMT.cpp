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
Identifier::Type AnalyzeSTMT::getType() const
{
    return type;
}

std::string AnalyzeSTMT::getDescription() const
{
    std::ostringstream stream;
    stream << "ANALYZE";
    switch (switcher) {
    case Switch::Analyze:
        break;
    case Switch::Schema:
        stream << space << schema;
        break;
    case Switch::TableOrIndex:
        stream << space << schema << "." << tableOrIndex;
        break;
    }
    return stream.str();
}

void AnalyzeSTMT::iterate(const Iterator& iterator, void* parameter)
{
    Identifier::iterate(iterator, parameter);
    switch (switcher) {
    case Switch::Analyze:
        break;
    case Switch::Schema:
        // fallthrough
    case Switch::TableOrIndex:
        schema.iterate(iterator, parameter);
        break;
    }
}

} // namespace Syntax

} // namespace WCDB
