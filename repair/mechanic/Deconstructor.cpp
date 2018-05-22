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
#include <WCDB/Deconstructor.hpp>
#include <WCDB/Page.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
Deconstructor::Deconstructor(const std::string &path)
    : MasterCrawler(path, true), m_height(-1)
{
}

bool Deconstructor::work()
{
    if (!m_pager.initialize()) {
        return false;
    }

    if (!MasterCrawler::work()) {
        return false;
    }

    for (auto &element : m_masters) {
        Master &master = element.second;
        if (m_filter && !m_filter(master.tableName)) {
            continue;
        }

        Materaial::Content content;
        content.tableName = std::move(master.tableName);
        content.sql = std::move(master.sql);
        content.associatedSQLs = std::move(master.associatedSQLs);
        m_materaial.contents.push_back(std::move(content));

        m_height = -1;
        if (!crawl(master.rootpage)) {
            return false;
        }
    }

    return true;
}

void Deconstructor::filter(const Filter &shouldTableDeconstructed)
{
    m_filter = shouldTableDeconstructed;
}

const Materaial &Deconstructor::getMaterail() const
{
    return m_materaial;
}

#pragma mark - Crawlable
bool Deconstructor::onCellCrawled(const Cell &cell)
{
    if (isMasterCrawling()) {
        return MasterCrawler::onCellCrawled(cell);
    }
    WCTInnerFatalError();
    return false;
}

bool Deconstructor::onPageCrawled(const Page &page, int height)
{
    if (isMasterCrawling()) {
        return MasterCrawler::onPageCrawled(page, height);
    }
    switch (page.getType()) {
        case Page::Type::LeafTable:
            m_height = height;
            m_materaial.contents.back().pagenos.push_back(page.number);
            return false;
        case Page::Type::InteriorTable:
            if (m_height > 0 && height == m_height - 1) {
                auto &pagenos = m_materaial.contents.back().pagenos;
                for (int i = 0; i < page.getSubPageCount(); ++i) {
                    auto pair = page.getSubPageno(i);
                    if (!pair.first) {
                        m_pager.markAsCorrupted();
                        break;
                    }
                    pagenos.push_back(page.number);
                }
                return false;
            }
            return true;
        default:
            break;
    }
    return false;
}

} //namespace Repair

} //namespace WCDB
