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

#include <WCDB/String.hpp>
#include <zlib.h>

namespace WCDB {

String::String(const char *str) : std::string(str != nullptr ? str : "")
{
}

String::String(std::string &&str) : std::string(std::move(str))
{
}

String::String(const std::string &str) : std::string(str)
{
}

String::String(const UnsafeStringView &str) : std::string(str.data(), str.length())
{
}

String::operator UnsafeStringView() const
{
    return UnsafeStringView(c_str(), length());
}

const String &String::null()
{
    static const String *s_empty = new String("");
    return *s_empty;
}

bool String::CaseInsensiveComparator::operator()(const String &lhs, const String &rhs) const
{
    return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
}

bool String::isCaseInsensiveEqual(const String &target) const
{
    return caseInsensiveCompare(target) == 0;
}

int String::caseInsensiveCompare(const String &target) const
{
    return strcasecmp(c_str(), target.c_str());
}

bool String::hasCaseInsensivePrefix(const String &target) const
{
    if (size() < target.size()) {
        return false;
    }
    return strncasecmp(c_str(), target.c_str(), target.size()) == 0;
}

bool String::hasPrefix(const String &target) const
{
    if (size() < target.size()) {
        return false;
    }
    return strncmp(c_str(), target.c_str(), target.size()) == 0;
}

String &String::replacingOccurrencesOfString(const String &target, const String &replacement)
{
    size_t last = 0;
    size_t found = find(target, last);
    if (found == String::npos) {
        //quick return for no replacing
        return *this;
    }
    String newString;
    const size_t targetLength = target.length();
    do {
        newString.append(substr(last, found - last));
        newString.append(replacement);
        last = found + targetLength;
    } while ((found = find(target, last)) != String::npos);
    newString.append(substr(last, -1));
    assign(newString);
    return *this;
}

String String::formatted(const char *format, ...)
{
    size_t size = strlen(format) * 2 + 50;
    String result;
    va_list ap;
    do {
        result.resize(size);
        va_start(ap, format);
        int n = vsnprintf((char *) result.data(), size, format, ap);
        va_end(ap);
        if (n > -1 && n < size) {
            result.resize(n);
            return result;
        }
        if (n > -1) {
            size = n + 1;
        } else {
            size *= 2;
        }
    } while (true);
    return result;
}

uint32_t String::hash() const
{
    return (uint32_t) crc32(0, (const unsigned char *) data(), (uint32_t) size());
}

} //namespace WCDB
