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
#include <WCDB/String.hpp>
#include <WCDB/ThreadedErrors.hpp>
#include <WCDB/Time.hpp>
#include <iomanip>
#include <sstream>

namespace WCDB {

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

std::string Time::stringify() const
{
    std::time_t seconds
    = (std::time_t) std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch())
      .count();
    struct tm tm;
    if (!localtime_r(&seconds, &tm)) {
        Error error;
        error.setSystemCode(errno, Error::Code::Error);
        Notifier::shared()->notify(error);
        setThreadedError(std::move(error));
        return String::empty();
    }
    std::time_t nanoseconds
    = (std::time_t) std::chrono::duration_cast<std::chrono::nanoseconds>(time_since_epoch())
      .count();
    while (nanoseconds != 0 && nanoseconds % 10 == 0) {
        nanoseconds /= 10;
    }
    std::ostringstream stream;
    stream << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S") << "." << nanoseconds;
    return stream.str();
}

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

double SteadyClock::seconds() const
{
    std::time_t seconds
    = (std::time_t) std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch())
      .count();
    std::time_t nanoseconds
    = (std::time_t) std::chrono::duration_cast<std::chrono::nanoseconds>(time_since_epoch())
      .count();
    return (double) seconds + (double) nanoseconds / 1E9;
}

} //namespace WCDB
