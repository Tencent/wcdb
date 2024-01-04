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

#pragma once

#include "Crawlable.hpp"

namespace WCDB {

namespace Repair {

struct SequenceItem;
class SequenceCrawler;

class SequenceCrawlerDelegate {
public:
    virtual ~SequenceCrawlerDelegate() = 0;

protected:
    friend class SequenceCrawler;

    virtual void onSequencePageCrawled(const Page &page) = 0;
    virtual void onSequenceCellCrawled(const Cell &cell, const SequenceItem &sequence) = 0;
    virtual void onSequenceCrawlerError() = 0;
};

class SequenceCrawler final : public Crawlable {
#pragma mark - Initialize
public:
    SequenceCrawler();
    ~SequenceCrawler() override;

#pragma mark - Sequence
public:
    bool work(int rootpage, SequenceCrawlerDelegate *delegate);

protected:
    SequenceCrawlerDelegate *m_delegate;

#pragma mark - Crawlable
protected:
    void onCellCrawled(const Cell &cell) override final;
    void onCrawlerError() override final;
    bool willCrawlPage(const Page &page, int height) override final;
};

} //namespace Repair

} //namespace WCDB
