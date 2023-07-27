//
// Created by sanhuazhang on 2019/06/06
//

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

#include "AsyncQueue.hpp"
#include "Assertion.hpp"
#include "CoreConst.h"
#include "Exiting.hpp"
#include "Notifier.hpp"
#include "StringView.hpp"
#include "WCDBError.hpp"
#include <atomic>
#include <thread>

namespace WCDB {

AsyncQueue::AsyncQueue(const UnsafeStringView& name_) : name(name_)
{
}

AsyncQueue::~AsyncQueue()
{
    if (m_running.valid()
        && m_running.wait_for(
           std::chrono::nanoseconds((long long) (AsyncQueueTimeOutForExiting * 1E9)))
           == std::future_status::timeout) {
        Error error(Error::Code::Warning, Error::Level::Warning, "Queue does not exit on time.");
        error.infos.insert_or_assign("Timeout", AsyncQueueTimeOutForExiting);
        error.infos.insert_or_assign("Name", name);
        Notifier::shared().notify(error);
    }
}

void AsyncQueue::run()
{
    m_running = std::async(std::launch::async, &AsyncQueue::load, this);
}

void AsyncQueue::load()
{
    Thread::setName(name);
    main();
}

} // namespace WCDB
