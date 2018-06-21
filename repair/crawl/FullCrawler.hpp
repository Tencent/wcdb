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

#ifndef FullCrawler_hpp
#define FullCrawler_hpp

#include <WCDB/Assembler.hpp>
#include <WCDB/Crawlable.hpp>
#include <WCDB/MasterCrawler.hpp>
#include <WCDB/Repairman.hpp>
#include <WCDB/SequenceCrawler.hpp>
#include <map>

namespace WCDB {

namespace Repair {

class FullCrawler : public Repairman,
                    public MasterCrawlerDelegate,
                    public SequenceCrawlerDelegate {
#pragma mark - Initialize
public:
    FullCrawler(const std::string &source);

#pragma mark - Repair
public:
    bool work();

#pragma mark - Crawlable
protected:
    void onCellCrawled(const Cell &cell) override;
    bool willCrawlPage(const Page &, int) override;

#pragma mark - Error
protected:
    void onErrorCritical() override;

#pragma mark - MasterCrawlerDelegate
protected:
    void onMasterPageCrawled(const Page &page) override;
    void onMasterCellCrawled(const Cell &cell, const Master *master) override;
    void onMasterCrawlerError() override;

    MasterCrawler m_masterCrawler;

#pragma mark - SequenceCrawlerDelegate
protected:
    void onSequencePageCrawled(const Page &page) override;
    void onSequenceCellCrawled(const Cell &cell,
                               const Sequence &sequence) override;
    void onSequenceCrawlerError() override;

    SequenceCrawler m_sequenceCrawler;
};

} //namespace Repair

} //namespace WCDB

#endif /* FullCrawler_hpp */
