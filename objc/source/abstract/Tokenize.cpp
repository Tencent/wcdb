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

#include <WCDB/Abstract.h>
#include <WCDB/Error.hpp>

namespace WCDB {

namespace FTS {

Modules *Modules::shared()
{
    static Modules s_modules;
    return &s_modules;
}

void Modules::addAddress(const std::string &name, unsigned char *module)
{
    LockGuard lockGuard(m_lock);
    m_modules.insert({name, module});
}

unsigned char *Modules::getAddress(const std::string &name) const
{
    SharedLockGuard lockGuard(m_lock);
    auto iter = m_modules.find(name);
    WCTAssert(iter != m_modules.end(), "Tokenize name is not registered.");
    return iter->second;
}

TokenizerInfoBase::TokenizerInfoBase(int argc, const char *const *argv)
{
}

CursorInfoBase::CursorInfoBase(const char *input,
                               int inputLength,
                               TokenizerInfoBase *tokenizerInfo)
    : m_tokenizerInfo(tokenizerInfo)
{
}

} // namespace FTS

} //namespace WCDB
