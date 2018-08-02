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

#include <WCDB/Core.h>
#include <WCDB/Dispatch.hpp>
#include <atomic>

namespace WCDB {

AsyncQueue::AsyncQueue(const std::string& name_)
: name(name_)
#ifdef DEBUG
, m_running(false)
#endif
{
}

void AsyncQueue::run()
{
#ifdef DEBUG
    WCTInnerAssert(m_running == false);
    m_running = true;
#endif
    Dispatch::async(name, std::bind(&AsyncQueue::loop, this));
}

bool AsyncQueue::exit()
{
    static std::atomic<bool>* s_exit = new std::atomic<bool>(false);
    atexit([]() { s_exit->store(true); });
    return s_exit->load();
}

} // namespace WCDB
