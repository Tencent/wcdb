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

#include <WCDB/Lang.h>

namespace WCDB {

namespace Lang {

CopyOnWriteString JoinClause::SQL() const
{
    std::string description;
    LangDebugAssert(!tableOrSubquery.empty());
    description.append(tableOrSubquery.description().get());
    if (!joinOperands.empty()) {
        if (joinOperands.get().front().get().joinOperator.get().join) {
            description.append(" ");
        }
        description.append(joinOperands.description().get());
    }
    return description;
}

CopyOnWriteString JoinClause::Operand::SQL() const
{
    std::string description;
    LangDebugAssert(!joinOperator.empty());
    LangDebugAssert(!tableOrSubquery.empty());
    description.append(joinOperator.description().get() + " " +
                       tableOrSubquery.description().get());
    if (!joinConstraint.empty()) {
        description.append(" " + joinConstraint.description().get());
    }
    return description;
}

template <>
CopyOnWriteString
CopyOnWriteLazyLangList<JoinClause::Operand>::calculatedDescription() const
{
    std::string description;
    bool space = false;
    for (const auto &element : this->get()) {
        if (space) {
            description.append(" ");
        } else {
            space = true;
        }
        LangDebugAssert(!element.empty());
        description.append(element.description().get());
    }
    return description;
}

} // namespace Lang

} // namespace WCDB
