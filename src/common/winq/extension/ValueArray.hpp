//
// Created by qiuwenchen on 2022/8/15.
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

#include <list>
#include <type_traits>
#include <vector>

namespace WCDB {

template<typename T>
class ValueArray : public std::vector<T> {
public:
    using std::vector<T>::vector;
    ValueArray() = default;
    ValueArray(const T& value) { this->push_back(value); }

    template<typename U, typename Enable = typename std::enable_if<std::is_constructible<T, U>::value>::type>
    ValueArray(const U& u)
    {
        this->emplace_back(u);
    }

    template<typename U, typename Enable = typename std::enable_if<std::is_constructible<T, U>::value>::type>
    ValueArray(const std::list<U>& others)
    {
        this->reserve(others.size());
        for (const auto& other : others) {
            this->emplace_back(other);
        }
    }

    template<typename U, typename Enable = typename std::enable_if<std::is_constructible<T, U>::value>::type>
    ValueArray(const std::initializer_list<U>& others)
    {
        this->reserve(others.size());
        for (const auto& other : others) {
            this->emplace_back(other);
        }
    }

    template<typename U, typename Enable = typename std::enable_if<std::is_constructible<T, U>::value>::type>
    ValueArray(const std::vector<U>& others)
    {
        this->reserve(others.size());
        for (const auto& other : others) {
            this->emplace_back(other);
        }
    }
};

} // namespace WCDB
