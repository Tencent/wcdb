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

#include <WCDB/lang.h>

namespace WCDB {

namespace lang {

copy_on_write_string JoinClause::SQL() const
{
    std::string description;
    assert(!tableOrSubquery.empty());
    description.append(tableOrSubquery.description().get());
    if (!joinOperands.empty()) {
        description.append(" " + joinOperands.description().get());
    }
    return description;
}

copy_on_write_string JoinClause::Operand::SQL() const
{
    assert(!joinOperator.empty());
    assert(!tableOrSubquery.empty());
    assert(!joinConstraint.empty());
    return joinOperator.description().get() + " " +
           tableOrSubquery.description().get() + " " +
           joinConstraint.description().get();
}

template <>
copy_on_write_string
copy_on_write_lazy_lang_list<JoinClause::Operand>::calculatedDescription()
    const
{
    std::string description;
    bool comma = false;
    for (const auto &element : this->get()) {
        if (comma) {
            description.append(" ");
        } else {
            comma = true;
        }
        assert(!element.empty());
        description.append(element.description().get());
    }
    return description;
}

} // namespace lang

} // namespace WCDB

