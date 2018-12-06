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
Identifier::Type CreateIndexSTMT::getType() const
{
    return type;
}

String CreateIndexSTMT::getDescription() const
{
    std::ostringstream stream;
    stream << "CREATE ";
    if (unique) {
        stream << "UNIQUE ";
    }
    stream << "INDEX ";
    if (ifNotExists) {
        stream << "IF NOT EXISTS ";
    }
    stream << schema << "." << index << " ON " << table << "(" << indexedColumns << ")";
    if (useCondition) {
        stream << " WHERE " << condition;
    }
    return stream.str();
}

void CreateIndexSTMT::iterate(const Iterator& iterator, void* parameter)
{
    Identifier::iterate(iterator, parameter);
    schema.iterate(iterator, parameter);
    listIterate(indexedColumns, iterator, parameter);
    if (useCondition) {
        condition.iterate(iterator, parameter);
    }
}

} // namespace Syntax

} // namespace WCDB
