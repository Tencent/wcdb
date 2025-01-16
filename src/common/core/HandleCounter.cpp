//
// Created by 陈秋文 on 2023/8/13.
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

#include "HandleCounter.hpp"
#include "Assertion.hpp"
#include "WCDBLog.hpp"
#include <condition_variable>

namespace WCDB {

HandleCounter::HandleCounter() : m_writerCount(0), m_totalCount(0)
{
}

HandleCounter::~HandleCounter() = default;

static int handlerIndex = 0 ;

bool HandleCounter::tryIncreaseHandleCount(HandleType type, bool writeHint)
{
    std::unique_lock<std::mutex> lockGuard(m_lock);
    handlerIndex++;
    WCDB2_LOGI("tryIncreaseHandleCount-start-获得一个%s handle, writerCount=%d,m_totalCount=%d,handlerIndex=%d",
               writeHint ? "可写-tryIncrease" : "仅读-tryIncrease", m_writerCount, m_totalCount, handlerIndex);
    while (m_totalCount >= HandlePoolMaxAllowedNumberOfHandles
           || (writeHint && m_writerCount >= HandlePoolMaxAllowedNumberOfWriters)) {
        while (m_totalCount >= HandlePoolMaxAllowedNumberOfHandles) {
            if (!handleShouldWaitWhenFull(type)) {
                return false;
            }
            m_pendingNormals.emplace(Thread::current());
            m_conditionalNormals.wait(lockGuard);
            m_pendingNormals.pop();
        }
        while (writeHint && m_writerCount >= HandlePoolMaxAllowedNumberOfWriters) {
            if (!handleShouldWaitWhenFull(type)) {
                return false;
            }
            m_pendingWriters.emplace(Thread::current());
            m_conditionalWriters.wait(lockGuard);
            m_pendingWriters.pop();
        }
    }
    if (writeHint) {
        m_writerCount++;
    }
    m_totalCount++;
    WCDB2_LOGI("tryIncreaseHandleCount-end-获得一个%s handle, writerCount=%d,m_totalCount=%d,handlerIndex=%d",
              writeHint ? "可写-tryIncrease" : "仅读-tryIncrease", m_writerCount, m_totalCount,handlerIndex);
    return true;
}

void HandleCounter::decreaseHandleCount(bool writeHint)
{
    bool hasNotified = false;
    std::unique_lock<std::mutex> lockGuard(m_lock);
    WCDB2_LOGI("decreaseHandleCount-start-释放一个%s handle, writerCount=%d,m_totalCount=%d,handlerIndex=%d",
               writeHint ? "可写-decrease" : "仅读-decrease", m_writerCount, m_totalCount,handlerIndex);
    if (writeHint) {
        m_writerCount--;
        WCTAssert(m_writerCount >= 0);
        if (m_pendingWriters.size() > 0) {
#ifdef __APPLE__
            m_conditionalWriters.notify(m_pendingWriters.front());
#else
            m_conditionalWriters.notify_all();
#endif
            hasNotified = true;
        }
    }
    m_totalCount--;
    WCTAssert(m_totalCount >= 0);
    if (!hasNotified && m_pendingNormals.size() > 0) {
#ifdef __APPLE__
        m_conditionalNormals.notify(m_pendingNormals.front());
#else
        m_conditionalNormals.notify_all();
#endif
    }
    WCDB2_LOGI("decreaseHandleCount-end-释放一个%s handle, writerCount=%d,m_totalCount=%d,handlerIndex=%d",
              writeHint ? "可写-decrease" : "仅读-decrease", m_writerCount, m_totalCount,handlerIndex);
}

} // namespace WCDB
