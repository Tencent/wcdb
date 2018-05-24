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
#include <WCDB/Crawler.hpp>
#include <WCDB/Page.hpp>
#include <WCDB/ThreadedErrors.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
Crawler::Crawler(const std::string &source)
    : MasterCrawler(source, false)
    , m_columnWeightForCurrentPage(0)
    , m_parsedLeafPageCount(0)
{
}

#pragma mark - Repair
void Crawler::work()
{
    WCTInnerAssert(m_assembler != nullptr);

    if (m_pager.initialize()) {
        MasterCrawler::work();

        for (const auto &element : getMasters()) {
            const Master &master = element.second;
            if (master.tableName.empty() || master.sql.empty()) {
                continue;
            }
            m_assembler->markAsAssembling(element.second.tableName);
            if (m_assembler->assembleTable(master.sql) &&
                m_assembler->assembleTableAssociated(master.associatedSQLs)) {
                crawl(master.rootpage);
            } else {
                tryUpgradeError(
                    std::move(ThreadedErrors::shared()->moveThreadedError()));
            }
            m_assembler->markAsAssembled();
        }
    } else {
        tryUpgradeError(
            std::move(ThreadedErrors::shared()->moveThreadedError()));
    }

    if (m_score != 0 && m_parsedLeafPageCount != 0) {
        //calculate score
        int leafTablePageCount = 0;
        for (int i = 0; i < m_pager.getPageCount(); ++i) {
            Page page(i, m_pager);
            auto pair = page.acquireType();
            if (!pair.first // treat as leaf table
                || pair.second == Page::Type::LeafTable) {
                ++leafTablePageCount;
            }
        }
        m_score = m_score * m_parsedLeafPageCount / leafTablePageCount;
    }
}

#pragma mark - Crawlable
bool Crawler::onCellCrawled(const Cell &cell)
{
    if (isMasterCrawling()) {
        if (MasterCrawler::onCellCrawled(cell)) {
            m_score += m_columnWeightForCurrentPage;
        }
    } else {
        Crawlable::onCellCrawled(cell);
        if (m_assembler->assembleCell(cell)) {
            m_score += m_columnWeightForCurrentPage;
        }
    }
    return true;
}

bool Crawler::onPageCrawled(const Page &page, int unused)
{
    if (isMasterCrawling()) {
        MasterCrawler::onPageCrawled(page, unused);
    } else {
        Crawlable::onPageCrawled(page, unused);
    }
    if (page.getType() == Page::Type::LeafTable) {
        m_columnWeightForCurrentPage = (double) 1.0 / page.getCellCount();
        ++m_parsedLeafPageCount;
    }
    return true;
}

} //namespace Repair

} //namespace WCDB
