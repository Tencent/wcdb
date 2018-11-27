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
#include <WCDB/CorruptionQueue.hpp>
#include <WCDB/DatabasePool.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/Notifier.hpp>

namespace WCDB {

CorruptionQueue::CorruptionQueue(const String& name,
                                 const std::shared_ptr<DatabasePool>& databasePool)
: AsyncQueue(name), m_databasePool(databasePool)
{
    WCTInnerAssert(m_databasePool != nullptr);
    Notifier::shared()->setNotification(
    0, name, std::bind(&CorruptionQueue::handleError, this, std::placeholders::_1));
}

CorruptionQueue::~CorruptionQueue()
{
    Notifier::shared()->unsetNotification(name);
}

void CorruptionQueue::handleError(const Error& error)
{
    if (!error.isCorruption()) {
        return;
    }
    const auto& infos = error.infos.getStrings();
    auto iter = infos.find("Path");
    if (iter == infos.end()) {
        return;
    }
    auto database = m_databasePool->get(iter->second);
    if (database == nullptr || !database->containsRecoverScheme()) {
        return;
    }
    bool succeed;
    uint32_t identifier;
    std::tie(succeed, identifier) = FileManager::getFileIdentifier(iter->second);
    if (!succeed) {
        return;
    }
    bool notify = false;
    {
        std::lock_guard<std::mutex> lockGuard(m_mutex);
        notify = m_corrupted.empty();
        m_corrupted[iter->second] = identifier;
    }
    lazyRun();
    if (notify) {
        m_cond.notify_all();
    }
}

void CorruptionQueue::loop()
{
    while (!exit()) {
        String path;
        uint32_t corruptedIdentifier;
        {
            std::unique_lock<std::mutex> lockGuard(m_mutex);
            if (m_corrupted.empty()) {
                m_cond.wait(lockGuard);
                continue;
            }
            auto iter = m_corrupted.begin();
            path = iter->first;
            corruptedIdentifier = iter->second;
        }
        auto database = m_databasePool->get(path);
        if (database != nullptr) {
            database->blockade();
            do {
                bool succeed;
                uint32_t identifier;
                std::tie(succeed, identifier) = FileManager::getFileIdentifier(path);
                if (!succeed) {
                    break;
                }
                if (identifier != corruptedIdentifier) {
                    break;
                }
                database->recover();
            } while (false);
            database->unblockade();
        }
        std::lock_guard<std::mutex> lockGuard(m_mutex);
        m_corrupted.erase(path);
    }
}

} //namespace WCDB
