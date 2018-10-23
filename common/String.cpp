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

namespace String {

const std::string &empty()
{
    static const std::string *s_empty = new std::string("");
    return *s_empty;
}

bool CaseInsensiveComparator::operator()(const std::string &lhs, const std::string &rhs) const
{
    return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
}

bool isCaseInsensiveEqual(const std::string &lhs, const std::string &rhs)
{
    return strcasecmp(lhs.c_str(), rhs.c_str()) == 0;
}

bool hasCaseInsensivePrefix(const std::string &origin, const std::string &target)
{
    if (origin.size() < target.size()) {
        return false;
    }
    return strncasecmp(origin.c_str(), target.c_str(), target.size()) == 0;
}

bool hasPrefix(const std::string &origin, const std::string &target)
{
    if (origin.size() < target.size()) {
        return false;
    }
    return strncmp(origin.c_str(), target.c_str(), target.size()) == 0;
}

std::string replacingOccurrencesOfString(const std::string &origin,
                                         const std::string &target,
                                         const std::string &replacement)
{
    size_t last = 0;
    size_t found = origin.find(target, last);
    if (found == std::string::npos) {
        //quick return for no replacing
        return origin;
    }
    std::string output;
    const size_t targetLength = target.length();
    do {
        output.append(origin.substr(last, found - last));
        output.append(replacement);
        last = found + targetLength;
    } while ((found = origin.find(target, last)) != std::string::npos);
    output.append(origin.substr(last, -1));
    return output;
}

std::string formatted(const char *format, ...)
{
    size_t size = strlen(format) * 2 + 50;
    std::string result;
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

uint32_t hash(const std::string &source)
{
    return (uint32_t) crc32(
    0, (const unsigned char *) source.data(), (uint32_t) source.size());
}

} //namespace String

} //namespace WCDB
