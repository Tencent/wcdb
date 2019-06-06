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
#include <WCDB/Master.hpp>
#include <WCDB/Notifier.hpp>
#include <WCDB/Page.hpp>
#include <WCDB/Sequence.hpp>
#include <WCDB/String.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
Backup::Backup(const String &path)
: m_pager(path), Crawlable(m_pager), m_masterCrawler(m_pager)
{
}

#pragma mark - Backup
bool Backup::work()
{
    WCTRemedialAssert(m_readLocker != nullptr && m_writeLocker != nullptr,
                      "Read/Write locker is not avaiable.",
                      return false;);
    m_readLocker->setPath(m_pager.getPath());
    m_writeLocker->setPath(m_pager.getPath());

    bool readLocked = false;
    bool writeLocked = false;
    bool succeed = false;
    do {
        //acquire read lock to avoid shm changed during initialize
        if (!m_writeLocker->acquireWriteLock()) {
            setError(m_writeLocker->getError());
            break;
        }
        writeLocked = true;

        //acquire read lock to avoid wal truncated/restarted during whole iteration of pager
        if (!m_readLocker->acquireReadLock()) {
            setError(m_readLocker->getError());
            break;
        }
        readLocked = true;

        if (!m_pager.initialize()) {
            break;
        }

        if (!m_writeLocker->releaseWriteLock()) {
            setError(m_writeLocker->getError());
            break;
        }
        writeLocked = false;

        m_material.info.pageSize = m_pager.getPageSize();
        m_material.info.reservedBytes = m_pager.getReservedBytes();
        if (m_pager.getNumberOfWalFrames() > 0) {
            m_material.info.walSalt = m_pager.getWalSalt();
            m_material.info.numberOfWalFrames = m_pager.getNumberOfWalFrames();
        }
        succeed = m_masterCrawler.work(this);
    } while (false);
    if (!succeed) {
        Error error(Error::Code::Notice, Error::Level::Notice, "Backup failed hint.");
        error.infos.set(ErrorStringKeySource, ErrorSourceRepair);
        Notifier::shared().notify(error);
        if (Console::debuggable()) {
            m_pager.hint();
        }
        setError(m_pager.getError());
    }

    if (writeLocked && !m_writeLocker->releaseWriteLock() && succeed) {
        setError(m_writeLocker->getError());
        succeed = false;
    }

    if (readLocked && !m_readLocker->releaseReadLock() && succeed) {
        setError(m_readLocker->getError());
        succeed = false;
    }

    m_readLocker->clearPath();
    m_writeLocker->clearPath();

    return succeed;
}

const Material &Backup::getMaterial() const
{
    return m_material;
}

Material::Content &Backup::getOrCreateContent(const String &tableName)
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

bool Backup::filter(const String &tableName)
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
    WCTInnerAssert(false);
}

bool Backup::willCrawlPage(const Page &page, int height)
{
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
        page.number, String::formatted("Unexpected page type: %d", page.getType()));
        return false;
    }
}

void Backup::onCrawlerError()
{
    m_masterCrawler.stop();
}

#pragma mark - MasterCrawlerDelegate
void Backup::onMasterCellCrawled(const Cell &cell, const Master &master)
{
    WCDB_UNUSED(cell)
    if (master.name == Sequence::tableName()) {
        SequenceCrawler(m_pager).work(master.rootpage, this);
    } else if (filter(master.tableName) && !Master::isReservedTableName(master.tableName)
               && !Master::isReservedTableName(master.name)) {
        Material::Content &content = getOrCreateContent(master.tableName);
        if (master.type.isCaseInsensiveEqual("table")
            && master.name.isCaseInsensiveEqual(master.tableName)) {
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
void Backup::onSequenceCellCrawled(const Cell &cell, const Sequence &sequence)
{
    WCDB_UNUSED(cell)
    if (!Master::isReservedTableName(sequence.name) && filter(sequence.name)) {
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
