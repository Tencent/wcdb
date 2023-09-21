//
// Created by sanhuazhang on 2019/08/27
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

#include "Assemble.hpp"
#include "Assertion.hpp"
#include "Cell.hpp"

namespace WCDB {

namespace Repair {

AssembleDelegate::AssembleDelegate()
: m_duplicatedIgnorable(false), m_duplicatedReplaceable(false)
{
}

AssembleDelegate::~AssembleDelegate() = default;

void AssembleDelegate::markDuplicatedAsIgnorable(bool ignorable)
{
    m_duplicatedIgnorable = ignorable;
}

void AssembleDelegate::markDuplicatedAsReplaceable(bool replaceable)
{
    m_duplicatedReplaceable = replaceable;
}

bool AssembleDelegate::isDuplicatedIgnorable() const
{
    return m_duplicatedIgnorable;
}

bool AssembleDelegate::isDuplicatedReplaceable() const
{
    return m_duplicatedReplaceable;
}

AssembleDelegateHolder::AssembleDelegateHolder() : m_assembleDelegate(nullptr)
{
}

void AssembleDelegateHolder::setAssembleDelegate(AssembleDelegate* delegate)
{
    m_assembleDelegate = delegate;
}

AssembleDelegateHolder::~AssembleDelegateHolder() = default;

} //namespace Repair

} //namespace WCDB
