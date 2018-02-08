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

#ifndef copy_on_write_data_hpp
#define copy_on_write_data_hpp

#include <WINQ/copy_on_write.hpp>
#include <vector>

class copy_on_write_data : public copy_on_write<std::vector<unsigned char>> {
public:
    void append(const unsigned char *raw, const size_t &size);
    void append(const std::vector<unsigned char> &data);

    bool empty() const;

private:
    using copy_on_write<std::vector<unsigned char>>::get_or_copy;
};

#endif /* copy_on_write_data_hpp */
