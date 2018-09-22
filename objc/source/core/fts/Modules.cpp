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

#include <WCDB/Assertion.hpp>
#include <WCDB/Error.hpp>
#include <WCDB/Modules.hpp>

namespace WCDB {

namespace FTS {

void Modules::addAddress(const std::string &name, unsigned char *address)
{
    LockGuard lockGuard(m_lock);
    m_addresses[name] = UnsafeData(address, sizeof(unsigned char *));
}

const UnsafeData &Modules::getAddress(const std::string &name) const
{
    SharedLockGuard lockGuard(m_lock);
    auto iter = m_addresses.find(name);
    WCTRemedialAssert(iter != m_addresses.end(),
                      "Tokenize name is not registered.",
                      return UnsafeData::emptyData(););
    return iter->second;
}

} // namespace FTS

} // namespace WCDB
