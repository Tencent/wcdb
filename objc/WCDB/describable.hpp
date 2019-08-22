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

#ifndef describable_hpp
#define describable_hpp

#include <list>
#include <string>
#include <type_traits>

namespace WCDB {

class Describable {
public:
    template <typename T>
    static std::string GetListDescription(const std::list<const T> &t)
    {
        std::string s;
        GetDescription(t, s);
        return s;
    }

    const std::string &getDescription() const;

    template <typename T>
    void joinDescribableList(const std::list<const T> &list)
    {
        GetDescription<T>(list, m_description);
    }

    explicit operator const std::string &() const;

    bool isEmpty() const;

protected:
    template <typename T>
    static void GetDescription(const std::list<const T> &list,
                               std::string &output)
    {
        bool flag = false;
        for (const auto &t : list) {
            if (flag) {
                output.append(",");
            } else {
                flag = true;
            }
            output.append(std::string(t));
        }
    }

    Describable(const std::string &description);
    Describable(const char *description);

    std::string m_description;
};

} //namespace WCDB

#endif /* describable_hpp */
