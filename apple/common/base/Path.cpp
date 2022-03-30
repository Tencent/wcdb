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

#include <WCDB/Path.hpp>
#include <libgen.h>
#include <pwd.h>
#include <unistd.h>

// TODO std::filesystem is supported since C++17 and Clang with Xcode 11

namespace WCDB {

namespace Path {

StringView addExtention(const UnsafeStringView &base, const UnsafeStringView &extention)
{
    std::ostringstream stream;
    stream << base << extention;
    return StringView(stream.str());
}

StringView addComponent(const UnsafeStringView &base, const UnsafeStringView &component)
{
    std::ostringstream stream;
    stream << base;
    if (base.empty() || base[base.length() - 1] != '/') {
        stream << "/";
    }
    stream << component;
    return StringView(stream.str());
}

StringView getFileName(const UnsafeStringView &base)
{
    std::string file = base.data();
    file = basename(&(*file.begin()));
    return StringView(std::move(file));
}

StringView getDirectoryName(const UnsafeStringView &base)
{
    std::string dir = base.data();
    dir = dirname(&(*dir.begin()));
    return StringView(std::move(dir));
}

StringView normalize(const UnsafeStringView &path)
{
    std::string normalized = path.data();
    // replace '//' with '/'
    std::size_t found;
    while ((found = normalized.find("//")) != std::string::npos) {
        normalized.replace(found, 2, "/");
    }
    return StringView(std::move(normalized));
}

} //namespace Path

} //namespace WCDB
