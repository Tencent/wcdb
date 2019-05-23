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
#include <WCDB/Global.hpp>
#include <WCDB/Handle.hpp>
#include <WCDB/String.hpp>
#include <regex>

namespace WCDB {

CheckpointConfig::CheckpointConfig(const std::shared_ptr<CheckpointQueue>& queue)
: Config(), m_identifier(String::formatted("Checkpoint-%p", this)), m_queue(queue)
{
    WCTInnerAssert(m_queue != nullptr);

    Global::shared().setNotificationForLog(
    m_identifier,
    std::bind(&CheckpointConfig::log, this, std::placeholders::_1, std::placeholders::_2));
}

CheckpointConfig::~CheckpointConfig()
{
    Global::shared().setNotificationForLog(m_identifier, nullptr);
}

bool CheckpointConfig::invoke(Handle* handle)
{
    m_queue->register_(handle->getPath());

    handle->setNotificationWhenCheckpointed(
    m_identifier,
    std::bind(&CheckpointConfig::onCheckpointed, this, std::placeholders::_1));

    handle->setNotificationWhenCommitted(
    0,
    m_identifier,
    std::bind(&CheckpointConfig::onCommitted, this, std::placeholders::_1, std::placeholders::_2));

    return true;
}

bool CheckpointConfig::uninvoke(Handle* handle)
{
    handle->unsetNotificationWhenCommitted(m_identifier);

    handle->setNotificationWhenCheckpointed(m_identifier, nullptr);

    m_queue->unregister(handle->getPath());

    return true;
}

bool CheckpointConfig::onCommitted(const String& path, int frames)
{
    m_queue->put(path, frames);
    return true;
}

void CheckpointConfig::onCheckpointed(const String& path)
{
    m_queue->remove(path);
}

void CheckpointConfig::log(int rc, const char* message)
{
    Error::ExtCode extCode = Error::rc2ec(rc);
    if (extCode != Error::ExtCode::NoticeRecoverWal || message == nullptr) {
        return;
    }
    std::regex pattern("recovered (\\w+) frames from WAL file (.+)\\-wal");
    const String source = message;
    std::smatch match;
    if (std::regex_search(source.begin(), source.end(), match, pattern)) {
        int frames = atoi(match[1].str().c_str());
        if (frames > 0) {
            // hint checkpoint
            String path = match[2].str();
            m_queue->put(path, frames);
        }
    }
    WCTInnerAssert(match.size() == 3); // assert match and match 3.
}

} //namespace WCDB
