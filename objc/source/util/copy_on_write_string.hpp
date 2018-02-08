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

#ifndef copy_on_write_string_hpp
#define copy_on_write_string_hpp

#include <WINQ/copy_on_write.hpp>
#include <string>

class copy_on_write_string : public copy_on_write<std::string> {
public:
    using Super = copy_on_write<std::string>;

    copy_on_write_string();

    copy_on_write_string(const std::string &string);

    copy_on_write_string(const char *string);

    void append(const std::string &string);

    void append(const copy_on_write_string &string);

    bool empty() const;

private:
    using copy_on_write<std::string>::get_or_copy;
};

#endif /* copy_on_write_string_hpp */
