//
// Created by qiuwenchen on 2023/12/15.
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

#include "Assemble.hpp"
#include "Backup.hpp"
#include "ErrorProne.hpp"
#include "FactoryRelated.hpp"
#include "Progress.hpp"

namespace WCDB {

namespace Repair {

class FactoryVacuum final : public FactoryRelated,
                            public ErrorProne,
                            public Progress,
                            public CipherDelegateHolder,
                            public AssembleDelegateHolder {
public:
    FactoryVacuum(const Factory &factory);
    ~FactoryVacuum() override final;

    const StringView directory;
    const StringView database;

    bool work();
    bool prepare(int64_t totalPageSize);

protected:
    bool increaseProgress(double progress, double increment);
    bool exit(bool result);
};

} // namespace Repair

} // namespace WCDB
