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

#ifndef Time_hpp
#define Time_hpp

#include <WCDB/SharedThreadedErrorProne.hpp>
#include <WCDB/String.hpp>
#include <chrono>

namespace WCDB {

class Time final : public std::chrono::system_clock::time_point, public SharedThreadedErrorProne {
public:
    using Super = std::chrono::system_clock::time_point;
    using Super::time_point;
    Time(const Super &super);
    Time(Super &&super);
    Time(const struct timespec &ts);

    static Time now();

    std::time_t seconds() const;
    std::time_t nanoseconds() const;
    bool empty() const;
    String stringify() const;
};

class SteadyClock final : public std::chrono::steady_clock::time_point {
public:
    using Super = std::chrono::steady_clock::time_point;
    using Super::time_point;
    SteadyClock(const Super &super);
    SteadyClock(Super &&super);

    template<typename T, typename U>
    SteadyClock(const std::chrono::duration<T, U> &duration)
    : Super(std::chrono::duration_cast<std::chrono::steady_clock::duration>(duration))
    {
    }

    static SteadyClock now();

    double seconds() const;
};

} //namespace WCDB

#endif /* Time_hpp */
