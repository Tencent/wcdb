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

#pragma once

#include "AuxiliaryFunctionModule.hpp"
#include "Config.hpp"
#include "FunctionModules.hpp"
#include "WINQ.h"

namespace WCDB {

typedef FunctionModules<FTS5AuxiliaryFunctionModule> AuxiliaryFunctionModules;

class AuxiliaryFunctionConfig final : public Config {
public:
    AuxiliaryFunctionConfig(const UnsafeStringView& name,
                            const std::shared_ptr<AuxiliaryFunctionModules>& modules);
    ~AuxiliaryFunctionConfig() override;

    StringView name;

    bool invoke(InnerHandle* handle) override final;

protected:
    std::shared_ptr<AuxiliaryFunctionModules> m_modules;
    StatementSelect m_Statement;
};

} //namespace WCDB
