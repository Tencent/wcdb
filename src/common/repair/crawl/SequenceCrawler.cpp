//
// Created by sanhuazhang on 2018/05/31
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

#include "SequenceCrawler.hpp"
#include "Assertion.hpp"
#include "Cell.hpp"
#include "Page.hpp"
#include "SequenceItem.hpp"
#include "StringView.hpp"

namespace WCDB {

namespace Repair {

SequenceCrawlerDelegate::~SequenceCrawlerDelegate() = default;

#pragma mark - Initialize
SequenceCrawler::SequenceCrawler() : Crawlable(), m_delegate(nullptr)
{
}

SequenceCrawler::~SequenceCrawler() = default;

#pragma mark - Sequence
bool SequenceCrawler::work(int rootpage, SequenceCrawlerDelegate *delegate)
{
    WCTAssert(delegate != nullptr);
    m_delegate = delegate;
    bool result = crawl(rootpage);
    m_delegate = nullptr;
    return result;
}

#pragma mark - Crawlable
void SequenceCrawler::onCellCrawled(const Cell &cell)
{
    if (cell.getValueType(0) != Cell::Type::Text) {
        markAsCorrupted(cell.getPage().number,
                        StringView::formatted("Name with type: %d in Sequence is Text.",
                                              cell.getValueType(0)));
        return;
    }
    if (cell.getValueType(1) != Cell::Type::Integer) {
        markAsCorrupted(cell.getPage().number,
                        StringView::formatted("Seq with type: %d in Sequence is Integer.",
                                              cell.getValueType(1)));
        return;
    }
    StringView name = cell.stringValue(0);
    if (name.empty()) {
        markAsCorrupted(cell.getPage().number, "Name in Sequence is empty.");
        return;
    }

    SequenceItem sequence;
    sequence.name = std::move(name);
    sequence.seq = cell.integerValue(1);
    m_delegate->onSequenceCellCrawled(cell, sequence);
}

void SequenceCrawler::onCrawlerError()
{
    m_delegate->onSequenceCrawlerError();
    suspend();
}

bool SequenceCrawler::willCrawlPage(const Page &page, int height)
{
    WCDB_UNUSED(height);
    m_delegate->onSequencePageCrawled(page);
    return true;
}

} //namespace Repair

} //namespace WCDB
