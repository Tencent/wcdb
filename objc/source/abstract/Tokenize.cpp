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

#include <WCDB/Tokenize.hpp>
#include <WCDB/error.hpp>

namespace WCDB {

namespace FTS {

Modules *Modules::SharedModules()
{
    static Modules s_modules;
    return &s_modules;
}

void Modules::addModule(const std::string &name,
                        const std::shared_ptr<void> &module)
{
    SpinLockGuard<Spin> lockGuard(m_spin);
    m_modules.insert({name, module});
}

std::vector<unsigned char> Modules::getAddress(const std::string &name) const
{
    SpinLockGuard<Spin> lockGuard(m_spin);
    auto iter = m_modules.find(name);
    if (iter == m_modules.end()) {
        WCDB::Error::Abort("Tokenize name is not registered");
    }
    const void *module = iter->second.get();
    unsigned char *address = (unsigned char *) &module;
    return std::vector<unsigned char>(address,
                                      address + sizeof(unsigned char *));
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
