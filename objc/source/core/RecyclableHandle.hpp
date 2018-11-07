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

#ifndef _WCDB_RECYCLABLEHANDLE_HPP
#define _WCDB_RECYCLABLEHANDLE_HPP

#include <WCDB/ConfiguredHandle.hpp>
#include <WCDB/Recyclable.hpp>

namespace WCDB {

class RecyclableHandle final : public Recyclable<std::shared_ptr<ConfiguredHandle>> {
public:
    using Super = Recyclable<std::shared_ptr<ConfiguredHandle>>;

    RecyclableHandle(const std::shared_ptr<ConfiguredHandle> &value,
                     const Super::OnRecycled &onRecycled);
    RecyclableHandle(const std::nullptr_t &);
    RecyclableHandle();

    constexpr Handle *operator->() const { return m_handle; }
    Handle *get() const;

    bool operator==(const std::nullptr_t &) const;
    bool operator!=(const std::nullptr_t &) const;

protected:
    using Recyclable::get;
    using Recyclable::unsafeGet;

    Handle *m_handle;
};

} //namespace WCDB

#endif /* _WCDB_RECYCLABLEHANDLE_HPP */
