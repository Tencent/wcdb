//
// Created by sanhuazhang on 2019/05/28
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

#include "TokenizerConfig.hpp"
#include "Assertion.hpp"
#include "InnerHandle.hpp"
#include "SQLite.h"
#include "TokenizerModules.hpp"
#include <cstring>

namespace WCDB {

TokenizerConfig::TokenizerConfig(const UnsafeStringView& name_,
                                 const std::shared_ptr<TokenizerModules>& modules)
: Config()
, name(name_)
, m_modules(modules)
, m_fts3Statement(StatementSelect().select(
  Expression::function("fts3_tokenizer").invoke().arguments(BindParameter::bindParameters(2))))
, m_fts5Statement(StatementSelect().select(
  Expression::function("fts5").invoke().arguments(BindParameter::bindParameters(1))))
{
    size_t pos = name.find(" ");
    if (pos != UnsafeStringView::npos) {
        name = StringView(name.data(), pos);
    }
}

TokenizerConfig::~TokenizerConfig() = default;

bool TokenizerConfig::invoke(InnerHandle* handle)
{
    const TokenizerModule* module = m_modules->get(name);
    WCTRemedialAssert(module != nullptr, "Module does not exist.", return true;);
    if (module->getFts3Module() != nullptr) {
        auto exists = handle->ft3TokenizerExists(name);
        if (!exists.succeed()) {
            return false;
        }
        if (exists.value()) {
            return true;
        }
        bool succeed = false;
        if (handle->prepare(m_fts3Statement)) {
            handle->bindText(name, 1);
            FTS3TokenizerModule* fts3Module = module->getFts3Module().get();
            UnsafeData address((unsigned char*) &fts3Module, sizeof(unsigned char*));
            handle->bindBLOB(address, 2);
            succeed = handle->step();
            handle->finalize();
        }
        return succeed;
    } else {
        fts5_api* api = nullptr;
        if (handle->prepare(m_fts5Statement)) {
            const UnsafeStringView type = "fts5_api_ptr";
            handle->bindPointer((void*) &api, 1, type, nullptr);
            if (!handle->step() || api == nullptr) {
                handle->finalize();
                return false;
            }
            handle->finalize();
        }
        void* pCtx = nullptr;
        fts5_tokenizer* pTokenizer = new fts5_tokenizer();
        if (api->xFindTokenizer(api, name.data(), &pCtx, pTokenizer) == SQLITE_OK) {
            delete pTokenizer;
            return true;
        }
        delete pTokenizer;
        if (api->xCreateTokenizer(api,
                                  name.data(),
                                  module->getFts5Module()->getContext(),
                                  (fts5_tokenizer*) module->getFts5Module().get(),
                                  nullptr)
            != SQLITE_OK) {
            return false;
        }
        return true;
    }
}

} //namespace WCDB
