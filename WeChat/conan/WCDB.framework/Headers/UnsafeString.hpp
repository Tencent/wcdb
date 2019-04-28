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

#ifndef __WCDB_UNSAFE_STRING_HPP
#define __WCDB_UNSAFE_STRING_HPP

#include <string>

namespace WCDB {

// TODO: refactor to readonly string
class UnsafeString final {
public:
    UnsafeString();
    UnsafeString(const char* str, ssize_t length);

    template<typename T, typename Enable = void>
    struct Convertible : public std::false_type {
    public:
        static UnsafeString asUnsafeString(const T&);
    };

    template<typename T, typename Enable = typename std::enable_if<Convertible<T>::value>::type>
    UnsafeString(const T& t) : UnsafeString(Convertible<T>::asUnsafeString(t))
    {
    }

    static const UnsafeString& null();

    bool isCaseInsensiveEqual(const UnsafeString& target) const;
    int caseInsensiveCompare(const UnsafeString& target) const;
    const char* cstring() const;
    size_t length() const;

    operator const char*() const;

protected:
    const char* m_cstring;
    ssize_t m_length;

private:
    static const char* emptyCString();
};

} //namespace WCDB

#endif /* __WCDB_UNSAFE_STRING_HPP */
