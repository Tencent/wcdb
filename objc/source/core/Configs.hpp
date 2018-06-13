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

#include <WCDB/BasicConfig.hpp>
#include <WCDB/CheckpointConfig.hpp>
#include <WCDB/CipherConfig.hpp>
#include <WCDB/Config.hpp>
#include <WCDB/TokenizeConfig.hpp>
#include <WCDB/TraceConfig.hpp>

#pragma GCC visibility push(hidden)

namespace WCDB {

class Configs {
#pragma mark - Configs
public:
    static std::shared_ptr<const Configs> default_();

    std::shared_ptr<const Configs>
    configsBySettingConfig(const std::shared_ptr<Config> &config,
                           const std::string &name,
                           int priority) const;
    std::shared_ptr<const Configs>
    configsBySettingConfig(const std::shared_ptr<Config> &config,
                           const std::string &name) const;
    std::shared_ptr<const Configs>
    configsByRemovingConfig(const std::string &names) const;

    bool invoke(Handle *handle) const;

    bool equal(const std::shared_ptr<const Configs> &configs) const;

#pragma mark - Element
protected:
    struct Element {
        Element(const std::shared_ptr<Config> &config,
                const std::string &name,
                int priority);
        std::shared_ptr<Config> config;
        std::string name;
        int priority;
    };
    typedef struct Element Element;
    Configs(const std::list<Element> &elements);

    void setElement(const Element &element);
    void removeElement(const std::string &name);

    std::list<Element> m_elements;

#pragma mark - Config
public:
    enum Priority {
        //TODO
        Trace,
        Cipher,
        Basic,
        Tokenize,
        Checkpoint,
        End,
    };

    static std::shared_ptr<Config> basic();
    static std::shared_ptr<Config> trace();
    static std::shared_ptr<Config> checkpoint();
};

#pragma GCC visibility pop

} //namespace WCDB

#endif /* Configs_hpp */
