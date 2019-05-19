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

#pragma once

#include <WCDB/String.hpp>
#include <functional>
#include <list>

namespace WCDB {

template<typename T>
class CaseInsensiveList : public std::list<std::pair<String, T>> {
public:
    auto caseInsensiveFind(const String& key)
    {
        return std::find_if(
        this->begin(),
        this->end(),
        std::bind(&CaseInsensiveList::caseInsensiveEqual, key, std::placeholders::_1));
    }

    const auto caseInsensiveFind(const String& key) const
    {
        return std::find_if(
        this->begin(),
        this->end(),
        std::bind(&CaseInsensiveList::caseInsensiveEqual, key, std::placeholders::_1));
    }

    void alphabeticallyCaseInsensiveSort() { this->sort(caseInsensiveLess); }

protected:
    static bool caseInsensiveEqual(const String& key, const std::pair<String, T>& element)
    {
        return key.isCaseInsensiveEqual(element.first);
    }

    static bool caseInsensiveLess(const std::pair<String, T>& left,
                                  const std::pair<String, T>& right)
    {
        return left.first.caseInsensiveCompare(right.first) < 0;
    }
};

} // namespace WCDB
