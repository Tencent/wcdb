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

#ifndef ConfiguredHandle_hpp
#define ConfiguredHandle_hpp

#include <WCDB/Abstract.h>
#include <WCDB/Config.hpp>

#pragma GCC visibility push(hidden)

namespace WCDB {

class ConfiguredHandle {
public:
    ConfiguredHandle() = delete;
    ConfiguredHandle(const ConfiguredHandle &) = delete;
    ConfiguredHandle &operator=(const ConfiguredHandle &) = delete;

    static std::shared_ptr<ConfiguredHandle>
    configuredHandle(const std::shared_ptr<Handle> &handle);

    bool configured(const Configs &configs) const;
    bool configure(const Configs &configs);

    Handle *getHandle() const;

protected:
    ConfiguredHandle(const std::shared_ptr<Handle> &handle);
    std::shared_ptr<Handle> m_handle;
    Configs m_configs;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* ConfiguredHandle_hpp */
