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
#include <WCDB/Page.hpp>
#include <WCDB/Sequence.hpp>
#include <WCDB/SequenceCrawler.hpp>

namespace WCDB {

namespace Repair {

void SequenceCrawlerDelegate::onSequencePageCrawled(const Page &page)
{
}

#pragma mark - Initialize
SequenceCrawler::SequenceCrawler(Pager &pager)
    : Crawlable(pager), m_delegate(nullptr)
{
}

#pragma mark - Sequence
bool SequenceCrawler::work(int rootpage, SequenceCrawlerDelegate *delegate)
{
    WCTInnerAssert(delegate != nullptr);
    m_delegate = delegate;
    bool result = crawl(rootpage);
    m_delegate = nullptr;
    return result;
}

#pragma mark - Crawlable
void SequenceCrawler::onCellCrawled(const Cell &cell)
{
    if (cell.getValueType(0) != Cell::Type::Text ||
        cell.getValueType(1) != Cell::Type::Integer) {
        markAsCorrupted(cell.getPage().number, "CellType");
        return;
    }
    std::string name = cell.stringValue(0);
    if (name.empty()) {
        markAsCorrupted(cell.getPage().number, "CellValue");
        return;
    }

    Sequence sequence;
    sequence.name = std::move(name);
    sequence.seq = cell.integerValue(1);
    m_delegate->onSequenceCellCrawled(cell, sequence);
}

void SequenceCrawler::onCrawlerError()
{
    m_delegate->onSequenceCrawlerError();
    stop();
}

bool SequenceCrawler::willCrawlPage(const Page &page, int height)
{
    m_delegate->onSequencePageCrawled(page);
    return true;
}

} //namespace Repair

} //namespace WCDB
