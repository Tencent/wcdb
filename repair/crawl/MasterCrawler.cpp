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
#include <WCDB/MasterCrawler.hpp>

namespace WCDB {

namespace Repair {

void MasterCrawlerDelegate::onMasterPageCrawled(const Page &page)
{
}

MasterCrawler::MasterCrawler() : Crawlable(), m_delegate(nullptr)
{
}

bool MasterCrawler::work(MasterCrawlerDelegate *delegate)
{
    WCTInnerAssert(delegate != nullptr);
    m_delegate = delegate;
    bool result = crawl(1);
    m_delegate = nullptr;
    return result;
}

void MasterCrawler::onCellCrawled(const Cell &cell)
{
    if (cell.getValueType(1) != Cell::Type::Text ||
        cell.getValueType(2) != Cell::Type::Text ||
        cell.getValueType(3) != Cell::Type::Integer ||
        cell.getValueType(4) != Cell::Type::Text) {
        markAsCorrupted();
        return;
    }
    std::string name = cell.stringValue(1);
    std::string tblName = cell.stringValue(2);
    std::string sql = cell.stringValue(4);
    if (tblName.empty() || sql.empty() || name.empty()) {
        markAsCorrupted();
        return;
    }

    if (name != tblName) {
        //skip index/view/trigger
        m_delegate->onMasterCellCrawled(nullptr);
        return;
    }
    Master master;
    master.rootpage = (int) cell.integerValue(3);
    master.tableName = std::move(name);
    master.sql = std::move(sql);
    m_delegate->onMasterCellCrawled(&master);
}

bool MasterCrawler::willCrawlPage(const Page &page, int unused)
{
    m_delegate->onMasterPageCrawled(page);
    return true;
}

void MasterCrawler::onCrawlerError()
{
    m_delegate->onMasterCrawlerError();
}

Pager &MasterCrawler::getPager()
{
    return m_delegate->getMasterPager();
}

} //namespace Repair

} //namespace WCDB
