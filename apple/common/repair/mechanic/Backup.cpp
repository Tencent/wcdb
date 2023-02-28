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

#include <WCDB/Assertion.hpp>
#include <WCDB/Backup.hpp>
#include <WCDB/Cell.hpp>
#include <WCDB/CoreConst.h>
#include <WCDB/FileManager.hpp>
#include <WCDB/MasterItem.hpp>
#include <WCDB/Notifier.hpp>
#include <WCDB/Page.hpp>
#include <WCDB/SequenceItem.hpp>
#include <WCDB/StringView.hpp>
#include <WCDB/SyntaxCommonConst.hpp>

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
: Crawlable(), m_pager(path), m_masterCrawler()
{
    setAssociatedPager(&m_pager);
    m_masterCrawler.setAssociatedPager(&m_pager);
}

Backup::~Backup() = default;

#pragma mark - Backup
bool Backup::work()
{
    WCTRemedialAssert(isDelegateValid(), "Read/Write locker is not avaiable.", return false;);

    m_sharedDelegate->setBackupPath(m_pager.getPath());
    m_exclusiveDelegate->setBackupPath(m_pager.getPath());

    bool shared = false;
    bool exclusive = false;
    bool succeed = false;
    do {
        //acquire read lock to avoid shm changed during initialize
        if (!m_exclusiveDelegate->acquireBackupExclusiveLock()) {
            setError(m_exclusiveDelegate->getBackupError());
            break;
        }
        exclusive = true;

        //acquire read lock to avoid wal truncated/restarted during whole iteration of pager
        if (!m_sharedDelegate->acquireBackupSharedLock()) {
            setError(m_sharedDelegate->getBackupError());
            break;
        }
        shared = true;

        if (m_sharedDelegate->isCipherDB()) {
            size_t pageSize = m_sharedDelegate->getCipherPageSize();
            if (pageSize == 0) {
                setError(m_sharedDelegate->getBackupError());
                break;
            }
            void *pCodec = m_sharedDelegate->getCipherContext();
            m_pager.setCipherContext(pCodec);
            m_pager.setPageSize((int) pageSize);
        }

        if (!m_pager.initialize()) {
            break;
        }

        if (!m_exclusiveDelegate->releaseBackupExclusiveLock()) {
            setError(m_exclusiveDelegate->getBackupError());
            break;
        }
        exclusive = false;

        m_material.setCipherDelegate(m_cipherDelegate);
        m_material.info.pageSize = m_pager.getPageSize();
        m_material.info.reservedBytes = m_pager.getReservedBytes();
        if (m_pager.getNumberOfWalFrames() > 0) {
            m_material.info.walSalt = m_pager.getWalSalt();
            m_material.info.numberOfWalFrames = m_pager.getNumberOfWalFrames();
        }
        succeed = m_masterCrawler.work(this);
    } while (false);

    if (exclusive && !m_exclusiveDelegate->releaseBackupExclusiveLock() && succeed) {
        setError(m_exclusiveDelegate->getBackupError());
        succeed = false;
    }

    if (shared && !m_sharedDelegate->releaseBackupSharedLock() && succeed) {
        setError(m_sharedDelegate->getBackupError());
        succeed = false;
    }

    return succeed;
}

const Material &Backup::getMaterial() const
{
    return m_material;
}

Material::Content &Backup::getOrCreateContent(const UnsafeStringView &tableName)
{
    auto &contents = m_material.contents;
    auto iter = contents.find(tableName);
    if (iter == contents.end()) {
        iter = contents.emplace(tableName, Material::Content()).first;
    }
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
        bool emplaced
        = m_verifiedPagenos.emplace(page.number, page.getData().hash()).second;
        if (!emplaced) {
            markAsCorrupted(page.number, "Page is already crawled.");
        }
        return false;
    }
    default:
        markAsCorrupted(
        page.number, StringView::formatted("Unexpected page type: %d", page.getType()));
        return false;
    }
}

void Backup::onCrawlerError()
{
    m_masterCrawler.suspend();
}

#pragma mark - MasterCrawlerDelegate
void Backup::onMasterCellCrawled(const Cell &cell, const MasterItem &master)
{
    WCDB_UNUSED(cell)
    if (master.name == Syntax::sequenceTable) {
        SequenceCrawler crawler;
        crawler.setAssociatedPager(&m_pager);
        crawler.work(master.rootpage, this);
    } else if (filter(master.tableName)
               && !MasterItem::isReservedTableName(master.tableName)
               && !MasterItem::isReservedTableName(master.name)) {
        Material::Content &content = getOrCreateContent(master.tableName);
        if (master.type.caseInsensiveEqual("table")
            && master.name.caseInsensiveEqual(master.tableName)) {
            if (!crawl(master.rootpage)) {
                return;
            }
            content.verifiedPagenos = std::move(m_verifiedPagenos);
            content.sql = master.sql;
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
