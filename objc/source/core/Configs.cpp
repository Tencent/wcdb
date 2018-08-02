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

#include <WCDB/Assertion.hpp>
#include <WCDB/BasicConfig.hpp>
#include <WCDB/CheckpointConfig.hpp>
#include <WCDB/Configs.hpp>
#include <WCDB/PerformanceTraceConfig.hpp>
#include <WCDB/SQLTraceConfig.hpp>

namespace WCDB {

#pragma mark - Configs
std::shared_ptr<Configs>
Configs::configsBySettingConfig(const std::string &name,
                                const std::shared_ptr<Config> &config,
                                int priority) const
{
    WCTInnerAssert(config != nullptr);
    std::shared_ptr<Configs> configs(new Configs(m_list));
    configs->m_list.insert(priority, name, config);
    return configs;
}

std::shared_ptr<Configs> Configs::configsByRemovingConfig(const std::string &name) const
{
    std::shared_ptr<Configs> configs(new Configs(m_list));
    configs->m_list.erase(name);
    return configs;
}

bool Configs::invoke(Handle *handle)
{
    for (const auto &element : m_list.elements()) {
        if (!element.value->invoke(handle)) {
            return false;
        }
    }
    return true;
}

bool Configs::equal(const std::shared_ptr<Configs> &configs) const
{
    return this == configs.get();
}

Configs::Configs(const OrderedUniqueList<std::string, std::shared_ptr<Config>> &list)
: m_list(list)
{
}

Configs::Configs(OrderedUniqueList<std::string, std::shared_ptr<Config>> &&list)
: m_list(std::move(list))
{
}

} //namespace WCDB
