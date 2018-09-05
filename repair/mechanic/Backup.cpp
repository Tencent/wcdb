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
#include <WCDB/FileManager.hpp>
#include <WCDB/Master.hpp>
#include <WCDB/Page.hpp>
#include <WCDB/Sequence.hpp>
#include <WCDB/String.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
Backup::Backup(const std::string &path)
: m_pager(path), Crawlable(m_pager), m_masterCrawler(m_pager)
{
}

#pragma mark - Backup
bool Backup::work(int maxWalFrame)
{
    WCTInnerAssert(m_locker != nullptr);
    WCTInnerAssert(m_locker->getPath().empty());
    m_locker->setPath(m_pager.getPath());

    bool locked = false;
    bool succeed = false;
    do {
        //acquire read lock to avoid wal truncated/restarted during whole iteration of pager
        if (!m_locker->acquireLock()) {
            setError(m_locker->getError());
            break;
        }
        locked = true;

        m_pager.setMaxWalFrame(maxWalFrame);
        if (!m_pager.initialize()) {
            break;
        }

        m_material.info.pageSize = m_pager.getPageSize();
        m_material.info.reservedBytes = m_pager.getReservedBytes();
        if (m_pager.getWalFrameCount() > 0) {
            m_material.info.walSalt = m_pager.getWalSalt();
            m_material.info.walFrame = m_pager.getWalFrameCount();
        }
        succeed = m_masterCrawler.work(this);
    } while (false);
    if (!succeed) {
        setError(m_pager.getError());
    }

    if (locked && !m_locker->releaseLock() && succeed) {
        setError(m_locker->getError());
        succeed = false;
    }

    return succeed;
}

const Material &Backup::getMaterial() const
{
    return m_material;
}

Material::Content &Backup::getOrCreateContent(const std::string &tableName)
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

bool Backup::filter(const std::string &tableName)
{
    if (m_filter) {
        return m_filter(tableName);
    }
    return true;
}

#pragma mark - Crawlable
void Backup::onCellCrawled(const Cell &cell)
{
    WCTInnerFatalError();
}

bool Backup::willCrawlPage(const Page &page, int height)
{
    switch (page.getType()) {
    case Page::Type::LeafTable: {
        auto iter = m_verifiedPagenos.find(page.number);
        if (iter != m_verifiedPagenos.end()) {
            markAsCorrupted(page.number, "Page is already crawled.");
        } else {
            m_verifiedPagenos[page.number] = page.getData().hash();
        }
        return false;
    }
    case Page::Type::InteriorTable:
        return true;
    default:
        break;
    }
    return false;
}

void Backup::onCrawlerError()
{
    m_masterCrawler.stop();
}

#pragma mark - MasterCrawlerDelegate
void Backup::onMasterCellCrawled(const Cell &cell, const Master &master)
{
    if (master.name == Sequence::tableName()) {
        SequenceCrawler(m_pager).work(master.rootpage, this);
    } else if (filter(master.tableName) && !Master::isReservedTableName(master.tableName)
               && !Master::isReservedTableName(master.name)) {
        Material::Content &content = getOrCreateContent(master.tableName);
        if (String::isCaseInsensiveEqual(master.type, "table")
            && String::isCaseInsensiveEqual(master.name, master.tableName)) {
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