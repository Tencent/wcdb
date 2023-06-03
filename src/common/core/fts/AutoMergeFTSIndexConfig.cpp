//
// Created by qiuwenchen(qiuwenchen) on 2020/10/22.
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

#include "AutoMergeFTSIndexConfig.hpp"
#include "Assertion.hpp"
#include "InnerHandle.hpp"
#include "StringView.hpp"

namespace WCDB {

AutoMergeFTSIndexOperator::~AutoMergeFTSIndexOperator() = default;

AutoMergeFTSIndexConfig::AutoMergeFTSIndexConfig(const std::shared_ptr<AutoMergeFTSIndexOperator> &operator_)
: Config(), m_identifier(StringView::formatted("Merge-%p", this)), m_operator(operator_)
{
    WCTAssert(m_operator != nullptr);
}

AutoMergeFTSIndexConfig::~AutoMergeFTSIndexConfig() = default;

bool AutoMergeFTSIndexConfig::invoke(InnerHandle *handle)
{
    handle->setNotificationWhenTableModified(m_identifier,
                                             std::bind(&AutoMergeFTSIndexConfig::onTableModified,
                                                       this,
                                                       std::placeholders::_1,
                                                       std::placeholders::_2,
                                                       std::placeholders::_3));
    handle->setNotificationWhenCommitted(
    1,
    m_identifier,
    std::bind(&AutoMergeFTSIndexConfig::onCommitted, this, std::placeholders::_1, std::placeholders::_2));

    return true;
}

bool AutoMergeFTSIndexConfig::uninvoke(InnerHandle *handle)
{
    handle->unsetNotificationWhenCommitted(m_identifier);
    handle->setNotificationWhenTableModified(m_identifier, nullptr);

    return true;
}

void AutoMergeFTSIndexConfig::onTableModified(const UnsafeStringView &path,
                                              const UnsafeStringView &newTable,
                                              const UnsafeStringView &modifiedTable)
{
    WCTAssert(!path.empty());
    LockGuard lockGuard(m_lock);
    if (!newTable.empty()) {
        m_newTables[path].emplace(StringView(newTable));
    }
    if (!modifiedTable.empty()) {
        m_modifiedTables[path].emplace(StringView(modifiedTable));
    }
}

bool AutoMergeFTSIndexConfig::onCommitted(const UnsafeStringView &path, int pages)
{
    WCTAssert(!path.empty());
    LockGuard lockGuard(m_lock);
    if (pages == 0) {
        m_newTables.erase(path);
        m_modifiedTables.erase(path);
        return true;
    }
    auto newTablesIter = m_newTables.find(path);
    TableArray newTables = nullptr;
    if (newTablesIter != m_newTables.end()) {
        newTables = TableArray(new std::vector<StringView>());
        newTables->assign(newTablesIter->second.begin(), newTablesIter->second.end());
        m_newTables.erase(path);
    }

    auto modifiedTablesIter = m_modifiedTables.find(path);
    TableArray modifiedTables = nullptr;
    if (modifiedTablesIter != m_modifiedTables.end()) {
        modifiedTables = TableArray(new std::vector<StringView>());
        modifiedTables->assign(modifiedTablesIter->second.begin(),
                               modifiedTablesIter->second.end());
        m_modifiedTables.erase(path);
    }

    if (newTables != nullptr || modifiedTables != nullptr) {
        m_operator->asyncMergeFTSIndex(path, newTables, modifiedTables);
    }
    return true;
}

} //namespace WCDB
