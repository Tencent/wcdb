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
    Crawlable(const std::string &path, bool fatal);

protected:
    Pager m_pager;
    bool m_fatal;

#pragma mark - Error
protected:
    const Error &getError() const;

protected:
    bool isFatalError() const;

#pragma mark - Crawlable
protected:
    bool crawl(int rootpageno);

    //return false to indicate a manual break
    virtual bool onCellCrawled(const Cell &cell);
    virtual bool onPageCrawled(const Page &page, int height);

private:
    void
    safeCrawl(int rootpageno, std::set<int> &crawledInteriorPages, int height);
};

} //namespace Repair

} //namespace WCDB

#endif /* Crawlable_hpp */
