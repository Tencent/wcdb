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

#ifndef fts_modules_hpp
#define fts_modules_hpp

#include <WCDB/spin.hpp>
#include <string>
#include <unordered_map>

namespace WCDB {

namespace FTS {

class Modules {
public:
    static Modules *SharedModules();

    void addModule(const std::string &name,
                   const std::shared_ptr<void> &module);
    const void *getAddress(const std::string &name) const;

protected:
    std::unordered_map<std::string, std::shared_ptr<void>> m_modules;
    mutable Spin m_spin;
};

} //namespace FTS

} //namespace WCDB

#endif /* fts_modules_hpp */
