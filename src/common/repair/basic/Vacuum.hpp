//
// Created by qiuwenchen on 2023/12/23.
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

#include "Progress.hpp"
#include "StringView.hpp"
#include "WCDBError.hpp"

namespace WCDB {

namespace Repair {

class VacuumDelegate : public Progress {
public:
    VacuumDelegate();
    virtual ~VacuumDelegate() = 0;

    void setOriginalDatabase(const UnsafeStringView &originalPath);
    void setVacuumDatabase(const UnsafeStringView &vacuumPath);

    virtual bool executeVacuum() = 0;
    virtual const Error &getVacuumError() = 0;

protected:
    StringView m_originalPath;
    StringView m_vacuumPath;
};

class VacuumDelegateHolder {
public:
    VacuumDelegateHolder();
    virtual ~VacuumDelegateHolder() = 0;
    void setVacuumDelegate(VacuumDelegate *delegate);

protected:
    VacuumDelegate *m_vacuumDelegate;
};

} //namespace Repair

} //namespace WCDB
