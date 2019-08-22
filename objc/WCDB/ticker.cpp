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

#include <WCDB/ticker.hpp>
#include <mutex>
#ifdef _WIN32
#include <windows.h>
#else
#include <mach/mach_time.h>
#endif

namespace WCDB {

Ticker::Ticker() : m_base(0)
{
}

void Ticker::tick()
{
#ifdef _WIN32
#define EPOCHFILETIME (116444736000000000UL)
    FILETIME ft;
    LARGE_INTEGER li;
    int64_t now = 0;
    GetSystemTimeAsFileTime(&ft);
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    // 从1970年1月1日0:0:0:000到现在的微秒数(UTC时间)
    now = (li.QuadPart - EPOCHFILETIME) / 10;
#else
    uint64_t now = mach_absolute_time();
#endif
    if (m_base != 0) {
        m_elapses.push_back(now - m_base);
    }
    m_base = now;
}

void Ticker::pause()
{
    tick();
    m_base = 0;
}

std::vector<double> Ticker::getElapseTimes() const
{
    std::vector<double> result;
    for (const auto &elapse : m_elapses) {
        result.push_back(secondsFromElapse(elapse));
    }
    return result;
}

double Ticker::getElapseTime() const
{
    double result = 0;
    for (const auto &elapse : m_elapses) {
        result += secondsFromElapse(elapse);
    }
    return result;
}

std::string Ticker::log() const
{
    std::string result;
    int i = 0;
    for (const auto &cost : getElapseTimes()) {
        result.append(std::to_string(i) + ": " + std::to_string(cost) + "\n");
        ++i;
    }
    return result;
}

inline double Ticker::secondsFromElapse(const uint64_t &elapse)
{
#ifdef _WIN32
    return (double) elapse / 1000 / 1000;
#else
    static double s_numer = 0;
    static double s_denom = 0;
    static std::once_flag s_once;
    std::call_once(s_once, []() {
        mach_timebase_info_data_t info;
        mach_timebase_info(&info);
        s_numer = info.numer;
        s_denom = info.denom;
    });

    return (double) elapse * s_numer / s_denom / 1000 / 1000 / 1000;
#endif
}

ScopedTicker::ScopedTicker(std::shared_ptr<Ticker> &ticker) : m_ticker(ticker)
{
    if (m_ticker) {
        m_ticker->tick();
    }
}

ScopedTicker::~ScopedTicker()
{
    if (m_ticker) {
        m_ticker->pause();
    }
}

} //namespace WCDB
