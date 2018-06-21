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

#ifndef Backup_hpp
#define Backup_hpp

#include <WCDB/MasterCrawler.hpp>
#include <WCDB/Material.hpp>
#include <WCDB/SequenceCrawler.hpp>
#include <string>
#include <vector>

namespace WCDB {

namespace Repair {

class Backup : public Crawlable,
               public MasterCrawlerDelegate,
               public SequenceCrawlerDelegate {
#pragma mark - Initialize
public:
    Backup(const std::string &path);

protected:
    Pager m_pager;

#pragma mark - Backup
public:
    bool work(int maxWalFrame = std::numeric_limits<int>::max());

    const Error &getError() const;

    const Material &getMaterial() const;

protected:
    Material m_material;
    Material::Content &getOrCreateContent(const std::string &tableName);
    int m_height;
    std::vector<uint32_t> m_pagenos;

#pragma mark - Filter
public:
    typedef std::function<bool(const std::string &)> Filter;
    void filter(const Filter &tableShouldBeBackedUp);

protected:
    bool filter(const std::string &tableName);
    Filter m_filter;

#pragma mark - Crawlable
protected:
    void onCellCrawled(const Cell &cell) override;
    bool willCrawlPage(const Page &page, int height) override;
    void onCrawlerError() override;

#pragma mark - MasterCrawlerDelegate
protected:
    void onMasterCellCrawled(const Cell &cell, const Master *master) override;
    void onMasterCrawlerError() override;

    MasterCrawler m_masterCrawler;

#pragma mark - SequenceCrawlerDelegate
protected:
    void onSequenceCellCrawled(const Cell &cell,
                               const Sequence &sequence) override;
    void onSequenceCrawlerError() override;
};

} //namespace Repair

} //namespace WCDB

#endif /* Backup_hpp */
