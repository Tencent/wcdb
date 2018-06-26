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

#include <WCDB/Error.hpp>
#include <WCDB/String.hpp>
#include <WCDB/Time.hpp>
#include <iomanip>
#include <sstream>

namespace WCDB {

Time::Time() : m_sec(0), m_nsec(0)
{
}

Time::Time(long sec, long nsec) : m_sec(sec), m_nsec(nsec)
{
}

long Time::sec() const
{
    return m_sec;
}

long Time::nsec() const
{
    return m_nsec;
}

bool Time::now()
{
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
        Error error;
        error.setSystemCode(errno, Error::Code::Error);
        setThreadedError(std::move(error));
        return false;
    }
    m_sec = ts.tv_sec;
    m_nsec = ts.tv_nsec;
    return true;
}

bool Time::empty() const
{
    return m_sec == 0 && m_nsec == 0;
}

std::pair<bool, std::string> Time::stringify() const
{
    struct tm tm;
    if (!localtime_r(&m_sec, &tm)) {
        setThreadedError(Error::Code::Exceed);
        return {false, String::empty()};
    }
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S") << "." << m_nsec;
    return {true, oss.str()};
}

bool Time::operator==(const Time &operand) const
{
    return m_nsec == operand.m_nsec && m_sec == operand.m_sec;
}

bool Time::operator!=(const Time &operand) const
{
    return m_sec != operand.m_sec && m_nsec != operand.m_nsec;
}

bool Time::operator>(const Time &operand) const
{
    if (m_sec != operand.m_sec) {
        return m_sec > operand.m_sec;
    }
    return m_nsec > operand.m_nsec;
}

bool Time::operator<(const Time &operand) const
{
    if (m_sec != operand.m_sec) {
        return m_sec < operand.m_sec;
    }
    return m_nsec < operand.m_nsec;
}

long Time::second() const
{
    return m_sec;
}

long Time::nanosecond() const
{
    return m_nsec;
}

} //namespace WCDB
