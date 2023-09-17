//
// Created by sanhuazhang on 2018/06/05
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

#include "Backup.hpp"
#include "Assertion.hpp"
#include "Cell.hpp"
#include "CoreConst.h"
#include "Factory.hpp"
#include "FileManager.hpp"
#include "MasterItem.hpp"
#include "Notifier.hpp"
#include "Page.hpp"
#include "SequenceItem.hpp"
#include "StringView.hpp"
#include "SyntaxCommonConst.hpp"

namespace WCDB {

namespace Repair {

BackupBaseDelegate::~BackupBaseDelegate() = default;

BackupSharedDelegate::~BackupSharedDelegate() = default;

BackupExclusiveDelegate::BackupExclusiveDelegate() : m_suspend(false)
{
}

BackupExclusiveDelegate::~BackupExclusiveDelegate() = default;

void BackupExclusiveDelegate::suspendBackup(bool suspend)
{
    m_suspend = suspend;
}

bool BackupExclusiveDelegate::backupSuspended() const
{
    return m_suspend;
}

BackupDelegateHolder::BackupDelegateHolder() = default;

BackupDelegateHolder::~BackupDelegateHolder() = default;

void BackupDelegateHolder::setBackupSharedDelegate(BackupSharedDelegate *delegate)
{
    WCTAssert(delegate != nullptr);
    m_sharedDelegate = delegate;
}

void BackupDelegateHolder::setBackupExclusiveDelegate(BackupExclusiveDelegate *delegate)
{
    WCTAssert(delegate != nullptr);
    m_exclusiveDelegate = delegate;
}

bool BackupDelegateHolder::isDelegateValid() const
{
    return m_sharedDelegate != nullptr && m_exclusiveDelegate != nullptr
           && (uintptr_t) m_sharedDelegate != (uintptr_t) m_exclusiveDelegate;
}

#pragma mark - Initialize
Backup::Backup(const UnsafeStringView &path)
: Crawlable(), m_pager(path), m_incrementalMaterial(nullptr), m_masterCrawler()
{
    setAssociatedPager(&m_pager);
    m_masterCrawler.setAssociatedPager(&m_pager);
}

Backup::~Backup() = default;

#pragma mark - Backup
bool Backup::work(SharedIncrementalMaterial incrementalMaterial)
{
    WCTRemedialAssert(isDelegateValid(), "Read/Write locker is not avaiable.", return false;);

    auto materialLoad = tryLoadLatestMaterial(incrementalMaterial);
    if (!materialLoad.hasValue()) {
        return false;
    }
    if (!materialLoad.value()) {
        if (!fullBackup()) {
            return false;
        }
        m_incrementalMaterial = incrementalMaterial;
    } else {
        m_incrementalMaterial = incrementalMaterial;
        if (!incrementalBackup()) {
            return false;
        }
    }

    updateMaterial(materialLoad.value());

    return true;
}

Optional<bool> Backup::tryLoadLatestMaterial(SharedIncrementalMaterial incrementalMaterial)
{
    if (incrementalMaterial == nullptr
        || incrementalMaterial->info.incrementalBackupTimes >= BackupMaxIncrementalTimes) {
        return false;
    }
    auto materialPath = Factory::latestMaterialForDatabase(m_pager.getPath());
    if (materialPath.failed()) {
        return NullOpt;
    }
    if (materialPath->empty()) {
        return false;
    }
    m_cipherDelegate->openCipherInMemory();
    bool useMaterial = false;
    if (m_cipherDelegate->isCipherDB()) {
        m_material.setCipherDelegate(m_cipherDelegate);
        useMaterial = m_material.decryptedDeserialize(materialPath.value());
    } else {
        useMaterial = m_material.deserialize(materialPath.value());
    }
    if (!useMaterial) {
        m_material = Material();
        return NullOpt;
    }
    if (m_material.info.walSalt != incrementalMaterial->info.lastWalSalt
        || m_material.info.nBackFill != incrementalMaterial->info.lastNBackFill) {
        m_material = Material();
        return false;
    }
    return true;
}

bool Backup::fullBackup()
{
    bool exclusive = false;
    bool succeed = false;
    do {
        //acquire write lock to avoid shm changed during initialize
        if (!m_exclusiveDelegate->acquireBackupExclusiveLock()) {
            setError(m_exclusiveDelegate->getBackupError());
            break;
        }
        exclusive = true;

        if (m_cipherDelegate->isCipherDB()) {
            size_t pageSize = m_cipherDelegate->getCipherPageSize();
            if (pageSize == 0) {
                setError(m_cipherDelegate->getCipherError());
                break;
            }
            void *pCodec = m_cipherDelegate->getCipherContext();
            m_pager.setCipherContext(pCodec);
            m_pager.setPageSize((int) pageSize);
        }

        if (!m_pager.initialize()) {
            setError(m_pager.getError());
            break;
        }

        if (!m_exclusiveDelegate->releaseBackupExclusiveLock()) {
            setError(m_exclusiveDelegate->getBackupError());
            break;
        }
        exclusive = false;

        m_material.setCipherDelegate(m_cipherDelegate);
        succeed = m_masterCrawler.work(this);
        if (!succeed) {
            setError(m_pager.getError());
        }
        if (m_material.info.seqTableRootPage == Material::UnknownPageNo) {
            m_material.info.seqTableRootPage = 0;
        }
    } while (false);

    if (exclusive && !m_exclusiveDelegate->releaseBackupExclusiveLock() && succeed) {
        setError(m_exclusiveDelegate->getBackupError());
        succeed = false;
    }

    return succeed;
}

bool Backup::incrementalBackup()
{
    if (m_cipherDelegate->isCipherDB()) {
        size_t pageSize = m_cipherDelegate->getCipherPageSize();
        if (pageSize == 0) {
            setError(m_cipherDelegate->getCipherError());
            return false;
        }
        void *pCodec = m_cipherDelegate->getCipherContext();
        m_pager.setCipherContext(pCodec);
        m_pager.setPageSize((int) pageSize);
    }
    m_pager.setWalSkipped();
    if (!m_pager.initialize()) {
        setError(m_pager.getError());
        return false;
    }
    m_verifyingPagenos = m_incrementalMaterial->pages;
    int schemaCookie = m_pager.getSchemaCookie();
    if (schemaCookie != m_incrementalMaterial->info.lastSchemaCookie
        || m_material.info.seqTableRootPage == Material::UnknownPageNo) {
        if (!loadWal()) {
            setError(m_pager.getError());
            return false;
        }
        if (!m_masterCrawler.work(this)) {
            setError(m_pager.getError());
            return false;
        }
        if (m_material.info.seqTableRootPage == Material::UnknownPageNo) {
            m_material.info.seqTableRootPage = 0;
        }
        m_pager.disposeWal();
        for (auto iter = m_material.contentsList.begin();
             iter != m_material.contentsList.end();) {
            auto &content = **iter;
            if (content.checked) {
                iter++;
            } else {
                // Deleted table
                iter = m_material.contentsList.erase(iter);
                m_material.contentsMap.erase(content.tableName);
            }
        }
    } else if (m_material.info.seqTableRootPage != 0
               && m_material.info.seqTableRootPage != Material::UnknownPageNo) {
        SequenceCrawler crawler;
        crawler.setAssociatedPager(&m_pager);
        if (!crawler.work(m_material.info.seqTableRootPage, this)) {
            setError(m_pager.getError());
            return false;
        }
    }
    if (m_verifyingPagenos.size() == 0) {
        return true;
    }
    auto &contentList = m_material.contentsList;
    for (auto &content : contentList) {
        auto &pages = content->verifiedPagenos;
        for (auto page = pages.begin(); page != pages.end();) {
            if (m_verifyingPagenos.find(page->first) != m_verifyingPagenos.end()) {
                page = pages.erase(page);
            } else {
                m_unchangedLeaves.insert(page->first);
                page++;
            }
        }
    }
    for (auto iter = m_verifyingPagenos.begin(); iter != m_verifyingPagenos.end();) {
        auto &page = iter->second;
        if (page.type != Page::Type::LeafTable) {
            iter = m_verifyingPagenos.erase(iter);
        } else {
            iter++;
        }
    }
    if (m_verifyingPagenos.size() == 0) {
        return true;
    }
    for (auto iter = contentList.begin(); iter != contentList.end();) {
        WCTAssert((*iter)->rootPage > 1 && (*iter)->rootPage != Material::UnknownPageNo);
        if (!crawl((*iter)->rootPage)) {
            setError(m_pager.getError());
            return false;
        }
        if (m_verifiedPagenos.size() > 0) {
            (*iter)->verifiedPagenos.insert(m_verifiedPagenos.begin(),
                                            m_verifiedPagenos.end());
            m_verifiedPagenos.clear();
            contentList.insert(contentList.begin(), *iter);
            iter = contentList.erase(iter);
        } else {
            iter++;
        }
        if (m_verifyingPagenos.size() == 0) {
            return true;
        }
    }
    return true;
}

bool Backup::loadWal()
{
    bool exclusive = false;
    bool succeed = false;
    do {
        //acquire write lock to avoid shm changed during initialize
        if (!m_exclusiveDelegate->acquireBackupExclusiveLock()) {
            setError(m_exclusiveDelegate->getBackupError());
            break;
        }
        exclusive = true;

        WCTAssert(m_incrementalMaterial != nullptr);
        m_pager.setNBackFill(m_incrementalMaterial->info.currentNBackFill);
        m_pager.setWalSalt(m_incrementalMaterial->info.currentWalSalt);
        succeed = m_pager.loadWal();
    } while (false);

    if (exclusive && !m_exclusiveDelegate->releaseBackupExclusiveLock() && succeed) {
        setError(m_exclusiveDelegate->getBackupError());
        succeed = false;
    }
    return succeed;
}

const Material &Backup::getMaterial() const
{
    return m_material;
}

SharedIncrementalMaterial Backup::getIncrementalMaterial()
{
    return m_incrementalMaterial;
}

void Backup::updateMaterial(bool isIncremental)
{
    auto &info = m_material.info;

    if (m_incrementalMaterial != nullptr) {
        info.walSalt = m_incrementalMaterial->info.currentWalSalt;
        info.nBackFill = m_incrementalMaterial->info.currentNBackFill;
    } else {
        m_incrementalMaterial = std::make_shared<Repair::IncrementalMaterial>();
        info.walSalt = m_pager.getWalSalt();
        info.nBackFill = m_pager.getNBackFill();
        m_incrementalMaterial->info.lastCheckPointFinish
        = info.nBackFill == m_pager.getMaxFrame();
    }

    auto &incrementalInfo = m_incrementalMaterial->info;

    m_material.info.pageSize = m_pager.getPageSize();
    m_material.info.reservedBytes = m_pager.getReservedBytes();
    incrementalInfo.lastWalSalt = info.walSalt;
    incrementalInfo.lastNBackFill = info.nBackFill;
    incrementalInfo.currentWalSalt = info.walSalt;
    incrementalInfo.currentNBackFill = info.nBackFill;
    incrementalInfo.lastSchemaCookie = m_pager.getSchemaCookie();
    incrementalInfo.lastBackupTime = (uint32_t) Time::now().seconds();
    m_incrementalMaterial->pages.clear();
    if (!isIncremental) {
        incrementalInfo.incrementalBackupTimes = 0;
    } else {
        incrementalInfo.incrementalBackupTimes++;
    }
}

Material::Content &Backup::getOrCreateContent(const UnsafeStringView &tableName)
{
    auto &contentsMap = m_material.contentsMap;
    auto iter = contentsMap.find(tableName);
    if (iter == contentsMap.end()) {
        iter = contentsMap.emplace(tableName, Material::Content()).first;
        m_material.contentsList.push_back(&(iter->second));
    }
    iter->second.tableName = tableName;
    return iter->second;
}

#pragma mark - Filter
void Backup::filter(const Filter &tableShouldBeBackedUp)
{
    m_filter = tableShouldBeBackedUp;
}

bool Backup::filter(const UnsafeStringView &tableName)
{
    bool result = true;
    if (m_filter != nullptr) {
        result = m_filter(tableName);
    }
    return result;
}

#pragma mark - Crawlable
bool Backup::canCrawlPage(uint32_t pageno)
{
    if (m_unchangedLeaves.find(pageno) != m_unchangedLeaves.end()) {
        return false;
    }
    auto iter = m_verifyingPagenos.find(pageno);
    if (iter != m_verifyingPagenos.end()) {
        m_verifiedPagenos[iter->first] = iter->second.hash;
        m_verifyingPagenos.erase(iter);
        if (m_verifyingPagenos.size() == 0) {
            suspend();
        }
        return false;
    }
    return true;
}

void Backup::onCellCrawled(const Cell &cell)
{
    WCDB_UNUSED(cell)
    WCTAssert(false);
}

bool Backup::willCrawlPage(const Page &page, int height)
{
    if (m_exclusiveDelegate->backupSuspended()) {
        markAsInterrupted();
        return false;
    }
    WCDB_UNUSED(height)
    switch (page.getType()) {
    case Page::Type::InteriorTable:
        return true;
    case Page::Type::LeafTable: {
        WCTAssert(m_unchangedLeaves.size() == 0);
        bool emplaced
        = m_verifiedPagenos.emplace(page.number, page.getData().hash()).second;
        if (!emplaced) {
            markAsCorrupted(page.number, "Page is already crawled.");
        }
        return false;
    }
    case Page::Type::InteriorIndex:
    case Page::Type::LeafIndex:
        return false;
    default:
        markAsCorrupted(
        page.number, StringView::formatted("Unexpected page type: %d", page.getType()));
        return false;
    }
}

void Backup::onCrawlerError()
{
    m_masterCrawler.suspend();
    suspend();
}

#pragma mark - MasterCrawlerDelegate
void Backup::onMasterPageCrawled(const Page &page)
{
    m_verifyingPagenos.erase(page.number);
}

void Backup::onMasterCellCrawled(const Cell &cell, const MasterItem &master)
{
    WCDB_UNUSED(cell)
    if (master.name == Syntax::sequenceTable) {
        m_material.info.seqTableRootPage = master.rootpage;
        SequenceCrawler crawler;
        crawler.setAssociatedPager(&m_pager);
        crawler.work(master.rootpage, this);
    } else if (filter(master.tableName)
               && !MasterItem::isReservedTableName(master.tableName)
               && !MasterItem::isReservedTableName(master.name)) {
        Material::Content &content = getOrCreateContent(master.tableName);
        if (master.type.caseInsensitiveEqual("table")
            && master.name.caseInsensitiveEqual(master.tableName)) {
            content.sql = master.sql;
            content.rootPage = master.rootpage;
            content.checked = true;
            if (m_incrementalMaterial != nullptr) {
                return;
            }
            if (!crawl(master.rootpage)) {
                return;
            }
            content.verifiedPagenos = std::move(m_verifiedPagenos);
        } else {
            if (!master.sql.empty()) {
                content.associatedSQLs.push_back(master.sql);
            }
        }
    }
}

void Backup::onMasterCrawlerError()
{
    markAsError();
}

#pragma mark - SequenceCrawlerDelegate
void Backup::onSequencePageCrawled(const Page &page)
{
    m_verifyingPagenos.erase(page.number);
}

void Backup::onSequenceCellCrawled(const Cell &cell, const SequenceItem &sequence)
{
    WCDB_UNUSED(cell)
    if (sequence.seq == 0) {
        return;
    }
    if (!MasterItem::isReservedTableName(sequence.name) && filter(sequence.name)) {
        Material::Content &content = getOrCreateContent(sequence.name);
        //the columns in sqlite_sequence are not unique.
        content.sequence = std::max(content.sequence, sequence.seq);
    }
}

void Backup::onSequenceCrawlerError()
{
    markAsError();
}

} //namespace Repair

} //namespace WCDB
