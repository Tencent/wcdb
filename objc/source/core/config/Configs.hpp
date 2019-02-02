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

#ifndef __WCDB_CONFIGS_HPP
#define __WCDB_CONFIGS_HPP

#include <WCDB/Config.hpp>
#include <WCDB/Handle.hpp>
#include <WCDB/OrderedUniqueList.hpp>
#include <WCDB/String.hpp>

namespace WCDB {

class Configs final {
public:
    enum Priority : int {
        Highest = std::numeric_limits<int>::min(),
        Higher = std::numeric_limits<int>::min() + 1,
        High = -100,
        Default = 0,
        Low = 100,
    };

    Configs(OrderedUniqueList<String, std::shared_ptr<Config>> &&list);

    void insert(const String &name,
                const std::shared_ptr<Config> &config,
                int priority = Priority::Default);
    void remove(const String &name);

    bool invoke(Handle *handle);
    bool uninvoke(Handle *handle);

protected:
    OrderedUniqueList<String, std::shared_ptr<Config>> m_list;
};

class ConfigurableHandle : public Handle {
public:
    virtual ~ConfigurableHandle();

    bool reconfigure(const std::shared_ptr<Configs> &newConfigs);

private:
    std::shared_ptr<Configs> m_configs;
};

} //namespace WCDB

#endif /* __WCDB_CONFIGS_HPP */
