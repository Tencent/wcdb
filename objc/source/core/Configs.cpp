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

#pragma mark - Configs
std::shared_ptr<const Configs> Configs::default_()
{
    static std::shared_ptr<const Configs> *s_configs =
        new std::shared_ptr<const Configs>(new Configs({
            Element(Configs::trace(), "trace", Priority::Trace),
            Element(Configs::basic(), "basic", Priority::Basic),
            Element(Configs::checkpoint(), "checkpoint", Priority::Checkpoint),
        }));
    return *s_configs;
}

std::shared_ptr<const Configs>
Configs::configsBySettingConfig(const std::shared_ptr<Config> &config,
                                const std::string &name,
                                int priority) const
{
    WCTInnerAssert(config != nullptr);
    std::shared_ptr<Configs> configs(new Configs(m_elements));
    configs->setElement(Element(config, name, priority));
    return configs;
}

std::shared_ptr<const Configs>
Configs::configsBySettingConfig(const std::shared_ptr<Config> &config,
                                const std::string &name) const
{
    WCTInnerAssert(config != nullptr);
    std::shared_ptr<Configs> configs(new Configs(m_elements));
    configs->setElement(Element(config, name, 0));
    return configs;
}

std::shared_ptr<const Configs>
Configs::configsByRemovingConfig(const std::string &name) const
{
    std::shared_ptr<Configs> configs(new Configs(m_elements));
    configs->removeElement(name);
    return configs;
}

bool Configs::invoke(Handle *handle) const
{
    for (const auto &element : m_elements) {
        if (!element.config->invoke(handle)) {
            return false;
        }
    }
    return true;
}

bool Configs::equal(const std::shared_ptr<const Configs> &configs) const
{
    return this == configs.get();
}

#pragma mark - Element
Configs::Element::Element(const std::shared_ptr<Config> &config_,
                          const std::string &name_,
                          int priority_)
    : config(config_), name(name_), priority(priority_)
{
}

Configs::Configs(const std::list<Element> &elements)
{
    for (const auto &element : elements) {
        setElement(element);
    }
}

void Configs::setElement(const Element &element)
{
    std::list<Element>::iterator iter;
    for (iter = m_elements.begin(); iter != m_elements.end(); ++iter) {
        if (iter->name == element.name) {
            m_elements.erase(iter);
            break;
        }
    }
    for (iter = m_elements.begin(); iter != m_elements.end(); ++iter) {
        if (element.priority < iter->priority) {
            break;
        }
    }
    m_elements.insert(iter, element);
}

void Configs::removeElement(const std::string &name)
{
    for (auto iter = m_elements.begin(); iter != m_elements.end(); ++iter) {
        if (iter->name == name) {
            m_elements.erase(iter);
            return;
        }
    }
}

#pragma mark - Config
std::shared_ptr<Config> Configs::basic()
{
    static const std::shared_ptr<Config> *s_basic =
        new std::shared_ptr<Config>(new BasicConfig);
    return *s_basic;
}

std::shared_ptr<Config> Configs::trace()
{
    static const std::shared_ptr<Config> *s_trace =
        new std::shared_ptr<Config>(new TraceConfig);
    return *s_trace;
}

std::shared_ptr<Config> Configs::checkpoint()
{
    static const std::shared_ptr<Config> *s_checkpoint =
        new std::shared_ptr<Config>(new CheckpointConfig);
    return *s_checkpoint;
}

} //namespace WCDB
