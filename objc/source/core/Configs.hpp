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

#ifndef Configs_hpp
#define Configs_hpp

#include <WCDB/Config.hpp>

#pragma GCC visibility push(hidden)

namespace WCDB {

class Configs {
#pragma mark - Configs
public:
    enum Priority {
        Highest = std::numeric_limits<int>::min(),
        Higher = std::numeric_limits<int>::min() + 1,
        High = -100,
        Default = 0,
        Low = 100,
    };

    static std::shared_ptr<Configs> default_();

    std::shared_ptr<Configs>
    configsBySettingConfig(const std::shared_ptr<Config> &config,
                           int priority = Priority::Default) const;
    std::shared_ptr<Configs>
    configsByRemovingConfig(const std::string &names) const;

    bool invoke(Handle *handle);

    bool equal(const std::shared_ptr<Configs> &configs) const;

#pragma mark - Element
protected:
    struct Element {
        Element(const std::shared_ptr<Config> &config, int priority);
        std::shared_ptr<Config> config;
        int priority;
    };
    typedef struct Element Element;
    Configs(const std::list<Element> &elements);

    void setElement(const Element &element);
    void removeElement(const std::string &name);

    std::list<Element> m_elements;
};

#pragma GCC visibility pop

} //namespace WCDB

#endif /* Configs_hpp */
