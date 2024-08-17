//
// Created by qiuwenchen on 2024/8/15.
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

#include "AutoVacuumConfig.hpp"

namespace WCDB {

AutoVacuumConfig::AutoVacuumConfig(bool incremental)
: m_mode(incremental ? 2 : 1)
, m_getAutoVacuumMode(StatementPragma().pragma(Pragma::autoVacuum()))
, m_setAutoVacuumMode(StatementPragma().pragma(Pragma::autoVacuum()).to(m_mode))
{
}

AutoVacuumConfig::~AutoVacuumConfig() = default;

bool AutoVacuumConfig::invoke(InnerHandle *handle)
{
    if (!handle->prepare(m_getAutoVacuumMode)) {
        return false;
    }
    if (!handle->step()) {
        handle->finalize();
        return false;
    }
    int currentVacuumMode = (int) handle->getInteger();
    handle->finalize();
    if (currentVacuumMode == m_mode) {
        return true;
    }
    if (!handle->canWriteMainDB()) {
        handle->enableWriteMainDB(true);
        handle->close();
        if (!handle->open()) {
            handle->enableWriteMainDB(false);
            return false;
        }
        handle->enableWriteMainDB(false);
    }
    return handle->execute(m_setAutoVacuumMode);
}

} // namespace WCDB
