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

#ifndef __WCDB_BACKUP_HPP
#define __WCDB_BACKUP_HPP

#include <WCDB/Locker.hpp>
#include <WCDB/MasterCrawler.hpp>
#include <WCDB/Material.hpp>
#include <WCDB/SequenceCrawler.hpp>
#include <WCDB/String.hpp>
#include <vector>

namespace WCDB {

namespace Repair {

class Backup final : public Crawlable,
                     public MasterCrawlerDelegate,
                     public SequenceCrawlerDelegate,
                     public LockerHolder,
                     public ErrorProne {
#pragma mark - Initialize
public:
    Backup(const String &path);

protected:
    Pager m_pager;

#pragma mark - Backup
public:
    bool work();

    const Material &getMaterial() const;

protected:
    Material m_material;
    Material::Content &getOrCreateContent(const String &tableName);
    std::map<uint32_t, uint32_t> m_verifiedPagenos;

#pragma mark - Filter
public:
    typedef std::function<bool(const String &table)> Filter;
    void filter(const Filter &tableShouldBeBackedUp);

protected:
    bool filter(const String &tableName);
    Filter m_filter;

#pragma mark - Crawlable
protected:
    void onCellCrawled(const Cell &cell) override final;
    bool willCrawlPage(const Page &page, int height) override final;
    void onCrawlerError() override final;

#pragma mark - MasterCrawlerDelegate
protected:
    void onMasterCellCrawled(const Cell &cell, const Master &master) override final;
    void onMasterCrawlerError() override final;

    MasterCrawler m_masterCrawler;

#pragma mark - SequenceCrawlerDelegate
protected:
    void onSequenceCellCrawled(const Cell &cell, const Sequence &sequence) override final;
    void onSequenceCrawlerError() override final;
};

} //namespace Repair

} //namespace WCDB

#endif /* __WCDB_BACKUP_HPP */
