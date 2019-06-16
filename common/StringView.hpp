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

#include <map>
#include <sstream>
#include <string>
#include <string_view>

namespace WCDB {

class UnsafeStringView : public std::string_view {
#pragma mark - UnsafeStringView - Constructor
public:
    UnsafeStringView();
    UnsafeStringView(const char* string);
    UnsafeStringView(const char* string, size_t length);

    UnsafeStringView(const UnsafeStringView& other);
    UnsafeStringView(UnsafeStringView&& other);

    UnsafeStringView& operator=(const UnsafeStringView& other);
    UnsafeStringView& operator=(UnsafeStringView&& other);

    virtual ~UnsafeStringView();

protected:
    UnsafeStringView(std::shared_ptr<const std::string>&& buffer);
    void assign(std::shared_ptr<const std::string>&& buffer);

    static const std::string_view& emptyView();

private:
    friend class StringView;
    std::shared_ptr<const std::string> m_buffer;

#pragma mark - UnsafeStringView - Comparison
public:
    int caseInsensiveCompare(const UnsafeStringView& other) const;
    bool caseInsensiveEqual(const UnsafeStringView& other) const;

    bool hasPrefix(const UnsafeStringView& target) const;

#pragma mark - UnsafeStringView - Operations
public:
    uint32_t hash() const;

private:
    using std::string_view::substr;

#pragma mark - UnsafeStringView - Modifiers
public:
    void clear();

private:
    using std::string_view::remove_prefix;
    using std::string_view::remove_suffix;
    using std::string_view::swap;

#pragma mark - UnsafeStringView - Convertible
public:
    template<typename T, typename Enable = void>
    struct Convertible : public std::false_type {
    public:
        static UnsafeStringView asUnsafeStringView(const T&);
    };

    template<typename T, typename Enable = typename std::enable_if<Convertible<T>::value>::type>
    UnsafeStringView(const T& t)
    : UnsafeStringView(Convertible<T>::asUnsafeStringView(t))
    {
    }
};

class StringView : public UnsafeStringView {
#pragma mark - StringView - Constructor
public:
    StringView();
    StringView(const char* string);
    StringView(const char* string, size_t length);
    StringView(const UnsafeStringView& other);
    StringView(UnsafeStringView&& other);
    StringView(std::string&& string);

    StringView& operator=(const UnsafeStringView& other);
    StringView& operator=(UnsafeStringView&& other);

#pragma mark - StringView - Utility
    static StringView formatted(const char* format, ...);
};

struct StringViewComparator {
    using is_transparent = std::true_type;

    bool operator()(const StringView& lhs, const StringView& rhs) const;

    bool operator()(const StringView& lhs, const UnsafeStringView& rhs) const;
    bool operator()(const UnsafeStringView& lhs, const StringView& rhs) const;
};

template<typename T>
using StringViewMap = std::map<StringView, T, StringViewComparator>;

} // namespace WCDB

std::ostream& operator<<(std::ostream& stream, const WCDB::UnsafeStringView& string);
