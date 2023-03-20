//
// Created by sanhuazhang on 2019/05/26
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

#include "AutoMigrateConfig.hpp"
#include "Assertion.hpp"
#include "InnerHandle.hpp"

namespace WCDB {

AutoMigrateOperator::~AutoMigrateOperator() = default;

AutoMigrateConfig::AutoMigrateConfig(const std::shared_ptr<AutoMigrateOperator>& operator_)
: Config(), m_operator(operator_)
{
    WCTAssert(m_operator != nullptr);
}

AutoMigrateConfig::~AutoMigrateConfig() = default;

bool AutoMigrateConfig::invoke(InnerHandle* handle)
{
    const UnsafeStringView& path = handle->getPath();
    if (++getOrCreateRegister(path) == 1) {
        m_operator->asyncMigrate(path);
    }
    return true;
}

bool AutoMigrateConfig::uninvoke(InnerHandle* handle)
{
    const UnsafeStringView& path = handle->getPath();
    if (--getOrCreateRegister(path) == 0) {
        m_operator->stopMigrate(path);
    }
    return true;
}

std::atomic<int>& AutoMigrateConfig::getOrCreateRegister(const UnsafeStringView& path)
{
    {
        SharedLockGuard lockGuard(m_lock);
        auto iter = m_registers.find(path);
        if (iter != m_registers.end()) {
            WCTAssert(iter->second.load() >= 0);
            return iter->second;
        }
    }
    {
        LockGuard lockGuard(m_lock);
        auto iter = m_registers.find(path);
        if (iter == m_registers.end()) {
            m_registers[path] = 0;
            iter = m_registers.find(path);
        }
        WCTAssert(iter->second.load() >= 0);
        return iter->second;
    }
}

} // namespace WCDB
