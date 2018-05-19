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
#include <list>
#include <map>

namespace WCDB {

namespace Repair {

struct Master {
    std::string tableName;
    std::string sql;
    std::list<std::string> associatedSQLs;
    int rootpage;
};

class MasterCrawler : public Crawlable {
#pragma mark - Initialize
public:
    MasterCrawler(const std::string &path, bool fatal);

#pragma mark - Master
public:
    bool work();

    const std::map<std::string, Master> &getMasters() const;

protected:
    std::map<std::string, Master> m_masters;
    bool isMasterCrawling() const;

private:
    bool m_crawling;

#pragma mark - Crawlable
protected:
    bool onCellCrawled(const Cell &cell) override;
};

} //namespace Repair

} //namespace WCDB

#endif /* MasterCrawler_hpp */
