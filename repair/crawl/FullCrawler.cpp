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

#include <WCDB/Assembler.hpp>
#include <WCDB/Assertion.hpp>
#include <WCDB/FullCrawler.hpp>
#include <WCDB/Master.hpp>
#include <WCDB/Page.hpp>
#include <WCDB/Sequence.hpp>
#include <WCDB/SequenceCrawler.hpp>
#include <WCDB/String.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
FullCrawler::FullCrawler(const std::string &source)
: Repairman(source), m_sequenceCrawler(m_pager), m_masterCrawler(m_pager)
{
}

#pragma mark - Repair
bool FullCrawler::work()
{
    if (isEmptyDatabase()) {
        return exit(true);
    }

    if (!m_pager.initialize()) {
        if (m_pager.getError().isCorruption()) {
            tryUpgradeCrawlerError();
            return exit(true);
        } else {
            setCriticalError(m_pager.getError());
            return exit(false);
        }
    }

    //calculate score
    int leafTablePageCount = 0;
    for (int i = 1; i <= m_pager.getPageCount(); ++i) {
        Page page(i, &m_pager);
        auto pair = page.acquireType();
        if (!pair.first // treat as leaf table
            || pair.second == Page::Type::LeafTable) {
            ++leafTablePageCount;
        }
    }
    if (leafTablePageCount == 0) {
        return exit(true);
    }
    setPageWeight(Fraction(1, leafTablePageCount));

    if (markAsAssembling()) {
        m_masterCrawler.work(this);
        for (const auto &element : m_associatedSQLs) {
            assembleAssociatedSQLs(element.second);
        }
        markAsAssembled();
    }

    return exit();
}

#pragma mark - Crawlable
void FullCrawler::onCellCrawled(const Cell &cell)
{
    if (isErrorCritial()) {
        return;
    }
    assembleCell(cell);
}

bool FullCrawler::willCrawlPage(const Page &page, int)
{
    if (isErrorCritial()) {
        return false;
    }
    markPageAsCounted(page);
    increaseProgress(getPageWeight().value());
    return true;
}

#pragma mark - Error
void FullCrawler::onErrorCritical()
{
    m_masterCrawler.stop();
    m_sequenceCrawler.stop();
    Repairman::onErrorCritical();
}

#pragma mark - MasterCrawlerDelegate
void FullCrawler::onMasterPageCrawled(const Page &page)
{
    increaseProgress(getPageWeight().value());
    markPageAsCounted(page);
}

void FullCrawler::onMasterCellCrawled(const Cell &cell, const Master &master)
{
    if (isErrorCritial()) {
        return;
    }
    markCellAsCounted(cell);
    if (master.name == Sequence::tableName()) {
        WCTInnerAssert(String::isCaseInsensiveEqual(master.type, "table"));
        WCTInnerAssert(String::isCaseInsensiveEqual(master.tableName, master.name));
        m_sequenceCrawler.work(master.rootpage, this);
    } else if (Master::isReservedTableName(master.name)
               || Master::isReservedTableName(master.tableName)) {
        //Skip sqlite reserved table
        return;
    } else {
        if (String::isCaseInsensiveEqual(master.type, "table")) {
            WCTInnerAssert(String::isCaseInsensiveEqual(master.tableName, master.name));
            if (assembleTable(master.name, master.sql)) {
                crawl(master.rootpage);
            }
        } else {
            m_associatedSQLs[master.tableName].push_back(master.sql);
        }
    }
}

void FullCrawler::onMasterCrawlerError()
{
    tryUpgradeCrawlerError();
}

#pragma mark - SequenceCrawlerDelegate
void FullCrawler::onSequencePageCrawled(const Page &page)
{
    if (isErrorCritial()) {
        return;
    }
    increaseProgress(getPageWeight().value());
    markPageAsCounted(page);
}

void FullCrawler::onSequenceCellCrawled(const Cell &cell, const Sequence &sequence)
{
    if (isErrorCritial()) {
        return;
    }
    if (assembleSequence(sequence.name, sequence.seq)) {
        markCellAsCounted(cell);
    }
}

void FullCrawler::onSequenceCrawlerError()
{
    tryUpgradeCrawlerError();
}

} //namespace Repair

} //namespace WCDB
