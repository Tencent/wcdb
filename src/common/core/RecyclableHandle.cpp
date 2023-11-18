//
// Created by sanhuazhang on 2019/05/02
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

#include "RecyclableHandle.hpp"
#include "Assertion.hpp"
#include "DecorativeHandle.hpp"

namespace WCDB {

RecyclableHandle::RecyclableHandle() : Super(), m_handle(nullptr)
{
}

RecyclableHandle::RecyclableHandle(const std::nullptr_t &)
: Super(nullptr), m_handle(nullptr)
{
}

RecyclableHandle::RecyclableHandle(const std::shared_ptr<InnerHandle> &value,
                                   const Super::OnRecycled &onRecycled)
: Super(value, onRecycled), m_handle(value.get())
{
}

RecyclableHandle::~RecyclableHandle() = default;

RecyclableHandle &RecyclableHandle::operator=(const std::nullptr_t &)
{
    Super::operator=(nullptr);
    m_handle = nullptr;
    return *this;
}

InnerHandle *RecyclableHandle::operator->() const
{
    return m_handle;
}

RecyclableHandle &RecyclableHandle::operator=(const RecyclableHandle &other)
{
    Super::operator=(other);
    m_handle = other.m_handle;
    return *this;
}

InnerHandle *RecyclableHandle::get() const
{
    return m_handle;
}

DecorativeHandle *RecyclableHandle::getDecorative() const
{
    WCTAssert(dynamic_cast<DecorativeHandle *>(m_handle) != nullptr);
    return static_cast<DecorativeHandle *>(m_handle);
}

} //namespace WCDB
