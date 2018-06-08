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

Time::Time() : m_time(std::time(nullptr))
{
}

std::pair<bool, std::string> Time::stringify() const
{
    struct tm tm;
    if (!localtime_r(&m_time, &tm)) {
        setThreadedError(Error::Code::Exceed);
        return {false, String::empty()};
    }
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
    return {true, oss.str()};
}

} //namespace WCDB
