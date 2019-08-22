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

#include <WCDB/config.hpp>

namespace WCDB {

Configs::Configs() : m_configs(new ConfigList)
{
}

Configs::Configs(std::initializer_list<const ConfigWrap> configs)
    : m_configs(new ConfigList)
{
    for (const auto &config : configs) {
        m_configs->push_back(config);
    }
}

void Configs::setConfig(const std::string &name,
                        const Config &config,
                        Configs::Order order)
{
    std::shared_ptr<ConfigList> configs = m_configs;
    std::shared_ptr<ConfigList> newConfigs(new ConfigList);
    bool inserted = false;
    for (const auto &wrap : *configs.get()) {
        if (!inserted && order < wrap.order) {
            newConfigs->push_back({name, config, order});
            inserted = true;
        } else if (name != wrap.name) {
            newConfigs->push_back(wrap);
        }
    }
    if (!inserted) {
        newConfigs->push_back({name, config, order});
    }
    m_configs = newConfigs;
}

void Configs::setConfig(const std::string &name, const Config &config)
{
    std::shared_ptr<ConfigList> configs = m_configs;
    std::shared_ptr<ConfigList> newConfigs(new ConfigList);
    bool inserted = false;
    for (const auto &wrap : *configs.get()) {
        if (name != wrap.name) {
            newConfigs->push_back(wrap);
        } else {
            newConfigs->push_back({name, config, wrap.order});
            inserted = true;
        }
    }
    if (!inserted) {
        newConfigs->push_back({name, config, INT_MAX});
    }
    m_configs = newConfigs;
}

bool Configs::invoke(std::shared_ptr<Handle> &handle, Error &error)
{
    std::shared_ptr<ConfigList> configs = m_configs;
    for (const auto &config : *configs.get()) {
        if (config.invoke && !config.invoke(handle, error)) {
            return false;
        }
    }
    return true;
}

Config Configs::getConfigByName(const std::string &name) const
{
    std::shared_ptr<ConfigList> configs = m_configs;
    for (const auto &config : *configs.get()) {
        if (config.name == name) {
            return config.invoke;
        }
    }
    return nullptr;
}

bool operator==(const Configs &left, const Configs &right)
{
    return left.m_configs == right.m_configs;
}

bool operator!=(const Configs &left, const Configs &right)
{
    return left.m_configs != right.m_configs;
}

} //namespace WCDB
