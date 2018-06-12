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

#include <WCDB/HandlePoolRelated.hpp>
#include <WCDB/HandlePools.hpp>

namespace WCDB {

HandlePoolRelated::HandlePoolRelated(HandlePool *pool) : m_pool(pool)
{
}

const Error &HandlePoolRelated::getThreadedError()
{
    return m_pool->getThreadedError();
}

void HandlePoolRelated::setThreadedError(const Error &error)
{
    m_pool->setThreadedError(error);
}

void HandlePoolRelated::setThreadedError(Error &&error)
{
    m_pool->setThreadedError(std::move(error));
}

void HandlePoolRelated::assignWithSharedThreadedError()
{
    m_pool->assignWithSharedThreadedError();
}

} //namespace WCDB
