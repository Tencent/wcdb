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

#include "Time.hpp"
#include "Notifier.hpp"
#include "StringView.hpp"
#include "ThreadedErrors.hpp"
#include <iomanip>
#include <sstream>

#ifdef __APPLE__
#include <mach/mach_init.h>
#include <mach/thread_act.h>
#include <mach/thread_info.h>
#elif _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <pthread.h>
#include <time.h>
#endif

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

Time::Time(const uint64_t &second)
: Super(std::chrono::system_clock::time_point{
std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::seconds{ second }) })
{
}

Time::~Time() = default;

Time Time::now()
{
    return std::chrono::system_clock::now();
}

uint64_t Time::currentThreadCPUTimeInMicroseconds()
{
#ifdef __APPLE__
    thread_basic_info_data_t basic_info;
    mach_msg_type_number_t count = THREAD_BASIC_INFO_COUNT;
    kern_return_t result = thread_info(
    mach_thread_self(), THREAD_BASIC_INFO, (thread_info_t) &basic_info, &count);
    if (result != KERN_SUCCESS) {
        return 0;
    }
    return basic_info.user_time.seconds * 1000000 + basic_info.user_time.microseconds
           + basic_info.system_time.seconds * 1000000 + basic_info.system_time.microseconds;
#elif _WIN32
    HANDLE currentThread = GetCurrentThread();
    FILETIME creationTime, exitTime, kernelTime, userTime;
    if (GetThreadTimes(currentThread, &creationTime, &exitTime, &kernelTime, &userTime)) {
        uint64_t ret = ((uint64_t) kernelTime.dwHighDateTime << 32) + kernelTime.dwLowDateTime;
        ret += ((uint64_t) userTime.dwHighDateTime << 32) + userTime.dwLowDateTime;
        ret /= 10;
        return ret;
    }
    return 0;
#else
    clockid_t cid;
    struct timespec ts;
    if (pthread_getcpuclockid(pthread_self(), &cid) == 0 && clock_gettime(cid, &ts) == 0) {
        return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
    }
    return 0;
#endif
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

Optional<StringView> Time::stringify() const
{
    std::time_t nanoseconds
    = (std::time_t) std::chrono::duration_cast<std::chrono::nanoseconds>(time_since_epoch())
      .count();
    std::time_t secondsPart = nanoseconds / (int) 1E9;
    struct tm tm;
    int err = 0;
#ifndef _WIN32
    if (localtime_r(&secondsPart, &tm) == nullptr) {
        err = errno;
#else
    err = localtime_s(&tm, &secondsPart);
    if (err) {
#endif
        Error error;
        error.level = Error::Level::Error;
        error.setSystemCode(err, Error::Code::Error);
        Notifier::shared().notify(error);
        setThreadedError(std::move(error));
        return NullOpt;
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
