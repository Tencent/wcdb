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

#ifndef AsyncLoop_hpp
#define AsyncLoop_hpp

#include <atomic>
#include <mutex>
#include <string>
#include <thread>

#pragma GCC visibility push(hidden)

namespace WCDB {

template <int id>
class AsyncLoop {
public:
    static void once(const std::string &name,
                     const std::function<void(std::atomic<bool> &)> &loop,
                     const std::function<void(void)> &atExit)
    {
        static std::once_flag s_flag;
        std::call_once(s_flag, [&name, &loop, &atExit]() {
            AsyncLoop::run(name, loop, atExit);
        });
    }

    static void run(const std::string &name,
                    const std::function<void(std::atomic<bool> &)> &loop,
                    const std::function<void(void)> &atExit)
    {
        static std::thread s_thread([loop, name, atExit]() {
            static std::atomic<bool> s_stop(false);
            static std::atomic<bool> s_running(true);
            static std::function<void(void)> s_atExit(atExit);
            pthread_setname_np(name.c_str());
            atexit([]() {
                s_stop.store(true);
                if (s_atExit) {
                    s_atExit();
                }
                while (s_running.load())
                    ;
            });
            while (!s_stop.load()) {
                loop(s_stop);
            }
            s_running.store(false);
        });
        s_thread.detach();
    }
};

#define AsyncRunloop(name, closure, atExit)                                    \
    AsyncLoop<__COUNTER__>::run((name), (closure), (atExit))

#define AsyncRunloopOnce(name, closure, atExit)                                \
    AsyncLoop<__COUNTER__>::once((name), (closure), (atExit))

} //namespace WCDB

#pragma GCC visibility pop

#endif /* Dispatch_hpp */
