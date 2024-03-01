//
// Created by sanhuazhang on 2018/09/26
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

#include "UpgradeableErrorProne.hpp"
#include "Assertion.hpp"
#include "ThreadedErrors.hpp"

namespace WCDB {

namespace Repair {

UpgradeableErrorProne::UpgradeableErrorProne() : m_severity(Severity::None)
{
}

UpgradeableErrorProne::~UpgradeableErrorProne() = default;

int UpgradeableErrorProne::getErrorSeverity() const
{
    return m_severity;
}

bool UpgradeableErrorProne::isErrorCritial() const
{
    return m_severity >= Severity::Critical;
}

int UpgradeableErrorProne::errorSeverity(const Error &error)
{
    if (error.level == Error::Level::Ignore) {
        if (error.code() != Error::Code::Interrupt) {
            return -1;
        }
    } else if (error.level < Error::Level::Error) {
        return -1;
    }
    switch (error.code()) {
    case Error::Code::Full:
        return Severity::Critical + 3;
    case Error::Code::Corrupt:
    case Error::Code::NotADatabase:
        return Severity::Critical + 2;
    case Error::Code::IOError:
        return Severity::Critical + 1;
    case Error::Code::NoMemory:
    case Error::Code::Interrupt:
        return Severity::Critical;
    case Error::Code::OK:
        return Severity::None;
    default:
        return Severity::Normal;
    }
}

int UpgradeableErrorProne::tryUpgradeError(const Error &error)
{
    int severity = errorSeverity(error);
    if (severity > m_severity) {
        ErrorProne::setError(error);
        setSeverity(severity);
    }
    return m_severity;
}

int UpgradeableErrorProne::tryUpgradeError(Error &&error)
{
    int severity = errorSeverity(error);
    if (severity > m_severity) {
        ErrorProne::setError(std::move(error));
        setSeverity(severity);
    }
    return m_severity;
}

int UpgradeableErrorProne::tryUpgradeErrorWithSharedThreadedError()
{
    return tryUpgradeError(std::move(ThreadedErrors::shared().moveThreadedError()));
}

void UpgradeableErrorProne::setCriticalError(const Error &error)
{
    ErrorProne::setError(error);
    setSeverity(Severity::MostCritical);
    onErrorCritical();
}

void UpgradeableErrorProne::setCriticalError(Error &&error)
{
    ErrorProne::setError(std::move(error));
    setSeverity(Severity::MostCritical);
    onErrorCritical();
}

void UpgradeableErrorProne::setCriticalErrorWithSharedThreadedError()
{
    setCriticalError(std::move(ThreadedErrors::shared().moveThreadedError()));
}

void UpgradeableErrorProne::setSeverity(int severity)
{
    WCTAssert(severity > m_severity);
    WCTAssert(m_severity < Severity::Critical);
    if (severity > m_severity) {
        m_severity = severity;
        if (m_severity >= Severity::Critical) {
            onErrorCritical();
        }
    }
}

void UpgradeableErrorProne::onErrorCritical()
{
}

} //namespace Repair

} //namespace WCDB
