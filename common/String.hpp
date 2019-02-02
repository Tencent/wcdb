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

#ifndef __WCDB_STRING_HPP
#define __WCDB_STRING_HPP

#include <WCDB/UnsafeString.hpp>
#include <string>

namespace WCDB {

class String final : public std::string {
public:
    using std::string::string;
    String(std::string &&str);
    String(const std::string &str);
    String(const UnsafeString &str);

    template<typename T, typename Enable = void>
    struct Convertible : public std::false_type {
    public:
        static String asString(const T &);
    };

    template<typename T, typename Enable = typename std::enable_if<Convertible<T>::value>::type>
    String(const T &t) : String(Convertible<T>::asString(t))
    {
    }

    operator UnsafeString() const;

    static String formatted(const char *format, ...);
    static const String &null();

    struct CaseInsensiveComparator {
        bool operator()(const String &lhs, const String &rhs) const;
    };
    typedef struct CaseInsensiveComparator CaseInsensiveComparator;

    bool isCaseInsensiveEqual(const String &target) const;
    bool hasCaseInsensivePrefix(const String &target) const;
    bool hasPrefix(const String &target) const;

    String &replacingOccurrencesOfString(const String &target, const String &replacement);

    uint32_t hash() const;
};

template<typename T>
struct String::Convertible<T, typename std::enable_if<UnsafeString::Convertible<T>::value>::type>
: public std::true_type {
public:
    static String asString(const T &t)
    {
        return UnsafeString::Convertible<T>::asUnsafeString(t);
    }
};

} //namespace WCDB

#endif /* Utility_hpp */
