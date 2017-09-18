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

#ifndef config_hpp
#define config_hpp

#include <WCDB/abstract.h>
#include <WCDB/error.hpp>
#include <WCDB/rwlock.hpp>
#include <WCDB/utility.hpp>
#include <functional>
#include <list>
#include <memory>

namespace WCDB {

typedef std::function<bool(std::shared_ptr<Handle> &, Error &)> Config;
typedef struct ConfigWrap ConfigWrap;

/*
 * [Configs] is a copy-on-write class.
 * Different [Configs]s with same configs share a [ConfigList].
 * When a write op acts, the new config will not be added to original [ConfigList].
 * Instead, a new [ConfigList] will be generated which combines the original [ConfigList] and the new config.
 *
 * Let code talks:
 *  
 *  if
 *      Configs c1, c2;
 *      c2 = c1;
 *  then
 *      &c1!=&c2
 *      c1.m_configs==c2.m_configs
 *      *c1.m_configs.get()==*c2.m_configs.get()
 *      c1==c2
 *
 *  if
 *      c1.setConfig("newConfig", ...);
 *  then
 *      &c1!=&c2
 *      c1.m_configs!=c2.m_configs//The pointer is changed
 *      *c1.m_configs.get()!=*c2.m_configs.get()
 *      c1!=c2
 */

class Configs {
public:
    typedef int Order; //Small numbers in front
    void setConfig(const std::string &name,
                   const Config &config,
                   Configs::Order order);
    void setConfig(const std::string &name, const Config &config);

    bool invoke(std::shared_ptr<Handle> &handle, Error &error);

    friend bool operator==(const Configs &left, const Configs &right);
    friend bool operator!=(const Configs &left, const Configs &right);

    Configs();
    Configs(std::initializer_list<const ConfigWrap> configs);

    Config getConfigByName(const std::string &name) const;

protected:
    typedef std::list<ConfigWrap> ConfigList;

    std::shared_ptr<ConfigList> m_configs; //copy-on-write
};

struct ConfigWrap {
    const std::string name;
    const Config invoke;
    const Configs::Order order;
    ConfigWrap(const std::string &theName,
               const Config &theConfig,
               Configs::Order theOrder)
        : name(theName), invoke(theConfig), order(theOrder)
    {
    }
};

} //namespace WCDB

#endif /* config_hpp */
