//
// Created by qiuwenchen on 2023/11/25.
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

#include "ScalarFunctionConfig.hpp"
#include "Assertion.hpp"
#include "InnerHandle.hpp"
#include "Notifier.hpp"
#include "SQLite.h"

namespace WCDB {

ScalarFunctionConfig::ScalarFunctionConfig(const UnsafeStringView& name_,
                                           const std::shared_ptr<ScalarFunctionModules>& modules)
: Config(), name(name_), m_modules(modules)
{
}

ScalarFunctionConfig::~ScalarFunctionConfig() = default;

bool ScalarFunctionConfig::invoke(InnerHandle* handle)
{
    const ScalarFunctionModule* module = m_modules->get(name);
    WCTRemedialAssert(module != nullptr, "Module does not exist.", return true;);
    int rc = sqlite3_create_function(
    handle->getRawHandle(),
    name.data(),
    module->getParameterNum(),
    module->isDeterminisic() ? SQLITE_DETERMINISTIC | SQLITE_UTF8 : SQLITE_UTF8,
    module->getUserCtx(),
    (void (*)(sqlite3_context*, int, sqlite3_value**)) module->getFunc(),
    nullptr,
    nullptr);
    if (rc != SQLITE_OK) {
        handle->notifyError(rc, "create scalar function");
        return false;
    }
    return true;
}

} //namespace WCDB
