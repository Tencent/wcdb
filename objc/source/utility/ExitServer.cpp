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

#include <WCDB/ExitServer.hpp>

namespace WCDB {

ExitServer *ExitServer::shared()
{
    static ExitServer s_exitSender;
    atexit([]() { s_exitSender.notify(); });
    return &s_exitSender;
}

ExitServer::Token ExitServer::enroll(const std::function<void(void)> &onExit)
{
    assert(!m_exiting.load());
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    ++m_token;
    m_listeners.insert({m_token, onExit});
    return m_token;
}

void ExitServer::disenroll(const Token &token)
{
    if (m_exiting.load()) {
        return;
    }
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    m_listeners.erase(token);
}

ExitServer::ExitServer() : m_token(0), m_exiting(false)
{
}

void ExitServer::notify()
{
    m_exiting.store(true);
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    for (const auto &iter : m_listeners) {
        iter.second();
    }
    m_listeners.clear();
}

} //namespace WCDB
