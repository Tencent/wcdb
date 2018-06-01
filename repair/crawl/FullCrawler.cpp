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
#include <WCDB/Page.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
FullCrawler::FullCrawler(const std::string &source) : Repairman(source)
{
}

#pragma mark - Repair
void FullCrawler::work()
{
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
    setPageWeight((double) leafTablePageCount / m_pager.getPageCount());

    if (!m_pager.initialize()) {
        markAsError();
        return;
    }

    markAsAssembling();
    MasterCrawler().work(this);
    markAsAssembled();
}

#pragma mark - Crawlable
void FullCrawler::onCellCrawled(const Cell &cell)
{
    assembleCell(cell);
}

#pragma mark - MasterCrawlerDelegate
void FullCrawler::onMasterPageCrawled(const Page &page)
{
    if (page.getType() == Page::Type::LeafTable) {
        markCellCount(page.getCellCount());
    }
}

void FullCrawler::onMasterCellCrawled(const Master *master)
{
    markCellAsCounted();
    if (master == nullptr) {
        //skip index/view/trigger
        return;
    }
    if (assembleTable(master->tableName, master->sql)) {
        crawl(master->rootpage);
    }
}

void FullCrawler::onMasterCrawlerError()
{
    tryUpgradeCrawlerError();
}

} //namespace Repair

} //namespace WCDB
