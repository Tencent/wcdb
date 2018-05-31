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

#ifndef MasterCrawler_hpp
#define MasterCrawler_hpp

#include <WCDB/Crawlable.hpp>

namespace WCDB {

namespace Repair {

struct Master {
    std::string tableName;
    std::string sql;
    int rootpage;
};

class MasterCrawler;

class MasterCrawlerDelegate {
protected:
    friend class MasterCrawler;

    virtual Pager &getMasterPager() = 0;
    virtual void onMasterPageCrawled(const Page &page);
    virtual void onMasterCellCrawled(const Master *master) = 0;
    virtual void onMasterCrawlerError() = 0;
};

class MasterCrawler : public Crawlable {
#pragma mark - Initialize
public:
    MasterCrawler();

#pragma mark - Master
public:
    bool work(MasterCrawlerDelegate *delegate);

protected:
    MasterCrawlerDelegate *m_delegate;

#pragma mark - Crawlable
protected:
    Pager &getPager() override;
    void onCellCrawled(const Cell &cell) override;
    bool willCrawlPage(const Page &page, int height) override;
    void onCrawlerError() override;
};

} //namespace Repair

} //namespace WCDB

#endif /* MasterCrawler_hpp */
