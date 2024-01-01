//
// Created by sanhuazhang on 2018/08/15
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

#include "CrossPlatform.h"
#include "Lock.hpp"

namespace WCDB {

class AbstractHighWater {
public:
    AbstractHighWater(ssize_t init = 0);
    virtual ~AbstractHighWater() = 0;

    virtual void increase(size_t size);
    virtual void decrease(size_t size);
    virtual ssize_t getCurrent() const;
    virtual ssize_t getHighWater() const;

protected:
    ssize_t m_current;
    ssize_t m_highWater;
};

class HighWater final : public AbstractHighWater {
public:
    using AbstractHighWater::AbstractHighWater;
    ~HighWater() override;
};

class ShareableHighWater final : public AbstractHighWater {
public:
    using AbstractHighWater::AbstractHighWater;
    ~ShareableHighWater() override;

    void increase(size_t size) override final;
    void decrease(size_t size) override final;
    ssize_t getCurrent() const override final;
    ssize_t getHighWater() const override final;

protected:
    mutable std::mutex m_lock;
};

typedef std::shared_ptr<ShareableHighWater> SharedHighWater;

} // namespace WCDB
