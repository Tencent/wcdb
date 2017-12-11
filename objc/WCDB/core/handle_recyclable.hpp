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

#ifndef handle_recyclable_hpp
#define handle_recyclable_hpp

#include <WCDB/abstract.h>
#include <WCDB/config.hpp>
#include <WCDB/recyclable.hpp>
#include <memory>

namespace WCDB {

class HandleWrap {
public:
    HandleWrap(const std::shared_ptr<Handle> &handle, const Configs &configs);

    constexpr Handle *operator->() const { return handle.get(); }

    std::shared_ptr<Handle> handle;
    Configs configs;
};

class RecyclableHandle {
public:
    RecyclableHandle(
        const std::shared_ptr<HandleWrap> &value,
        const Recyclable<std::shared_ptr<HandleWrap>>::OnRecycled &onRecycled);
    constexpr Handle *operator->() const { return m_value->operator->(); }
    operator bool() const;
    bool operator!=(const std::nullptr_t &) const;
    bool operator==(const std::nullptr_t &) const;

    RecyclableHandle &operator=(const std::nullptr_t &);

protected:
    std::shared_ptr<HandleWrap> m_value;
    Recyclable<std::shared_ptr<HandleWrap>> m_recyclable;
};

} //namespace WCDB

#endif /* handle_recyclable_hpp */
