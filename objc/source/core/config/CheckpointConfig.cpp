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

#include <WCDB/Assertion.hpp>
#include <WCDB/CheckpointConfig.hpp>
#include <WCDB/CheckpointQueue.hpp>
#include <WCDB/String.hpp>
#include <mutex>

namespace WCDB {

CheckpointConfig::CheckpointConfig(const std::shared_ptr<CheckpointQueue>& queue)
: Config(), m_identifier(String::formatted("Checkpoint-%p", this)), m_queue(queue)
{
    WCTInnerAssert(m_queue != nullptr);
}

bool CheckpointConfig::invoke(Handle* handle)
{
    handle->setNotificationWhenCommitted(
    0,
    m_identifier,
    std::bind(&CheckpointConfig::onCommitted, this, std::placeholders::_1, std::placeholders::_2));
    return true;
}

bool CheckpointConfig::uninvoke(Handle* handle)
{
    handle->unsetNotificationWhenCommitted(m_identifier);
    return true;
}

bool CheckpointConfig::onCommitted(Handle* handle, int frames)
{
    if (frames > framesForPassive) {
        m_queue->put(handle->path, 1.0, frames);
    } else {
        m_queue->put(handle->path, 10.0, frames);
    }
    return true;
}

} //namespace WCDB
