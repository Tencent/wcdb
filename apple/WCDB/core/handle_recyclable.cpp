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

#include <WCDB/handle_recyclable.hpp>

namespace WCDB {

HandleWrap::HandleWrap(const std::shared_ptr<Handle> &theSqlBase,
                       const Configs &theConfigs)
    : handle(theSqlBase), configs(theConfigs)
{
}

RecyclableHandle::RecyclableHandle(
    const std::shared_ptr<HandleWrap> &value,
    const Recyclable<std::shared_ptr<HandleWrap>>::OnRecycled &onRecycled)
    : m_value(value), m_recyclable(value, onRecycled)
{
}

RecyclableHandle::operator bool() const
{
    return m_value != nullptr;
}

bool RecyclableHandle::operator!=(const std::nullptr_t &) const
{
    return m_value != nullptr;
}

bool RecyclableHandle::operator==(const std::nullptr_t &) const
{
    return m_value == nullptr;
}

RecyclableHandle &RecyclableHandle::operator=(const std::nullptr_t &)
{
    m_value = nullptr;
    m_recyclable = nullptr;
    return *this;
}

} //namespace WCDB
