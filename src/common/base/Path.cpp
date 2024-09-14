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
#include "CommonCore.hpp"
#include "CrossPlatform.h"
#include "FileManager.hpp"

#include <errno.h>
#include <stdlib.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <conio.h>
#include <direct.h>
#endif

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
    bool foundName = false;
    for (int i = (int) path.length() - 1; i >= 0; i--) {
        if (str[i] != kPathSeparator) {
            // Deal with the case /A/B//
            foundName = true;
        } else if (foundName) {
            return StringView(str, i + 1);
        }
    }
    return StringView();
}

UnsafeStringView normalize(const UnsafeStringView &path)
{
    if (path.equal(":memory:")) {
        return path;
    }
    std::string normalized;
#ifndef _WIN32
    const char *resolvePath = realpath(path.data(), nullptr);
    if (resolvePath == nullptr && errno == ENOENT) {
        CommonCore::shared().setThreadedErrorPath(path);
        if (FileManager::createDirectoryWithIntermediateDirectories(getDirectory(path))
            && FileManager::createFile(path)) {
            resolvePath = realpath(path.data(), nullptr);
            if (resolvePath == NULL) {
                Error error;
                error.level = Error::Level::Warning;
                error.setSystemCode(errno, Error::Code::IOError);
                error.infos.insert_or_assign(ErrorStringKeyPath, path);
                Notifier::shared().notify(error);
            }
            FileManager::removeItem(path);
        }
        CommonCore::shared().setThreadedErrorPath(nullptr);
    }
    if (resolvePath != nullptr) {
        UnsafeStringView newPath = UnsafeStringView(resolvePath);
#ifdef __APPLE__
        /*
         /var is the symlink to /private/var.
         In most cases, realpath will return the path with the /var prefix,
         while in a small number of cases it will return the path with the /private/var prefix.
         In order to avoid the inconsistency of the path of the same file, remove the /private prefix of path here
         */
        if (newPath.hasPrefix("/private")) {
            newPath = UnsafeStringView(resolvePath + 8);
        }
#endif
        normalized = newPath;
        free((void *) resolvePath);
    }
#else
    wchar_t resolvePath[_MAX_PATH];
    if (::_wfullpath(resolvePath, GetPathString(path), _MAX_PATH) != NULL) {
        normalized = StringView::createFromWString(resolvePath);
    }
#endif
    if (normalized.length() > 0) {
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
    } else {
        return path;
    }
}

} //namespace Path

} //namespace WCDB
