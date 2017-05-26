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

#include <WCDB/in_case_lock_guard.hpp>
#include <WCDB/utility.hpp>
#include <WCDB/error.hpp>

namespace WCDB {

InCaseLockGuard::InCaseLockGuard(std::shared_ptr<std::mutex>& mutex)
: m_mutex(mutex)
{
    if (!m_mutex->try_lock()) {
        WCDB::Error::Abort("This context can not be accessed by multiple threads at the same time");
        m_mutex->lock();
    }
}

InCaseLockGuard::~InCaseLockGuard()
{
    if (m_mutex) {
        m_mutex->unlock();
    }
}

}//namespace WCDB
