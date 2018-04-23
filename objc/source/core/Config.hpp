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

#ifndef Config_hpp
#define Config_hpp

#include <WCDB/Abstract.h>
#include <WCDB/CopyOnWriteList.hpp>
#include <WCDB/Error.hpp>
#include <WCDB/String.hpp>
#include <functional>
#include <list>
#include <memory>

#pragma GCC visibility push(hidden)

namespace WCDB {

class Config {
public:
    using Callback = std::function<bool(Handle *)>;

    Config(const std::string &theName,
           const Callback &theCallback,
           int theOrder); //Small numbers in front

    bool invoke(Handle *handle) const;

    const std::string name;
    const Callback callback;
    const int order;
};

class Configs {
public:
    Configs();
    Configs(const std::list<Config> &configs);

    void setConfig(const Config &config);
    void setConfig(const std::string &name, const Config::Callback &callback);

    bool invoke(Handle *handle) const;

    bool equal(const Configs &other) const;

public:
    CopyOnWriteList<Config> m_configs;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* Config_hpp */
