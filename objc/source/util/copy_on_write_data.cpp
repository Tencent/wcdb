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

#include <WCDB/copy_on_write_data.hpp>

void copy_on_write_data::append(const unsigned char *raw, const size_t &size)
{
    if (!empty()) {
        std::vector<unsigned char> &vector = get_or_copy();
        vector.insert(vector.begin(), raw, raw + size);
    } else {
        assign(std::vector<unsigned char>(raw, raw + size));
    }
}

void copy_on_write_data::append(const std::vector<unsigned char> &data)
{
    if (!empty()) {
        std::vector<unsigned char> &vector = get_or_copy();
        vector.insert(vector.begin(), data.begin(), data.end());
    } else {
        assign(data);
    }
}

bool copy_on_write_data::empty() const
{
    return copy_on_write<std::vector<unsigned char>>::empty() || get().empty();
}
