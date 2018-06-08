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
    switch (error.code()) {
        case Error::Code::Full:
            return std::numeric_limits<int>::max();
        case Error::Code::Corrupt:
        case Error::Code::NotADatabase:
            if (error.level == Error::Level::Warning) {
                return -1;
            }
            return std::numeric_limits<int>::max() - 1;
        case Error::Code::IOError:
            return std::numeric_limits<int>::max() - 2;
        case Error::Code::OK:
            return std::numeric_limits<int>::min();
        default:
            return 0;
    }
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

void CriticalErrorOnly::tryUpgradeErrorWithSharedThreadedError()
{
    tryUpgradeError(std::move(ThreadedErrors::shared()->moveThreadedError()));
}

bool CriticalErrorOnly::isCriticalErrorFatal() const
{
    return criticalLevel(m_criticalError) > 0;
}

} //namespace Repair

} //namespace WCDB
