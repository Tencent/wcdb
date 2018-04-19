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

IndexedColumn::IndexedColumn() : switcher(Switch::NotSet), order(Order::NotSet)
{
}

CopyOnWriteString IndexedColumn::SQL() const
{
    std::string description;
    switch (switcher) {
        case Switch::Column:
            LangRemedialAssert(!column.empty());
            description.append(column.description().get());
            break;
        case Switch::Expr:
            LangRemedialAssert(!expr.empty());
            description.append(expr.description().get());
            break;
        default:
            LangRemedialFatalError();
            break;
    }
    if (!collationName.empty()) {
        description.append(" COLLATE " + collationName.get());
    }
    if (order != Order::NotSet) {
        description.append(" ");
        description.append(LangOrderName(order));
    }
    return description;
}

} // namespace Lang

} // namespace WCDB
