//
// Created by sanhuazhang on 2018/06/08
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

#include "SharedThreadedErrorProne.hpp"
#include "StringView.hpp"
#include "WCDBOptional.hpp"
#include <chrono>
#include <ctime>

namespace WCDB {

class Time final : public std::chrono::system_clock::time_point, public SharedThreadedErrorProne {
public:
    using Super = std::chrono::system_clock::time_point;
    Time();
    Time(const Super &super);
    Time(Super &&super);
    Time(const struct timespec &ts);
    Time(const uint64_t &second);
    ~Time() override;

    static Time now();
    static uint64_t currentThreadCPUTimeInMicroseconds();

    std::time_t seconds() const;
    std::time_t nanoseconds() const;
    bool empty() const;
    Optional<StringView> stringify() const;
};

class SteadyClock final : public std::chrono::steady_clock::time_point {
public:
    using Super = std::chrono::steady_clock::time_point;
    SteadyClock();
    SteadyClock(const Super &super);
    SteadyClock(Super &&super);

    template<typename T, typename U>
    SteadyClock(const std::chrono::duration<T, U> &duration)
    : Super(std::chrono::duration_cast<std::chrono::steady_clock::duration>(duration))
    {
    }

    SteadyClock steadyClockByAddingTimeInterval(double seconds) const;

    double timeIntervalSinceSteadyClock(const SteadyClock &other) const;

    double timeIntervalSinceNow() const;

    static double timeIntervalSinceSteadyClockToNow(const SteadyClock &other);

    static SteadyClock now();
};

} //namespace WCDB
