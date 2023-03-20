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

#include "ErrorProne.hpp"

namespace WCDB {

#pragma mark - ErrorProne
ErrorProne::~ErrorProne() = default;

const Error &ErrorProne::getError() const
{
    return m_error;
}

void ErrorProne::assignWithSharedThreadedError()
{
    setError(std::move(ThreadedErrors::shared().moveThreadedError()));
}

void ErrorProne::setError(const Error &error)
{
    m_error = error;
}

void ErrorProne::setError(Error &&error)
{
    m_error = std::move(error);
}

#pragma mark - ThreadedErrorProne
ThreadedErrorProne::~ThreadedErrorProne() = default;

void ThreadedErrorProne::setThreadedError(const Error &error)
{
    m_threadedError.setThreadedError(error);
}

void ThreadedErrorProne::setThreadedError(Error &&error)
{
    m_threadedError.setThreadedError(std::move(error));
}

const Error &ThreadedErrorProne::getThreadedError()
{
    return m_threadedError.getThreadedError();
}

void ThreadedErrorProne::assignWithSharedThreadedError()
{
    setThreadedError(std::move(ThreadedErrors::shared().moveThreadedError()));
}

} //namespace WCDB
