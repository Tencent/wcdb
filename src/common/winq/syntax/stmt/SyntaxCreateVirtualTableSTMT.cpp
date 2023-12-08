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

#include "FTSConst.h"
#include "Syntax.h"
#include "SyntaxAssertion.hpp"

namespace WCDB {

namespace Syntax {

CreateVirtualTableSTMT::~CreateVirtualTableSTMT() = default;

bool CreateVirtualTableSTMT::isValid() const
{
    return !table.empty();
}

#pragma mark - Identifier
Identifier::Type CreateVirtualTableSTMT::getType() const
{
    return type;
}

bool CreateVirtualTableSTMT::describle(std::ostream& stream) const
{
    stream << "CREATE VIRTUAL TABLE ";
    if (ifNotExists) {
        stream << "IF NOT EXISTS ";
    }
    if (!schema.empty()) {
        stream << schema << ".";
    }
    stream << table << " USING " << module;
    bool isFTS5 = module.caseInsensitiveEqual(Module::FTS5);
    StringView tokenizerPrefix = tokenizerPreFix();
    if (!arguments.empty()) {
        stream << "(";
        bool comma = false;
        std::list<StringView> notIndexedColumn;
        for (const auto& argument : arguments) {
            if (comma) {
                stream << ", ";
            } else {
                comma = true;
            }
            offset_t loc = UnsafeStringView::npos;
            if (isFTS5 && (loc = argument.find(tokenizerPrefix)) == 0) {
                stream << tokenizerPrefix << "'";
                stream << UnsafeStringView(argument.data() + 11, argument.length() - 11);
                stream << "'";
            } else if (!isFTS5 && (loc = argument.find("UNINDEXED")) != UnsafeStringView::npos) {
                std::string columnDef = std::string(argument.data()).erase(loc - 1, 10);
                offset_t spaceLoc = argument.find(" ");
                notIndexedColumn.push_back(std::string(argument.data(), spaceLoc));
                stream << columnDef;
            } else {
                stream << argument;
            }
        }
        if (!notIndexedColumn.empty()) {
            for (const auto& column : notIndexedColumn) {
                stream << ", notindexed=";
                stream << column;
            }
        }
        stream << ")";
    }
    return true;
}

void CreateVirtualTableSTMT::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, true, stop);
    recursiveIterate(schema, iterator, stop);
    Identifier::iterate(iterator, false, stop);
}

StringView CreateVirtualTableSTMT::tokenizerPreFix()
{
    return StringView("tokenize = ");
}

} // namespace Syntax

} // namespace WCDB
