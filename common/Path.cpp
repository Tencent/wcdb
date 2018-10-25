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

#if __cplusplus > 201402L
#warning TODO std::filesystem is supported since C++17
#endif

namespace WCDB {

namespace Path {

String addExtention(const String &base, const String &extention)
{
    return base + extention;
}

String addComponent(const String &base, const String &component)
{
    String newPath = base;
    if (newPath.empty() || newPath[newPath.size() - 1] != '/') {
        newPath.append("/");
    }
    newPath.append(component);
    return newPath;
}

String getFileName(const String &base)
{
    String file = base;
    file = basename(&(*file.begin()));
    return file;
}

String getDirectoryName(const String &base)
{
    String dir = base;
    dir = dirname(&(*dir.begin()));
    return dir;
}

String normalize(const String &path)
{
    String normalized = path;
    // replace '//' with '/'
    std::size_t found;
    while ((found = normalized.find("//")) != String::npos) {
        normalized.replace(found, 2, "/");
    }
    return normalized;
}

} //namespace Path

} //namespace WCDB
