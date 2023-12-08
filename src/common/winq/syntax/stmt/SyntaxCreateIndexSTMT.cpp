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

CreateIndexSTMT::~CreateIndexSTMT() = default;

bool CreateIndexSTMT::isValid() const
{
    return !index.empty();
}

#pragma mark - Identifier
Identifier::Type CreateIndexSTMT::getType() const
{
    return type;
}

bool CreateIndexSTMT::describle(std::ostream& stream) const
{
    stream << "CREATE ";
    if (unique) {
        stream << "UNIQUE ";
    }
    stream << "INDEX ";
    if (ifNotExists) {
        stream << "IF NOT EXISTS ";
    }
    if (!schema.empty()) {
        stream << schema << ".";
    }
    stream << index << " ON " << table << "(" << indexedColumns << ")";
    if (WCDB_SYNTAX_CHECK_OPTIONAL_VALID(condition)) {
        stream << " WHERE " << condition.value();
    }
    return true;
}

void CreateIndexSTMT::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, true, stop);
    recursiveIterate(schema, iterator, stop);
    listIterate(indexedColumns, iterator, stop);
    if (WCDB_SYNTAX_CHECK_OPTIONAL_VALID(condition)) {
        recursiveIterate(condition.value(), iterator, stop);
    }
    Identifier::iterate(iterator, false, stop);
}

} // namespace Syntax

} // namespace WCDB
