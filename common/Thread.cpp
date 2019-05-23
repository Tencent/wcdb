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

#include <WCDB/Thread.hpp>

namespace WCDB {

Thread::Thread(const std::thread::id& id) : m_id(id)
{
}

Thread::Thread() : Thread(std::thread::id())
{
}

Thread& Thread::operator=(const std::nullptr_t&)
{
    m_id = std::thread::id();
    return *this;
}

Thread Thread::current()
{
    return Thread(std::this_thread::get_id());
}

bool Thread::isMain()
{
    return pthread_main_np() != 0;
}

bool Thread::isCurrentThread() const
{
    return equal(Thread::current());
}

bool Thread::equal(const Thread& other) const
{
    return m_id == other.m_id;
}

} // namespace WCDB
