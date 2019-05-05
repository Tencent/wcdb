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

#ifndef __WCDB_TOKENIZER_MODULES_HPP
#define __WCDB_TOKENIZER_MODULES_HPP

#include <WCDB/Lock.hpp>
#include <WCDB/String.hpp>
#include <WCDB/TokenizerModule.hpp>
#include <WCDB/TokenizerModuleTemplate.hpp>
#include <WCDB/UnsafeData.hpp>
#include <list>

namespace WCDB {

class TokenizerModules final {
public:
    void add(const String& name, const TokenizerModule& module);
    std::pair<bool, TokenizerModule> get(const String& name) const;

protected:
    std::map<String, TokenizerModule> m_modules;
    mutable SharedLock m_lock;
};

} // namespace WCDB

#endif /* __WCDB_TOKENIZER_MODULES_HPP */
