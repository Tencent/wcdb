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
#include <WCDB/Cell.hpp>
#include <WCDB/Crawlable.hpp>
#include <WCDB/Page.hpp>
#include <WCDB/Pager.hpp>
#include <WCDB/ThreadedErrors.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
Crawlable::Crawlable(const std::string &path, bool fatal)
    : m_pager(path), m_fatal(fatal)
{
}

#pragma mark - Error
bool Crawlable::isFatal() const
{
    return m_fatal;
}

void Crawlable::markAsCorrupted()
{
    m_pager.markAsCorrupted();
    markAsError();
}

void Crawlable::markAsError()
{
    tryUpgradeErrorWithThreadedError();
}

bool Crawlable::crawl(int rootpageno)
{
    std::set<int> crawledInteriorPages;
    safeCrawl(rootpageno, crawledInteriorPages, 1);
    return getCriticalError().isOK();
}

void Crawlable::safeCrawl(int rootpageno,
                          std::set<int> &crawledInteriorPages,
                          int height)
{
    Page rootpage(rootpageno, m_pager);
    if (!rootpage.initialize()) {
        markAsError();
        return;
    }
    if (!onPageCrawled(rootpage, height)) {
        return;
    }
    switch (rootpage.getType()) {
        case Page::Type::InteriorTable:
            if (crawledInteriorPages.find(rootpageno) !=
                crawledInteriorPages.end()) {
                //avoid dead loop
                markAsCorrupted();
                return;
            }
            crawledInteriorPages.insert(rootpageno);
            for (int i = 0; i < rootpage.getSubPageCount(); ++i) {
                if (isFatal() && !getCriticalError().isOK()) {
                    break;
                }
                auto pair = rootpage.getSubPageno(i);
                if (pair.first) {
                    safeCrawl(pair.second, crawledInteriorPages, height + 1);
                } else {
                    markAsCorrupted();
                }
            }
            break;
        case Page::Type::LeafTable:
            for (int i = 0; i < rootpage.getCellCount(); ++i) {
                if (isFatal() && !getCriticalError().isOK()) {
                    break;
                }
                Cell cell = rootpage.getCell(i);
                if (!cell.initialize()) {
                    markAsError();
                    continue;
                }
                if (!onCellCrawled(cell)) {
                    break;
                }
            }
            break;
        default:
            break;
    }
}

bool Crawlable::onCellCrawled(const Cell &cell)
{
    return true;
}

bool Crawlable::onPageCrawled(const Page &page, int height)
{
    return true;
}

} //namespace Repair

} //namespace WCDB
