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

#include <WCDB/Configs.hpp>

namespace WCDB {

std::shared_ptr<const Configs> Configs::default_()
{
    static std::shared_ptr<const Configs> s_configs(
        new Configs({TraceConfig::config(), BasicConfig::config(),
                     CheckpointConfig::config()}));
    return s_configs;
}

std::shared_ptr<const Configs>
Configs::configs(const std::list<std::shared_ptr<Config>> &configs)
{
    return std::shared_ptr<const Configs>(new Configs(configs));
}

Configs::Configs(const std::list<std::shared_ptr<Config>> &configs)
{
    for (const auto &config : configs) {
        setConfig(config);
    }
}

std::shared_ptr<const Configs>
Configs::configsBySettingConfig(const std::shared_ptr<Config> &newConfig) const
{
    std::shared_ptr<Configs> configs(new Configs(m_configs));
    configs->setConfig(newConfig);
    return configs;
}

std::shared_ptr<const Configs>
Configs::configsByRemovingConfig(const std::string &name) const
{
    std::shared_ptr<Configs> configs(new Configs(m_configs));
    configs->removeConfig(name);
    return configs;
}

bool Configs::invoke(Handle *handle) const
{
    for (const auto &config : m_configs) {
        if (!config->invoke(handle)) {
            return false;
        }
    }
    return true;
}

void Configs::setConfig(const std::shared_ptr<Config> &config)
{
    WCTInnerAssert(config != nullptr);
    std::list<std::shared_ptr<Config>>::iterator iter;
    for (iter = m_configs.begin(); iter != m_configs.end(); ++iter) {
        if (iter->get()->name == config->name) {
            m_configs.erase(iter);
            break;
        }
    }
    for (iter = m_configs.begin(); iter != m_configs.end(); ++iter) {
        if (config->order < iter->get()->order) {
            break;
        }
    }
    m_configs.insert(iter, config);
}

void Configs::removeConfig(const std::string &name)
{
    for (auto iter = m_configs.begin(); iter != m_configs.end(); ++iter) {
        if (iter->get()->name == name) {
            m_configs.erase(iter);
            return;
        }
    }
}

bool Configs::equal(const std::shared_ptr<const Configs> &configs) const
{
    return this == configs.get();
}

} //namespace WCDB
