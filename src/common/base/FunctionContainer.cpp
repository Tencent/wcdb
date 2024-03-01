//
// Created by qiuwenchen on 2023/11/15.
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

#include "FunctionContainer.hpp"
#include <stdlib.h>

namespace WCDB {

FunctionContainerBase::FunctionContainerBase()
: m_functionSlots(nullptr), m_funcDeconstructors(nullptr)
{
}

FunctionContainerBase::FunctionContainerBase(FunctionContainerBase&& other)
: m_functionSlots(other.m_functionSlots), m_funcDeconstructors(other.m_funcDeconstructors)
{
    other.m_functionSlots = nullptr;
    other.m_funcDeconstructors = nullptr;
}

FunctionContainerBase& FunctionContainerBase::operator=(FunctionContainerBase&& other)
{
    clearFunctionSlot();
    m_functionSlots = other.m_functionSlots;
    m_funcDeconstructors = other.m_funcDeconstructors;
    other.m_functionSlots = nullptr;
    other.m_funcDeconstructors = nullptr;
    return *this;
}

FunctionContainerBase::~FunctionContainerBase()
{
    clearFunctionSlot();
}

void FunctionContainerBase::initFunctionSlot(size_t slotCount)
{
    WCTAssert(m_functionSlots == nullptr);
    m_functionSlots = (Slot*) calloc(slotCount, sizeof(Slot) + sizeof(Destructor*));
    m_funcDeconstructors = (Destructor*) (m_functionSlots + slotCount);
}

size_t FunctionContainerBase::getSlotCount() const
{
    return ((int64_t) m_funcDeconstructors - (int64_t) m_functionSlots) / sizeof(Slot);
}

void FunctionContainerBase::clearFunctionSlot()
{
    if (m_functionSlots == nullptr) {
        return;
    }
    for (int i = 0; i < getSlotCount(); i++) {
        if (m_funcDeconstructors[i] == nullptr) {
            continue;
        }
        m_funcDeconstructors[i](m_functionSlots + i);
    }
    free(m_functionSlots);
    m_functionSlots = nullptr;
}

} // namespace WCDB
