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

#ifndef Crawlable_hpp
#define Crawlable_hpp

#include <WCDB/Pager.hpp>
#include <set>

namespace WCDB {

namespace Repair {

class Cell;
class Page;
class Pager;

class Crawlable {
#pragma mark - Initialize
public:
    Crawlable(Pager &pager);

private:
    Pager &m_associatedPager;

#pragma mark - Stoppable
public:
    void stop();

protected:
    bool m_stop;

#pragma mark - Error
public:
    const Error &getCrawlError() const;

protected:
    void markAsCorrupted(int page, const std::string &element);
    void markAsError();

#pragma mark - Crawlable
protected:
    bool crawl(int rootpageno);

    virtual void onCellCrawled(const Cell &cell);
    //return false to skip current page
    virtual bool willCrawlPage(const Page &page, int height);
    virtual void onCrawlerError();

private:
    void
    safeCrawl(int rootpageno, std::set<int> &crawledInteriorPages, int height);
    bool m_isCrawling;
};

} //namespace Repair

} //namespace WCDB

#endif /* Crawlable_hpp */
