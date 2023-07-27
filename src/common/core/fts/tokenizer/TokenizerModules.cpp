//
// Created by sanhuazhang on 2019/05/02
//

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

#include "TokenizerModules.hpp"
#include "Assertion.hpp"
#include "SQLiteFTS3Tokenizer.h"
#include "WCDBError.hpp"

namespace WCDB {

void TokenizerModules::add(const UnsafeStringView& name, const TokenizerModule& module)
{
    WCTRemedialAssert(!name.empty(), "Name of module can't be null.", return;);
    LockGuard lockGuard(m_lock);
    WCTRemedialAssert(
    m_modules.find(name) == m_modules.end(), "Module already exists.", return;);
    auto iter = m_modules.emplace(name, module).first;
    WCTAssert(m_pointers.find(name) == m_pointers.end());
    m_pointers.emplace(name, &iter->second);
}

const TokenizerModule* TokenizerModules::get(const UnsafeStringView& name) const
{
    SharedLockGuard lockGuard(m_lock);
    const TokenizerModule* module = nullptr;
    const auto iter = m_pointers.find(name);
    if (iter != m_pointers.end()) {
        module = iter->second;
    }
    return module;
}

} // namespace WCDB
