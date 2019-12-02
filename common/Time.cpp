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

#include <WCDB/Notifier.hpp>
#include <WCDB/StringView.hpp>
#include <WCDB/ThreadedErrors.hpp>
#include <WCDB/Time.hpp>
#include <iomanip>
#include <sstream>

namespace WCDB {

Time::Time() = default;

Time::Time(const Super &super) : Super(super)
{
}

Time::Time(Super &&super) : Super(std::move(super))
{
}

Time::Time(const struct timespec &ts)
: Super(std::chrono::system_clock::time_point{
std::chrono::duration_cast<std::chrono::system_clock::duration>(
std::chrono::seconds{ ts.tv_sec } + std::chrono::nanoseconds{ ts.tv_nsec }) })
{
}

Time::~Time() = default;

Time Time::now()
{
    return std::chrono::system_clock::now();
}

std::time_t Time::seconds() const
{
    return (std::time_t) std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch())
    .count();
}

std::time_t Time::nanoseconds() const
{
    return (std::time_t) std::chrono::duration_cast<std::chrono::nanoseconds>(time_since_epoch())
    .count();
}

bool Time::empty() const
{
    return time_since_epoch().count() == 0;
}

std::optional<StringView> Time::stringify() const
{
    std::time_t nanoseconds
    = (std::time_t) std::chrono::duration_cast<std::chrono::nanoseconds>(time_since_epoch())
      .count();
    std::time_t secondsPart = nanoseconds / (int) 1E9;
    struct tm tm;
    if (localtime_r(&secondsPart, &tm) == nullptr) {
        Error error;
        error.level = Error::Level::Error;
        error.setSystemCode(errno, Error::Code::Error);
        Notifier::shared().notify(error);
        setThreadedError(std::move(error));
        return std::nullopt;
    }
    std::time_t nanosecondsPart = nanoseconds % (int) 1E9;
    std::ostringstream stream;
    stream << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S") << "." << nanosecondsPart;
    return StringView(stream.str());
}

SteadyClock::SteadyClock() = default;

SteadyClock::SteadyClock(const Super &super) : Super(super)
{
}

SteadyClock::SteadyClock(Super &&super) : Super(std::move(super))
{
}

SteadyClock SteadyClock::now()
{
    return std::chrono::steady_clock::now();
}

double SteadyClock::timeIntervalSinceSteadyClock(const SteadyClock &other) const
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(*this - other).count() / 1E9;
}

double SteadyClock::timeIntervalSinceNow() const
{
    return timeIntervalSinceSteadyClock(SteadyClock::now());
}

double SteadyClock::timeIntervalSinceSteadyClockToNow(const SteadyClock &other)
{
    return SteadyClock::now().timeIntervalSinceSteadyClock(other);
}

SteadyClock SteadyClock::steadyClockByAddingTimeInterval(double seconds) const
{
    return SteadyClock(*this + std::chrono::nanoseconds((long long) (seconds * 1E9)));
}

} //namespace WCDB
