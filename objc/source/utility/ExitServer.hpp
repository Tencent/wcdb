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

#ifndef ExitServer_hpp
#define ExitServer_hpp

#include <functional>
#include <map>
#include <mutex>

#pragma GCC visibility push(hidden)

namespace WCDB {

class ExitServer {
public:
    static ExitServer *shared();

    typedef unsigned int Token;

    int invalidToken() const;

    Token enroll(const std::function<void(void)> &onExit);
    void disenroll(const Token &token);

protected:
    ExitServer();
    void notify();

    Token m_token;
    std::map<int, std::function<void(void)>> m_listeners;
    std::mutex m_mutex;
    std::atomic<bool> m_exiting;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* ExitServer_hpp */
