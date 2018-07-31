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

std::string addExtention(const std::string &base, const std::string &extention)
{
    return base + extention;
}

std::string addComponent(const std::string &base, const std::string &component)
{
    std::string newPath = base;
    if (newPath.empty() || newPath[newPath.size() - 1] != '/') {
        newPath.append("/");
    }
    newPath.append(component);
    return newPath;
}

std::string getFileName(const std::string &base)
{
    size_t size = base.size();
    bool findLast = true;
    const char *cstr = base.c_str();
    size_t begin = 0, end = 0, length = 0;
    for (size_t i = 0; i < size; ++i) {
        size_t pos = size - i - 1;
        if (findLast) {
            if (cstr[pos] != '/') {
                end = pos;
                findLast = false;
            }
        } else {
            if (cstr[pos] == '/') {
                length = end - pos;
                begin = pos + 1;
                break;
            }
        }
    }
    return length != 0 && size > 0 ? std::string(cstr, begin, length) : "/";
}

std::string getBaseName(const std::string &base)
{
    std::string dir = base;
    dir = dirname(&(*dir.begin()));
    return dir;
}

std::string normalize(const std::string &path)
{
    std::string normalized;

    // expand tilde
    if (path.length() != 0 & path[0] == '~') {
        const char *pfx = NULL;
        std::string::size_type pos = path.find_first_of('/');
        if (path.length() == 1 || pos == 1) {
            pfx = getenv("HOME");
            if (!pfx) {
                struct passwd *pw = getpwuid(getuid());
                if (pw) {
                    pfx = pw->pw_dir;
                }
            }
        } else {
            std::string user(
            path, 1, (pos == std::string::npos) ? std::string::npos : pos - 1);
            struct passwd *pw = getpwnam(user.c_str());
            if (pw) {
                pfx = pw->pw_dir;
            }
        }

        if (pfx) {
            normalized = pfx;
            if (pos != std::string::npos) {
                if (normalized.length() == 0 || normalized[normalized.length() - 1] != '/') {
                    normalized += '/';
                }
                normalized += path.substr(pos + 1);
            }
        } else {
            normalized = path;
        }
    } else {
        normalized = path;
    }

    // replace '//' with '/'
    std::size_t found;
    while ((found = normalized.find("//")) != std::string::npos) {
        normalized.replace(found, 2, "/");
    }

    return normalized;
}

} //namespace Path

} //namespace WCDB
