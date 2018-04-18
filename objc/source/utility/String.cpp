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

namespace WCDB {

namespace String {

const std::string &empty()
{
    static const std::string s_empty("");
    return s_empty;
}

bool CaseInsensiveComparator::operator()(const std::string &lhs,
                                         const std::string &rhs) const
{
    return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
}

CopyOnWriteString
stringByReplacingOccurrencesOfString(const CopyOnWriteString &cowOrigin,
                                     const std::string &target,
                                     const std::string &replacement)
{
    size_t last = 0;
    const std::string &origin = cowOrigin.get();
    size_t found = origin.find(target, last);
    if (found == std::string::npos) {
        //quick return for no replacing
        return cowOrigin;
    }
    CopyOnWriteString cowOutput;
    std::string &output = cowOutput.get_or_copy();
    const size_t targetLength = target.length();
    do {
        output.append(origin.substr(last, found - last));
        output.append(replacement);
        last = found + targetLength;
    } while ((found = origin.find(target, last)) != std::string::npos);
    output.append(origin.substr(last, -1));
    return cowOutput;
}

} //namespace String

} //namespace WCDB
