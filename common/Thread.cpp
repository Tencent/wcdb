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
#include <WCDB/Notifier.hpp>
#include <WCDB/Thread.hpp>

namespace WCDB {

Thread::Thread(pthread_t id) : m_id(id)
{
}

Thread::Thread() : Thread(nullptr)
{
}

Thread& Thread::operator=(const std::nullptr_t&)
{
    m_id = nullptr;
    return *this;
}

Thread Thread::current()
{
    return Thread(pthread_self());
}

bool Thread::isMain()
{
    return pthread_main_np() != 0;
}

void Thread::setName(const String& name)
{
    if (pthread_setname_np(name.c_str()) != 0) {
        Error error;
        error.level = Error::Level::Error;
        error.setSystemCode(errno, Error::Code::Error);
        Notifier::shared().notify(error);
        SharedThreadedErrorProne::setThreadedError(std::move(error));
    }
}

bool Thread::isCurrentThread() const
{
    return equal(Thread::current());
}

bool Thread::equal(const Thread& other) const
{
    return pthread_equal(m_id, other.m_id) != 0;
}

bool Thread::operator==(const Thread& other) const
{
    return equal(other);
}

} // namespace WCDB
