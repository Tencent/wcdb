//
// Created by sanhuazhang on 2019/05/27
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

#include <WCDB/Assertion.hpp>
#include <WCDB/AutoCheckpointConfig.hpp>
#include <WCDB/Global.hpp>
#include <WCDB/InnerHandle.hpp>
#include <WCDB/StringView.hpp>
#include <regex>

namespace WCDB {

AutoCheckpointOperator::~AutoCheckpointOperator() = default;

AutoCheckpointConfig::AutoCheckpointConfig(const std::shared_ptr<AutoCheckpointOperator>& operator_)
: Config(), m_identifier(StringView::formatted("Checkpoint-%p", this)), m_operator(operator_)
{
    WCTAssert(m_operator != nullptr);

    Global::shared().setNotificationForLog(
    m_identifier,
    std::bind(&AutoCheckpointConfig::log, this, std::placeholders::_1, std::placeholders::_2));
}

AutoCheckpointConfig::~AutoCheckpointConfig()
{
    Global::shared().setNotificationForLog(m_identifier, nullptr);
}

bool AutoCheckpointConfig::invoke(InnerHandle* handle)
{
    static_assert(SQLITE_DEFAULT_WAL_AUTOCHECKPOINT == 0, "");

    handle->setNotificationWhenCommitted(
    0,
    m_identifier,
    std::bind(&AutoCheckpointConfig::onCommitted, this, std::placeholders::_1, std::placeholders::_2));

    return true;
}

bool AutoCheckpointConfig::uninvoke(InnerHandle* handle)
{
    handle->unsetNotificationWhenCommitted(m_identifier);

    return true;
}

bool AutoCheckpointConfig::onCommitted(const UnsafeStringView& path, int frames)
{
    WCDB_UNUSED(frames);
    if (frames > 0) {
        m_operator->asyncCheckpoint(path);
    }
    return true;
}

void AutoCheckpointConfig::log(int rc, const char* message)
{
    Error::ExtCode extCode = Error::rc2ec(rc);
    if (extCode != Error::ExtCode::NoticeRecoverWal || message == nullptr) {
        return;
    }
    std::regex pattern("recovered (\\w+) frames from WAL file (.+)\\-wal");
    const std::string source = message;
    std::smatch match;
    if (std::regex_search(source.begin(), source.end(), match, pattern)) {
        int frames = atoi(match[1].str().c_str());
        if (frames > 0) {
            // hint checkpoint
            if (frames > 0) {
                StringView path(match[2].str());
                m_operator->asyncCheckpoint(path);
            }
        }
    }
    WCTAssert(match.size() == 3); // assert match and match 3.
}

} //namespace WCDB
