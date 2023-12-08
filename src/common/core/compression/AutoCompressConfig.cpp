//
// Created by qiuwenchen on 2023/12/2.
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

#include "AutoCompressConfig.hpp"
#include "Assertion.hpp"
#include "InnerHandle.hpp"

namespace WCDB {

AutoCompressOperator::~AutoCompressOperator() = default;

AutoCompressConfig::AutoCompressConfig(const std::shared_ptr<AutoCompressOperator>& operator_)
: Config(), m_operator(operator_)
{
    WCTAssert(m_operator != nullptr);
}

AutoCompressConfig::~AutoCompressConfig() = default;

bool AutoCompressConfig::invoke(InnerHandle* handle)
{
    const UnsafeStringView& path = handle->getPath();
    if (++getOrCreateRegister(path) == 1) {
        m_operator->asyncCompress(path);
    }
    return true;
}

bool AutoCompressConfig::uninvoke(InnerHandle* handle)
{
    const UnsafeStringView& path = handle->getPath();
    if (--getOrCreateRegister(path) == 0) {
        m_operator->stopCompress(path);
    }
    return true;
}

std::atomic<int>& AutoCompressConfig::getOrCreateRegister(const UnsafeStringView& path)
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
