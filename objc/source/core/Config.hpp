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

#include <WCDB/abstract.h>
#include <WCDB/copy_on_write_list.hpp>
#include <WCDB/error.hpp>
#include <WCDB/utility.hpp>
#include <functional>
#include <list>
#include <memory>

namespace WCDB {

class Config {
public:
    using Callback = std::function<bool(std::shared_ptr<Handle> &, Error &)>;

    Config(const std::string &theName,
           const Callback &theCallback,
           const int &theOrder); //Small numbers in front

    bool invoke(std::shared_ptr<Handle> &handle, Error &error) const;

    const std::string name;
    const Callback callback;
    const int order;
};

class Configs {
public:
    Configs(const std::list<Config> &configs);

    void setConfig(const Config &config);
    void setConfig(const std::string &name, const Config::Callback &callback);

    bool invoke(std::shared_ptr<Handle> &handle, Error &error) const;

    bool operator!=(const Configs &other) const;

public:
    copy_on_write_list<Config> m_configs;
};

} //namespace WCDB

#endif /* Config_hpp */
