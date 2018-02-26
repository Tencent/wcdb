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

#include <WCDB/rwlock.hpp>

namespace WCDB {

RWLock::RWLock() : m_reader(0), m_writer(0), m_pending(0)
{
}

RWLock::~RWLock()
{
}

void RWLock::lockRead()
{
    std::unique_lock<decltype(m_mutex)> lock(m_mutex);
    while (m_writer > 0 || m_pending > 0) {
        m_cond.wait(lock);
    }
    ++m_reader;
}

void RWLock::unlockRead()
{
    std::unique_lock<decltype(m_mutex)> lock(m_mutex);
    --m_reader;
    if (m_reader == 0) {
        m_cond.notify_all();
    }
}

bool RWLock::tryLockRead()
{
    std::unique_lock<decltype(m_mutex)> lock(m_mutex);
    if (m_writer > 0 || m_pending > 0) {
        return false;
    }
    ++m_reader;
    return true;
}

void RWLock::lockWrite()
{
    std::unique_lock<decltype(m_mutex)> lock(m_mutex);
    ++m_pending;
    while (m_writer > 0 || m_reader > 0) {
        m_cond.wait(lock);
    }
    --m_pending;
    ++m_writer;
}

void RWLock::unlockWrite()
{
    std::unique_lock<decltype(m_mutex)> lock(m_mutex);
    --m_writer;
    m_cond.notify_all();
}

bool RWLock::tryLockWrite()
{
    std::unique_lock<decltype(m_mutex)> lock(m_mutex);
    if (m_writer > 0 || m_reader > 0) {
        return false;
    }
    ++m_writer;
    return true;
}

bool RWLock::isWriting() const
{
    std::unique_lock<decltype(m_mutex)> lock(m_mutex);
    return m_writer > 0;
}

bool RWLock::isReading() const
{
    std::unique_lock<decltype(m_mutex)> lock(m_mutex);
    return m_reader > 0;
}

} //namespace WCDB
