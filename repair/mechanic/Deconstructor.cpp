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
    : MasterCrawler(path, true), m_height(-1), m_sequenceCrawling(false)
{
}

bool Deconstructor::work()
{
    if (!m_pager.initialize()) {
        return false;
    }

    m_material.info.pageSize = m_pager.getPageSize();
    m_material.info.reservedBytes = m_pager.getReservedBytes();

    if (!MasterCrawler::work()) {
        return false;
    }

    for (auto &master : m_masters) {
        m_sequenceCrawling = master.tableName == "sqlite_sequence";
        if (!m_sequenceCrawling && !filter(master.tableName)) {
            continue;
        }

        m_height = -1;
        if (!crawl(master.rootpage)) {
            return false;
        }

        if (!m_sequenceCrawling) {
            m_content.sql = std::move(master.sql);
            m_material.contents[master.tableName] = std::move(m_content);
        }
        m_sequenceCrawling = false;
    }

    for (const auto &sequence : m_sequences) {
        auto iter = m_material.contents.find(sequence.first);
        if (iter != m_material.contents.end()) {
            iter->second.sequence = sequence.second;
        }
    }

    return true;
}

void Deconstructor::filter(const Filter &shouldTableDeconstructed)
{
    m_filter = shouldTableDeconstructed;
}

bool Deconstructor::filter(const std::string &tableName)
{
    if (!m_filter) {
        return true;
    }
    return m_filter(tableName);
}

const Material &Deconstructor::getMaterial() const
{
    return m_material;
}

#pragma mark - Crawlable
bool Deconstructor::onCellCrawled(const Cell &cell)
{
    if (isMasterCrawling()) {
        return MasterCrawler::onCellCrawled(cell);
    } else if (m_sequenceCrawling) {
        if (cell.getValueType(1) != Cell::Type::Text ||
            //TODO accept int32 and int64
            cell.getValueType(2) != Cell::Type::Integer64) {
            markAsCorrupted();
            return false;
        }
        std::string name = cell.stringValue(1);
        if (filter(name)) {
            m_sequences[name] = cell.int64Value(2);
        }
        return true;
    }
    WCTInnerFatalError();
    return false;
}

bool Deconstructor::onPageCrawled(const Page &page, int height)
{
    if (isMasterCrawling()) {
        return MasterCrawler::onPageCrawled(page, height);
    } else if (m_sequenceCrawling) {
        return true;
    } else {
        switch (page.getType()) {
            case Page::Type::LeafTable:
                m_height = height;
                m_content.pagenos.push_back(page.number);
                return false;
            case Page::Type::InteriorTable:
                if (m_height > 0 && height == m_height - 1) {
                    auto &pagenos = m_content.pagenos;
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
    }
    return false;
}

} //namespace Repair

} //namespace WCDB
