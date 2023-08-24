//
// Created by sanhuazhang on 2018/05/31
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

#include "Path.hpp"
#include "Assertion.hpp"

// TODO std::filesystem is supported since C++17 and Clang with Xcode 11

namespace WCDB {

namespace Path {

StringView addExtention(const UnsafeStringView &base, const UnsafeStringView &extention)
{
    std::ostringstream stream;
    stream << base << extention;
    return StringView(stream.str());
}

const char kPathSeparator =
#if defined _WIN32
'\\';
#else
'/';
#endif

StringView addComponent(const UnsafeStringView &base, const UnsafeStringView &component)
{
    std::ostringstream stream;
    stream << base;
    if (base.empty() || base[base.length() - 1] != kPathSeparator) {
        stream << kPathSeparator;
    }
    stream << component;
    return StringView(stream.str());
}

StringView getFileName(const UnsafeStringView &base)
{
    const char *str = base.data();
    WCTAssert(str != nullptr || base.length() == 0);
    for (int i = (int) base.length() - 1; i >= 0; i--) {
        if (str[i] == kPathSeparator) {
            return StringView(str + i + 1, base.length() - i - 1);
        }
    }
    return StringView(base);
}

StringView getDirectory(const UnsafeStringView &path)
{
    const char *str = path.data();
    WCTAssert(str != nullptr);
    for (int i = (int) path.length() - 2; i >= 0; i--) {
        if (str[i] == kPathSeparator) {
            return StringView(str, i + 1);
        }
    }
    return StringView();
}

StringView normalize(const UnsafeStringView &path)
{
    std::string normalized = path.data();
    std::size_t found;
#if defined _WIN32
    const char *doublePathSeparator = "\\\\";
    const char *singlePathSeparator = "\\";
#else
    const char *doublePathSeparator = "//";
    const char *singlePathSeparator = "/";
#endif
    while ((found = normalized.find(doublePathSeparator)) != std::string::npos) {
        normalized.replace(found, 2, singlePathSeparator);
    }
    return StringView(std::move(normalized));
}

} //namespace Path

} //namespace WCDB
