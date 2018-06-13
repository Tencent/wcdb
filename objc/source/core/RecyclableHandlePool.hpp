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

#ifndef RecyclableHandlePool_hpp
#define RecyclableHandlePool_hpp

#include <WCDB/HandlePool.hpp>
#include <WCDB/Recyclable.hpp>

#pragma GCC visibility push(hidden)

namespace WCDB {

class RecyclableHandlePool : public Recyclable<std::shared_ptr<HandlePool>> {
public:
    using Super = Recyclable<std::shared_ptr<HandlePool>>;

    RecyclableHandlePool(const std::shared_ptr<HandlePool> &value,
                         const Super::OnRecycled &onRecycled);
    RecyclableHandlePool(const std::nullptr_t &);

    bool operator==(const std::nullptr_t &) const;
    bool operator!=(const std::nullptr_t &) const;

    constexpr HandlePool *operator->() const { return m_value.get(); }
    HandlePool *getHandlePool() const;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* RecyclableHandlePool_hpp */
