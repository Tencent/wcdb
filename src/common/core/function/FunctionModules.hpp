//
// Created by 陈秋文 on 2023/11/25.
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

#pragma once

#include "Assertion.hpp"
#include "Lock.hpp"
#include "StringView.hpp"
#include "WCDBError.hpp"
#include <list>

namespace WCDB {

template<typename FunctionModule>
class FunctionModules final {
public:
    void add(const UnsafeStringView& name, const FunctionModule& module)
    {
        WCTRemedialAssert(!name.empty(), "Name of module can't be null.", return;);
        LockGuard lockGuard(m_lock);
        WCTRemedialAssert(
        m_modules.find(name) == m_modules.end(), "Module already exists.", return;);
        m_modules.emplace(name, module);
    }

    const FunctionModule* get(const UnsafeStringView& name) const
    {
        SharedLockGuard lockGuard(m_lock);
        const FunctionModule* module = nullptr;
        const auto iter = m_modules.find(name);
        if (iter != m_modules.end()) {
            module = &(iter->second);
        }
        return module;
    }

protected:
    StringViewMap<const FunctionModule> m_modules;
    mutable SharedLock m_lock;
};

} // namespace WCDB
