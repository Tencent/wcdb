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
#include <WCDB/AutoBackupConfig.hpp>
#include <WCDB/Handle.hpp>
#include <WCDB/String.hpp>

namespace WCDB {

AutoBackupOperator::~AutoBackupOperator()
{
}

AutoBackupConfig::AutoBackupConfig(const std::shared_ptr<AutoBackupOperator> &operator_)
: Config(), m_identifier(String::formatted("Backup-%p", this)), m_operator(operator_)
{
    WCTInnerAssert(m_operator != nullptr);
}

bool AutoBackupConfig::invoke(Handle *handle)
{
    handle->setNotificationWhenCheckpointed(
    m_identifier,
    std::bind(&AutoBackupConfig::onCheckpointed, this, std::placeholders::_1));

    return true;
}

bool AutoBackupConfig::uninvoke(Handle *handle)
{
    handle->setNotificationWhenCheckpointed(m_identifier, nullptr);

    return true;
}

void AutoBackupConfig::onCheckpointed(const String &path)
{
    m_operator->asyncBackup(path);
}

} //namespace WCDB
