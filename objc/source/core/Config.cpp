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

#include <WCDB/Core.h>

namespace WCDB {

Config::Config(const std::string &theName,
               const Callback &theCallback,
               int theOrder)
    : name(theName), callback(theCallback), order(theOrder)
{
}

bool Config::invoke(Handle *handle) const
{
    return callback ? callback(handle) : true;
}

Configs::Configs()
{
}

Configs::Configs(const std::list<Config> &configs)
{
    for (const Config &config : configs) {
        setConfig(config);
    }
}

bool Configs::invoke(Handle *handle) const
{
    for (const auto &element : m_configs.get()) {
        if (!element.get().invoke(handle)) {
            return false;
        }
    }
    return true;
}

void Configs::setConfig(const Config &config)
{
    auto &configs = m_configs.get_or_copy();
    for (auto iter = configs.begin(); iter != configs.end(); ++iter) {
        if (iter->get().name == config.name) {
            configs.erase(iter);
            break;
        }
    }
    std::list<CopyOnWrite<Config>>::iterator iter;
    for (iter = configs.begin(); iter != configs.end(); ++iter) {
        if (config.order < iter->get().order) {
            break;
        }
    }
    if (config.callback != nullptr) {
        configs.insert(iter, config);
    }
}

bool Configs::equal(const Configs &other) const
{
    return m_configs.equal(other.m_configs);
}

} //namespace WCDB
