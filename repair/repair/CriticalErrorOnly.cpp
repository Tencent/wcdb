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

CriticalErrorOnly::CriticalErrorOnly() : m_criticalLevel(CriticalLevel::None)
{
}

const Error &CriticalErrorOnly::getCriticalError() const
{
    return m_criticalError;
}

int CriticalErrorOnly::criticalLevel(const Error &error)
{
    if (error.level < Error::Level::Error) {
        return -1;
    }
    switch (error.code()) {
        case Error::Code::Full:
            return CriticalLevel::Fatal + 3;
        case Error::Code::Corrupt:
        case Error::Code::NotADatabase:
            return CriticalLevel::Fatal + 2;
        case Error::Code::IOError:
            return CriticalLevel::Fatal + 1;
        case Error::Code::NoMemory:
            return CriticalLevel::Fatal;
        case Error::Code::OK:
            return CriticalLevel::None;
        default:
            return CriticalLevel::NotFatal;
    }
}

int CriticalErrorOnly::tryUpgradeError(const Error &newError)
{
    int newCriticalLevel = criticalLevel(newError);
    if (newCriticalLevel > m_criticalLevel) {
        m_criticalError = newError;
        m_criticalLevel = newCriticalLevel;
        if (m_criticalLevel >= CriticalLevel::Fatal) {
            onErrorCritical();
        }
    }
    return m_criticalLevel;
}

int CriticalErrorOnly::tryUpgradeError(Error &&newError)
{
    int newCriticalLevel = criticalLevel(newError);
    if (newCriticalLevel > m_criticalLevel) {
        m_criticalError = newError;
        m_criticalLevel = std::move(newCriticalLevel);
        if (m_criticalLevel >= CriticalLevel::Fatal) {
            onErrorCritical();
        }
    }
    return m_criticalLevel;
}

int CriticalErrorOnly::tryUpgradeErrorWithSharedThreadedError()
{
    return tryUpgradeError(
        std::move(ThreadedErrors::shared()->moveThreadedError()));
}

int CriticalErrorOnly::getCriticalLevel() const
{
    return m_criticalLevel;
}

void CriticalErrorOnly::setCriticalError(const Error &error)
{
    m_criticalError = error;
    m_criticalLevel = CriticalLevel::MostFatal;
    onErrorCritical();
}

void CriticalErrorOnly::setCriticalError(Error &&error)
{
    m_criticalError = std::move(error);
    m_criticalLevel = CriticalLevel::MostFatal;
    onErrorCritical();
}

void CriticalErrorOnly::setCriticalErrorWIthSharedThreadedError()
{
    m_criticalError = std::move(ThreadedErrors::shared()->moveThreadedError());
    m_criticalLevel = CriticalLevel::MostFatal;
    onErrorCritical();
}

void CriticalErrorOnly::onErrorCritical()
{
}

} //namespace Repair

} //namespace WCDB
