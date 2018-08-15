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

#ifndef HighWater_hpp
#define HighWater_hpp

#include <WCDB/Lock.hpp>
#include <unistd.h>

namespace WCDB {

class HighWater {
public:
    HighWater(ssize_t init = 0);

    void increase(size_t size);
    void decrease(size_t size);
    ssize_t getCurrent() const;
    ssize_t getHighWater() const;

protected:
    ssize_t m_current;
    ssize_t m_highWater;
};

class ShareableHighWater : public HighWater {
public:
    using HighWater::HighWater;

    void increase(size_t size);
    void decrease(size_t size);
    ssize_t getCurrent() const;
    ssize_t getHighWater() const;

protected:
    mutable SpinLock m_lock;
};

} // namespace WCDB

#endif /* HighWater_hpp */
