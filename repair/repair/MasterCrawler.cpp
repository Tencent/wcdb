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

#include <WCDB/Cell.hpp>
#include <WCDB/MasterCrawler.hpp>

namespace WCDB {

namespace Repair {

MasterCrawler::MasterCrawler(const std::string &path, bool fatal)
    : Crawlable(path, fatal)
{
}

bool MasterCrawler::work()
{
    m_crawling = true;
    bool result = crawl(1);
    m_crawling = false;
    return result;
}

bool MasterCrawler::onCellCrawled(const Cell &cell)
{
    if (cell.getValueType(1) != Cell::Type::Text ||
        cell.getValueType(2) != Cell::Type::Text ||
        cell.getValueType(3) != Cell::Type::Integer32 ||
        cell.getValueType(4) != Cell::Type::Text) {
        markAsCorrupted();
        return !isFatal();
    }
    std::string name = cell.stringValue(1);
    std::string tblName = cell.stringValue(2);
    std::string sql = cell.stringValue(4);
    if (tblName.empty() || sql.empty() || name.empty()) {
        markAsCorrupted();
        return !isFatal();
    }

    if (name != tblName) {
        //skip index/view/trigger
        return true;
    }
    Master master;
    master.rootpage = cell.int32Value(3);
    master.tableName = std::move(name);
    master.sql = std::move(sql);
    m_masters.push_back(std::move(master));
    return true;
}

bool MasterCrawler::isMasterCrawling() const
{
    return m_crawling;
}

const std::list<Master> &MasterCrawler::getMasters() const
{
    return m_masters;
}

} //namespace Repair

} //namespace WCDB
