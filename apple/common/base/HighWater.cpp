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

#include "HighWater.hpp"

namespace WCDB {

AbstractHighWater::AbstractHighWater(ssize_t init)
: m_current(init), m_highWater(init)
{
}

AbstractHighWater::~AbstractHighWater() = default;

void AbstractHighWater::increase(size_t size)
{
    m_current += size;
    if (m_current > m_highWater) {
        m_highWater = m_current;
    }
}

void AbstractHighWater::decrease(size_t size)
{
    m_current -= size;
}

ssize_t AbstractHighWater::getCurrent() const
{
    return m_current;
}

ssize_t AbstractHighWater::getHighWater() const
{
    return m_highWater;
}

ShareableHighWater::~ShareableHighWater() = default;

void ShareableHighWater::increase(size_t size)
{
    std::lock_guard<std::mutex> lockGuard(m_lock);
    AbstractHighWater::increase(size);
}

void ShareableHighWater::decrease(size_t size)
{
    std::lock_guard<std::mutex> lockGuard(m_lock);
    AbstractHighWater::decrease(size);
}

ssize_t ShareableHighWater::getCurrent() const
{
    std::lock_guard<std::mutex> lockGuard(m_lock);
    return AbstractHighWater::getCurrent();
}

ssize_t ShareableHighWater::getHighWater() const
{
    std::lock_guard<std::mutex> lockGuard(m_lock);
    return AbstractHighWater::getHighWater();
}

} // namespace WCDB
