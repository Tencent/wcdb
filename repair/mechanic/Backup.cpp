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
#include <WCDB/Backup.hpp>
#include <WCDB/Cell.hpp>
#include <WCDB/Page.hpp>

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
Backup::Backup(const std::string &path)
    : Crawlable(), m_pager(path), m_height(-1)
{
}

bool Backup::work()
{
    if (!m_pager.initialize()) {
        return false;
    }

    m_material.info.pageSize = m_pager.getPageSize();
    m_material.info.reservedBytes = m_pager.getReservedBytes();

    m_masterCrawler.work(this);
    return isError();
}

void Backup::filter(const Filter &shouldTableDeconstructed)
{
    m_filter = shouldTableDeconstructed;
}

bool Backup::filter(const std::string &tableName)
{
    if (!m_filter) {
        return true;
    }
    return m_filter(tableName);
}

const Material &Backup::getMaterial() const
{
    return m_material;
}

Material::Content &Backup::getOrCreateContent(const std::string &tableName)
{
    auto &contents = m_material.contents;
    auto iter = contents.find(tableName);
    if (iter == contents.end()) {
        iter = contents.insert({tableName, Material::Content()}).first;
    }
    return iter->second;
}

#pragma mark - Crawlable
void Backup::onCellCrawled(const Cell &cell)
{
    WCTInnerFatalError();
}

bool Backup::willCrawlPage(const Page &page, int height)
{
    switch (page.getType()) {
        case Page::Type::LeafTable:
            m_height = height;
            m_pagenos.push_back(page.number);
            return false;
        case Page::Type::InteriorTable:
            if (m_height > 0 && height == m_height - 1) {
                //avoid iterating the leaf table
                for (int i = 0; i < page.getSubPageCount(); ++i) {
                    auto pair = page.getSubPageno(i);
                    if (!pair.first) {
                        m_pager.markAsCorrupted();
                        break;
                    }
                    m_pagenos.push_back(page.number);
                }
                return false;
            }
            return true;
        default:
            break;
    }
    return false;
}

Pager &Backup::getPager()
{
    return m_pager;
}

void Backup::onCrawlerError()
{
    m_masterCrawler.stop();
}

#pragma mark - MasterCrawlerDelegate
void Backup::onMasterCellCrawled(const Master *master)
{
    if (master == nullptr || !filter(master->tableName)) {
        //skip index/view/trigger and filted table
        return;
    }
    if (master->tableName == SequenceCrawler::name()) {
        SequenceCrawler().work(master->rootpage, this);
    } else if (filter(master->tableName)) {
        m_height = -1;
        if (!crawl(master->rootpage)) {
            return;
        }

        Material::Content &content = getOrCreateContent(master->tableName);
        content.pagenos = std::move(m_pagenos);
        content.sql = std::move(master->sql);
    }
}

void Backup::onMasterCrawlerError()
{
    markAsError();
}

Pager &Backup::getMasterPager()
{
    return m_pager;
}

#pragma mark - SequenceCrawlerDelegate
void Backup::onSequenceCellCrawled(const Sequence &sequence)
{
    if (filter(sequence.name)) {
        Material::Content &content = getOrCreateContent(sequence.name);
        //the columns in sqlite_sequence are not unique.
        content.sequence = std::max(content.sequence, sequence.seq);
    }
}

Pager &Backup::getSequencePager()
{
    return m_pager;
}

void Backup::onSequenceCrawlerError()
{
    markAsError();
}

} //namespace Repair

} //namespace WCDB
