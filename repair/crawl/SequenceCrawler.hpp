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

#ifndef _WCDB_SEQUENCECRAWLER_HPP
#define _WCDB_SEQUENCECRAWLER_HPP

#include <WCDB/Crawlable.hpp>

namespace WCDB {

namespace Repair {

class Sequence;
class SequenceCrawler;

class SequenceCrawlerDelegate {
protected:
    friend class SequenceCrawler;

    virtual void onSequencePageCrawled(const Page &page);
    virtual void onSequenceCellCrawled(const Cell &cell, const Sequence &sequence) = 0;
    virtual void onSequenceCrawlerError() = 0;
};

class SequenceCrawler : public Crawlable {
#pragma mark - Initialize
public:
    SequenceCrawler(Pager &pager);

#pragma mark - Sequence
public:
    bool work(int rootpage, SequenceCrawlerDelegate *delegate);

protected:
    SequenceCrawlerDelegate *m_delegate;

#pragma mark - Crawlable
protected:
    void onCellCrawled(const Cell &cell) override;
    void onCrawlerError() override;
    bool willCrawlPage(const Page &page, int height) override;
};

} //namespace Repair

} //namespace WCDB

#endif /* _WCDB_SEQUENCECRAWLER_HPP */
