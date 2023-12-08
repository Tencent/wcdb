//
// Created by qiuwenchen on 2021/10/17.
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

#include "AuxiliaryFunctionConfig.hpp"
#include "Assertion.hpp"
#include "InnerHandle.hpp"
#include "SQLite.h"

namespace WCDB {

AuxiliaryFunctionConfig::AuxiliaryFunctionConfig(const UnsafeStringView& name_,
                                                 const std::shared_ptr<AuxiliaryFunctionModules>& modules)
: Config()
, name(name_)
, m_modules(modules)
, m_Statement(StatementSelect().select(
  Expression::function("fts5").invoke().arguments(BindParameter::bindParameters(1))))
{
}

AuxiliaryFunctionConfig::~AuxiliaryFunctionConfig() = default;

bool AuxiliaryFunctionConfig::invoke(InnerHandle* handle)
{
    const FTS5AuxiliaryFunctionModule* module = m_modules->get(name);
    WCTRemedialAssert(module != nullptr, "Module does not exist.", return true;);
    fts5_api* api = nullptr;
    if (handle->prepare(m_Statement)) {
        const UnsafeStringView type = "fts5_api_ptr";
        handle->bindPointer((void*) &api, 1, type, nullptr);
        if (!handle->step() || api == nullptr) {
            handle->finalize();
            return false;
        }
        handle->finalize();
    }
    int rc = api->xCreateFunction(
    api,
    name.data(),
    module->getContext(),
    (void (*)(const Fts5ExtensionApi*, Fts5Context*, sqlite3_context*, int, sqlite3_value**))
    module->getFunc(),
    nullptr);
    if (rc != SQLITE_OK) {
        handle->notifyError(rc, "create fts5 auxiliary function");
        return false;
    }
    return true;
}

} //namespace WCDB
