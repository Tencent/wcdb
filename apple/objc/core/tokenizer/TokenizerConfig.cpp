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
#include <WCDB/Handle.hpp>
#include <WCDB/TokenizerConfig.hpp>
#include <WCDB/TokenizerModules.hpp>

namespace WCDB {

TokenizerConfig::TokenizerConfig(const UnsafeStringView& name_,
                                 const std::shared_ptr<TokenizerModules>& modules)
: Config()
, name(name_)
, m_modules(modules)
, m_statement(StatementSelect().select(
  Expression::function("fts3_tokenizer").invoke().arguments(BindParameter::bindParameters(2))))
{
}

TokenizerConfig::~TokenizerConfig() = default;

bool TokenizerConfig::invoke(Handle* handle)
{
    const TokenizerModule* module = m_modules->get(name);
    WCTRemedialAssert(module != nullptr, "Module does not exist.", return true;);
    auto exists = handle->ft3TokenizerExists(name);
    if (!exists.has_value()) {
        return false;
    }
    if (exists.value()) {
        return true;
    }
    bool succeed = false;
    if (handle->prepare(m_statement)) {
        handle->bindText(name, 1);
        UnsafeData address((unsigned char*) &module, sizeof(unsigned char*));
        handle->bindBLOB(address, 2);
        succeed = handle->step();
        handle->finalize();
    }
    return succeed;
}

} //namespace WCDB
