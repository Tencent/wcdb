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

#include <string>

namespace WCDB {

class UnsafeStringView final : public std::string_view {
public:
    using std::string_view::string_view;
    
    template<typename T, typename Enable = void>
    struct Convertible : public std::false_type {
    public:
        static UnsafeStringView asUnsafeString(const T&);
    };

    template<typename T, typename Enable = typename std::enable_if<Convertible<T>::value>::type>
    UnsafeStringView(const T& t) : UnsafeStringView(Convertible<T>::asUnsafeString(t))
    {
    }
    
    const char* data() const {
        return std::string_view::data();
    }

    bool isCaseInsensiveEqual(const UnsafeStringView& target) const;
    int caseInsensiveCompare(const UnsafeStringView& target) const;
};

} //namespace WCDB
