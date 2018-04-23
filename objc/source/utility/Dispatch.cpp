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

#include <WCDB/Dispatch.hpp>
#include <WCDB/ExitServer.hpp>
#include <thread>

namespace WCDB {

void Dispatch::async(
    const std::string &name,
    const std::function<void(const std::atomic<bool> &)> &callback,
    const std::function<void(void)> &atExit)
{
    std::thread t([name, callback, atExit]() {
        pthread_setname_np(name.c_str());
        std::atomic<bool> stop(false);
        std::atomic<bool> running(true);
        ExitServer *exitServer = ExitServer::shared();
        ExitServer::Token token =
            exitServer->enroll([&stop, &running, atExit]() {
                stop.store(true);
                if (atExit) {
                    atExit();
                }
                while (running.load())
                    ;
            });
        callback(stop);
        exitServer->disenroll(token);
        running.store(false);
    });
    t.detach();
}

} //namespace WCDB
