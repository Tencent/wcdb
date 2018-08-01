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
#include <WCDB/String.hpp>
#include <WCDB/ThreadedErrors.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
Crawlable::Crawlable(Pager &pager)
: m_associatedPager(pager), m_stop(false), m_isCrawling(false)
{
}

void Crawlable::stop()
{
    m_stop = true;
}

#pragma mark - Error
const Error &Crawlable::getCrawlError() const
{
    return m_associatedPager.getError();
}

void Crawlable::markAsCorrupted(int page, const std::string &diagnostic)
{
    m_associatedPager.markAsCorrupted(page, diagnostic);
    markAsError();
}

void Crawlable::markAsError()
{
    onCrawlerError();
}

bool Crawlable::crawl(int rootpageno)
{
    WCTInnerAssert(!m_isCrawling);
    m_isCrawling = true;
    m_stop = false;
    std::set<int> crawledInteriorPages;
    safeCrawl(rootpageno, crawledInteriorPages, 1);
    m_isCrawling = false;
    return m_associatedPager.getError().isOK();
}

void Crawlable::safeCrawl(int rootpageno, std::set<int> &crawledInteriorPages, int height)
{
    Page rootpage(rootpageno, &m_associatedPager);
    if (!rootpage.initialize()) {
        markAsError();
        return;
    }
    if (!willCrawlPage(rootpage, height)) {
        return;
    }
    switch (rootpage.getType()) {
    case Page::Type::InteriorTable:
        if (crawledInteriorPages.find(rootpageno) != crawledInteriorPages.end()) {
            //avoid dead loop
            markAsCorrupted(rootpageno, "Page is already crawled.");
            return;
        }
        crawledInteriorPages.emplace(rootpageno);
        for (int i = 0; i < rootpage.getSubPageCount(); ++i) {
            if (m_stop) {
                break;
            }
            int pageno = rootpage.getSubPageno(i);
            safeCrawl(pageno, crawledInteriorPages, height + 1);
        }
        break;
    case Page::Type::LeafTable:
        for (int i = 0; i < rootpage.getCellCount(); ++i) {
            if (m_stop) {
                break;
            }
            Cell cell = rootpage.getCell(i);
            if (cell.initialize()) {
                onCellCrawled(cell);
            } else {
                markAsError();
            }
        }
        break;
    default:
        markAsCorrupted(
        rootpageno, String::formatted("Unexpected page type: %d", rootpage.getType()));
        break;
    }
}

void Crawlable::onCellCrawled(const Cell &cell)
{
}

bool Crawlable::willCrawlPage(const Page &page, int height)
{
    return true;
}

void Crawlable::onCrawlerError()
{
}

} //namespace Repair

} //namespace WCDB
