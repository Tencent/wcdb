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

#include <WCDB/describable.hpp>
#include <string.h>

namespace WCDB {

template <typename T>
std::string GetListDescription(const std::list<T> &t)
{
    return Describable::GetListDescription(t);
}

struct CaseInsensiveComparetor {
    bool operator()(const std::string &lhs, const std::string &rhs) const
    {
        return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
    }
};

} //namespace WCDB

#endif /* utility_hpp */
