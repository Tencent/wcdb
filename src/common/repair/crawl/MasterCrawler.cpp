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

#include "MasterCrawler.hpp"
#include "Assertion.hpp"
#include "Cell.hpp"
#include "MasterItem.hpp"
#include "Page.hpp"
#include "StringView.hpp"

namespace WCDB {

namespace Repair {

MasterCrawlerDelegate::~MasterCrawlerDelegate() = default;

MasterCrawler::MasterCrawler() : Crawlable(), m_delegate(nullptr)
{
}

MasterCrawler::~MasterCrawler() = default;

bool MasterCrawler::work(MasterCrawlerDelegate *delegate)
{
    WCTAssert(delegate != nullptr);
    m_delegate = delegate;
    bool result = crawl(1);
    m_delegate = nullptr;
    return result;
}

void MasterCrawler::onCellCrawled(const Cell &cell)
{
    MasterItem master;
    if (cell.getValueType(0) == Cell::Type::Text) {
        master.type = cell.stringValue(0);
    }
    if (cell.getValueType(1) == Cell::Type::Text) {
        master.name = cell.stringValue(1);
    }
    if (cell.getValueType(2) == Cell::Type::Text) {
        master.tableName = cell.stringValue(2);
    }
    if (cell.getValueType(3) == Cell::Type::Integer) {
        master.rootpage = (int) cell.integerValue(3);
        if (master.rootpage <= 0) {
            markAsCorrupted(cell.getPage().number,
                            StringView::formatted("Root page: %d in Master is less than or equal to 0.",
                                                  master.rootpage));
            return;
        }
    }
    if (cell.getValueType(4) == Cell::Type::Text) {
        master.sql = cell.stringValue(4);
    }
    m_delegate->onMasterCellCrawled(cell, master);
}

bool MasterCrawler::willCrawlPage(const Page &page, int)
{
    m_delegate->onMasterPageCrawled(page);
    return true;
}

void MasterCrawler::onCrawlerError()
{
    m_delegate->onMasterCrawlerError();
}

} //namespace Repair

} //namespace WCDB
