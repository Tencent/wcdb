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

#include "Vacuum.hpp"

namespace WCDB {

namespace Repair {

VacuumDelegate::VacuumDelegate() = default;

VacuumDelegate::~VacuumDelegate() = default;

void VacuumDelegate::setOriginalDatabase(const UnsafeStringView &originalPath)
{
    m_originalPath = originalPath;
}

void VacuumDelegate::setVacuumDatabase(const UnsafeStringView &vacuumPath)
{
    m_vacuumPath = vacuumPath;
}

VacuumDelegateHolder::VacuumDelegateHolder() : m_vacuumDelegate(nullptr)
{
}

void VacuumDelegateHolder::setVacuumDelegate(VacuumDelegate *delegate)
{
    m_vacuumDelegate = delegate;
}

VacuumDelegateHolder::~VacuumDelegateHolder() = default;

} //namespace Repair

} //namespace WCDB
