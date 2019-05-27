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
#include <WCDB/Configs.hpp>
#include <WCDB/Handle.hpp>

namespace WCDB {

void Configs::insert(const String &name, const std::shared_ptr<Config> &config, int priority)
{
    WCTRemedialAssert(config != nullptr, "Empty config make not sense.", return;);
    m_list.insert(priority, name, config);
}

void Configs::remove(const String &name)
{
    m_list.erase(name);
}

bool Configs::invoke(Handle *handle)
{
    for (const auto &element : m_list.elements()) {
        if (element.value != nullptr && !element.value->invoke(handle)) {
            return false;
        }
    }
    return true;
}

bool Configs::uninvoke(Handle *handle, bool stopIfFailed)
{
    for (const auto &element : m_list.elements()) {
        if (element.value != nullptr && !element.value->uninvoke(handle) && stopIfFailed) {
            return false;
        }
    }
    return true;
}

Configs::Configs(OrderedUniqueList<String, std::shared_ptr<Config>> &&list)
: m_list(std::move(list))
{
}

} //namespace WCDB
