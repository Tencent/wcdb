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

static bool s_exited = false;

ExitServer *ExitServer::shared()
{
    static ExitServer s_exitSender;
    static std::once_flag s_once;
    std::call_once(s_once, []() {
        atexit([]() {
            if (!s_exited) {
                s_exitSender.notify();
            }
            s_exited = true;
        });
    });
    return &s_exitSender;
}

ExitServer::~ExitServer()
{
    s_exited = true;
    notify();
}

ExitServer::Token ExitServer::enroll(const std::function<void(void)> &onExit)
{
    if (m_exiting.load()) {
        return invalidToken();
    }
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

int ExitServer::invalidToken() const
{
    return 0;
}

void ExitServer::notify()
{
    m_exiting.store(true);
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    for (const auto &iter : m_listeners) {
        if (iter.second) {
            iter.second();
        }
    }
    m_listeners.clear();
}

} //namespace WCDB
