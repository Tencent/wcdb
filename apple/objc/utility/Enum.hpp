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

#pragma once

#include <sstream>
#include <type_traits>

namespace WCDB {

class Enum final {
public:
    template<typename T, typename Enable = typename std::enable_if<std::is_enum<T>::value>::type>
    static constexpr const char* description(const T& t);
};

} // namespace WCDB

template<typename T, typename Enable = typename std::enable_if<std::is_enum<T>::value>::type>
std::ostream& operator<<(std::ostream& stream, const T& value)
{
    stream << WCDB::Enum::description(value);
    return stream;
}
