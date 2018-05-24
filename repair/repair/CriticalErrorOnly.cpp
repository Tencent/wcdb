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

#include <WCDB/CriticalErrorOnly.hpp>
#include <WCDB/ThreadedErrors.hpp>

namespace WCDB {

namespace Repair {

const Error &CriticalErrorOnly::getCriticalError() const
{
    return m_criticalError;
}

int CriticalErrorOnly::criticalLevel(const Error &error)
{
    static std::map<Error::Code, int> s_criticalLevel = {
        {Error::Code::Full, std::numeric_limits<int>::max()},
        {Error::Code::IOError, std::numeric_limits<int>::max() - 1},
        {Error::Code::NoMemory, std::numeric_limits<int>::max() - 2},
        // 0
        {Error::Code::Corrupt, -1},
        {Error::Code::OK, std::numeric_limits<int>::min()},
    };
    auto iter = s_criticalLevel.find(error.code());
    if (iter == s_criticalLevel.end()) {
        return 0;
    }
    return iter->second;
}

void CriticalErrorOnly::tryUpgradeError(const Error &newError)
{
    if (criticalLevel(newError) > criticalLevel(m_criticalError)) {
        m_criticalError = newError;
    }
}

void CriticalErrorOnly::tryUpgradeError(Error &&newError)
{
    if (criticalLevel(newError) > criticalLevel(m_criticalError)) {
        m_criticalError = std::move(newError);
    }
}

void CriticalErrorOnly::tryUpgradeErrorWithThreadedError()
{
    tryUpgradeError(std::move(ThreadedErrors::shared()->getThreadedError()));
}

} //namespace Repair

} //namespace WCDB
