//
// Created by sanhuazhang on 2019/05/26
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

#include "AutoBackupConfig.hpp"
#include "Assertion.hpp"
#include "Core.hpp"
#include "Factory.hpp"
#include "FileManager.hpp"
#include "InnerHandle.hpp"
#include "Notifier.hpp"
#include "StringView.hpp"

namespace WCDB {

AutoBackupOperator::~AutoBackupOperator() = default;

AutoBackupConfig::AutoBackupConfig(const std::shared_ptr<AutoBackupOperator>& operator_)
: Config(), m_identifier(StringView::formatted("Backup-%p", this)), m_operator(operator_)
{
    WCTAssert(m_operator != nullptr);
}

AutoBackupConfig::~AutoBackupConfig() = default;

bool AutoBackupConfig::invoke(InnerHandle* handle)
{
    AbstractHandle::CheckPointNotification notifination
    = { std::bind(&AutoBackupConfig::onCheckpointBegin,
                  this,
                  std::placeholders::_1,
                  std::placeholders::_2,
                  std::placeholders::_3,
                  std::placeholders::_4,
                  std::placeholders::_5),
        std::bind(&AutoBackupConfig::onCheckpointPage,
                  this,
                  std::placeholders::_1,
                  std::placeholders::_2,
                  std::placeholders::_3),
        std::bind(&AutoBackupConfig::onCheckpointFinish,
                  this,
                  std::placeholders::_1,
                  std::placeholders::_2,
                  std::placeholders::_3,
                  std::placeholders::_4,
                  std::placeholders::_5) };
    handle->setNotificationWhenCheckpointed(m_identifier, notifination);

    return true;
}

bool AutoBackupConfig::uninvoke(InnerHandle* handle)
{
    handle->setNotificationWhenCheckpointed(m_identifier, NullOpt);
    return true;
}

void AutoBackupConfig::tryRegisterIncrementalMaterial(const UnsafeStringView& path,
                                                      SharedIncrementalMaterial material)
{
    LockGuard lock(m_lock);
    m_materials[path] = material;
}

SharedIncrementalMaterial
AutoBackupConfig::tryGetIncrementalMaterial(const UnsafeStringView& path)
{
    LockGuard lock(m_lock);
    auto iter = m_materials.find(path);
    if (iter != m_materials.end()) {
        return iter->second;
    }
    return nullptr;
}

void AutoBackupConfig::onCheckpointBegin(AbstractHandle* handle,
                                         uint32_t nBackFill,
                                         uint32_t mxFrame,
                                         uint32_t salt1,
                                         uint32_t salt2)
{
    WCDB_UNUSED(mxFrame);
    LockGuard lock(m_lock);
    auto& path = handle->getPath();
    auto iter = m_materials.find(path);
    if (iter == m_materials.end()) {
        return;
    }
    auto& material = iter->second;
    auto& curSalt = material->info.currentWalSalt;
    uint32_t& currentNBackFill = material->info.currentNBackFill;
    bool& lastCheckPointFinish = material->info.lastCheckPointFinish;
    m_checkpointPages[path] = Pages();
    if (curSalt.first == salt1 && curSalt.second == salt2 && currentNBackFill == nBackFill) {
        return;
    }
    if (Repair::Wal::isCheckpointIncreasedSalt(curSalt, { salt1, salt2 })
        && nBackFill == 0 && lastCheckPointFinish) {
        curSalt.first = salt1;
        curSalt.second = salt2;
        currentNBackFill = nBackFill;
        lastCheckPointFinish = false;
        return;
    }

    Error error(Error::Code::Error, Error::Level::Warning, "Expired incremental Material");
    error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
    error.infos.insert_or_assign(ErrorStringKeyPath, path);
    error.infos.insert_or_assign(
    "currentSalt", (((uint64_t) curSalt.second) << 32) | curSalt.first);
    error.infos.insert_or_assign("newSalt", (((uint64_t) salt2) << 32) | salt1);
    error.infos.insert_or_assign("currentNBackFill", currentNBackFill);
    error.infos.insert_or_assign("newNBackFill", nBackFill);

    m_materials.erase(path);
    m_checkpointPages.erase(path);
    FileManager::removeItem(Repair::Factory::incrementalMaterialPathForDatabase(path));

    std::thread asyncNotify([=]() {
        Error newError = error;
        Notifier::shared().notify(newError);
    });
    asyncNotify.detach();
}

void AutoBackupConfig::onCheckpointPage(AbstractHandle* handle, uint32_t pageNo, const UnsafeData& data)
{
    if (data.size() == 0) {
        return;
    }
    LockGuard lock(m_lock);
    auto& path = handle->getPath();
    auto iter = m_checkpointPages.find(path);
    if (iter == m_checkpointPages.end()) {
        return;
    }

    Repair::IncrementalMaterial::Page newPage;
    newPage.number = pageNo;
    newPage.type = Repair::Page::convertToPageType(data.buffer()[0]);
    if (newPage.type == Repair::Page::Type::LeafTable) {
        newPage.hash = data.hash();
    } else {
        newPage.hash = 0;
    }
    iter->second[pageNo] = newPage;
}

void AutoBackupConfig::onCheckpointFinish(AbstractHandle* handle,
                                          uint32_t nBackFill,
                                          uint32_t mxFrame,
                                          uint32_t salt1,
                                          uint32_t salt2)
{
    LockGuard lock(m_lock);
    auto& path = handle->getPath();
    auto materialIter = m_materials.find(path);
    if (materialIter == m_materials.end()) {
        m_operator->asyncBackup(path, false);
        return;
    }
    auto& material = materialIter->second;
    WCTAssert((material->info.currentWalSalt.first == salt1
               && material->info.currentWalSalt.second == salt2)
              || (nBackFill == 0
                  && Repair::Wal::isCheckpointIncreasedSalt(
                  material->info.currentWalSalt, { salt1, salt2 })));
    material->info.currentWalSalt = { salt1, salt2 };
    material->info.currentNBackFill = nBackFill;
    material->info.lastCheckPointFinish = nBackFill == mxFrame;
    auto pagesIter = m_checkpointPages.find(path);
    WCTAssert(pagesIter != m_checkpointPages.end());
    if (pagesIter != m_checkpointPages.end()) {
        for (auto& iter : pagesIter->second) {
            material->pages[iter.first] = iter.second;
        }
        if (pagesIter->second.size() > 0) {
            m_operator->asyncBackup(path, true);
        }
    }
}

} //namespace WCDB
