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

#pragma once

#include "Recyclable.hpp"

namespace WCDB {

class InnerHandle;
class DecorativeHandle;

class WCDB_API RecyclableHandle final : public Recyclable<std::shared_ptr<InnerHandle>> {
public:
    using Super = Recyclable<std::shared_ptr<InnerHandle>>;

    RecyclableHandle();
    RecyclableHandle(const std::nullptr_t &);
    RecyclableHandle(const std::shared_ptr<InnerHandle> &value,
                     const Super::OnRecycled &onRecycled);
    ~RecyclableHandle() override;

    RecyclableHandle &operator=(const std::nullptr_t &);
    RecyclableHandle &operator=(const RecyclableHandle &other);

    InnerHandle *operator->() const;
    InnerHandle *get() const;
    DecorativeHandle *getDecorative() const;

protected:
    InnerHandle *m_handle;
};

} //namespace WCDB
