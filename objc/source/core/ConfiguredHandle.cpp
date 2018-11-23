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
#include <WCDB/ConfiguredHandle.hpp>

namespace WCDB {

ConfiguredHandle::ConfiguredHandle(const std::shared_ptr<Handle> &handle)
: m_handle(handle), m_configs(nullptr)
{
}

bool ConfiguredHandle::reconfigure(const std::shared_ptr<Configs> &newConfigs)
{
    if (m_configs == newConfigs) {
        return true;
    }
    if (m_configs) {
        if (!m_configs->uninvoke(m_handle.get())) {
            return false;
        }
    }
    if (newConfigs) {
        if (!newConfigs->invoke(m_handle.get())) {
            return false;
        }
    }
    m_configs = newConfigs;
    return true;
}

Handle *ConfiguredHandle::get() const
{
    return m_handle.get();
}

} //namespace WCDB
