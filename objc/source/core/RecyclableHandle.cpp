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

#include <WCDB/Assertion.hpp>
#include <WCDB/Core.h>

namespace WCDB {

RecyclableHandle::RecyclableHandle(const std::shared_ptr<ConfiguredHandle> &value,
                                   const Super::OnRecycled &onRecycled)
: Super(value, onRecycled), m_handle(value->getHandle())
{
}

RecyclableHandle::RecyclableHandle(const std::nullptr_t &)
: Super(nullptr, nullptr), m_handle(nullptr)
{
}

RecyclableHandle::RecyclableHandle()
: Super(nullptr, nullptr), m_handle(nullptr)
{
}

Handle *RecyclableHandle::getHandle() const
{
    return m_handle;
}

bool RecyclableHandle::operator==(const std::nullptr_t &) const
{
    return m_value == nullptr;
}

bool RecyclableHandle::operator!=(const std::nullptr_t &) const
{
    return m_value != nullptr;
}

} //namespace WCDB
