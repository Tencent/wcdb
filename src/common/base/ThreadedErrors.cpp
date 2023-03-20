//
// Created by sanhuazhang on 2018/05/24
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

#include "ThreadedErrors.hpp"
#include "CoreConst.h"
#include "Macro.h"

namespace WCDB {

ThreadedErrors &ThreadedErrors::shared()
{
    static ThreadedErrors *s_threadedErrors = new ThreadedErrors;
    return *s_threadedErrors;
}

const Error &ThreadedErrors::getThreadedError()
{
    return m_errors.getOrCreate();
}

Error &&ThreadedErrors::moveThreadedError()
{
    return std::move(m_errors.getOrCreate());
}

void ThreadedErrors::setThreadedError(const Error &error)
{
    m_errors.getOrCreate() = error;
}

void ThreadedErrors::setThreadedError(Error &&error)
{
    m_errors.getOrCreate() = std::move(error);
}

} //namespace WCDB
