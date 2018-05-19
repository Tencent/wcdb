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
#include <WCDB/Mechanic.hpp>
#include <WCDB/Page.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
Mechanic::Mechanic(const std::string &path)
    : Crawlable(path, false)
    , m_pageWeight(0)
    , m_columnWeightForCurrentPageInAllPages(0)
{
}

#pragma mark - Materaial
void Mechanic::setMateraial(const Materaial &materaial)
{
    m_materaial = materaial;
}

void Mechanic::setMateraial(Materaial &&materaial)
{
    m_materaial = std::move(materaial);
}

#pragma mark - Mechanic
void Mechanic::work()
{
    WCTInnerAssert(m_assembler != nullptr);
    if (!m_pager.initialize()) {
        return;
    }

    int pageCount = 0;
    for (const auto &content : m_materaial.contents) {
        pageCount += content.pagenos.size();
    }
    m_pageWeight = (double) 1.0 / pageCount;

    for (const auto &content : m_materaial.contents) {
        m_assembler->markAsAssembling(content.tableName);
        if (!m_assembler->assembleTable(content.sql) ||
            !m_assembler->assembleTableAssociated(content.associatedSQLs)) {
            for (const auto &pageno : content.pagenos) {
                crawl(pageno);
            }
        }
        m_assembler->markAsAssembled();
    }
}

#pragma mark - Crawlable
bool Mechanic::onCellCrawled(const Cell &cell)
{
    Crawlable::onCellCrawled(cell);
    if (m_assembler->assembleCell(cell)) {
        m_score += m_columnWeightForCurrentPageInAllPages;
    }
    return true;
}

bool Mechanic::onPageCrawled(const Page &page, int unused)
{
    Crawlable::onPageCrawled(page, unused);
    if (page.getType() == Page::Type::LeafTable) {
        m_columnWeightForCurrentPageInAllPages =
            (double) 1.0 / page.getCellCount() * m_pageWeight;
        return true;
    }
    m_pager.markAsCorrupted();
    return false;
}

} //namespace Repair

} //namespace WCDB
