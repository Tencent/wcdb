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

#include <WCDB/copy_on_write_lazy_string_list.hpp>

void copy_on_write_lazy_string_list::append(const copy_on_write_string &string)
{
    if (!empty()) {
        assign({string});
    } else {
        get_or_copy().push_back(string);
    }
}

bool copy_on_write_lazy_string_list::empty() const
{
    return Super::empty() || get().empty();
}

copy_on_write_string
copy_on_write_lazy_string_list::calculatedDescription() const
{
    std::string description;
    bool comma = false;
    for (const auto &element : get()) {
        if (comma) {
            description.append(", ");
        } else {
            comma = true;
        }
        assert(!element.empty());
        description.append(element.get());
    }
    return description;
}
