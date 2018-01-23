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

#ifndef utility_hpp
#define utility_hpp

#include <WCDB/convertible.hpp>
#include <WCDB/describable.hpp>
#include <string.h>

namespace WCDB {

struct CaseInsensiveComparetor {
    bool operator()(const std::string &lhs, const std::string &rhs) const
    {
        return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
    }
};

std::string
stringByReplacingOccurrencesOfString(const std::string &origin,
                                     const std::string &target,
                                     const std::string &replacement);

template <typename Container, typename T>
std::string
stringByJoiningList(const Container &container,
                    const std::function<Describable(const T &)> &convertor,
                    const std::string &seperator)
{
    bool flag = false;
    std::string output;
    for (const T &t : container) {
        if (flag) {
            output.append(seperator);
        } else {
            flag = true;
        }
        output.append(convertor(t).getDescription());
    }
    return output;
}

template <typename T>
typename std::enable_if<std::is_base_of<Describable, T>::value,
                        std::string>::type
stringByJoiningList(const std::list<const T> &list,
                    const std::string &seperator = ", ")
{
    bool flag = false;
    std::string output;
    for (const T &t : list) {
        if (flag) {
            output.append(seperator);
        } else {
            flag = true;
        }
        output.append(t.getDescription());
    }
    return output;
}

} //namespace WCDB

#endif /* utility_hpp */
