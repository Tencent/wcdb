//
// Created by sanhuazhang on 2018/06/05
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

#pragma once

#include "Cipher.hpp"
#include "IncrementalMaterial.hpp"
#include "MasterCrawler.hpp"
#include "Material.hpp"
#include "SequenceCrawler.hpp"
#include <memory>
#include <vector>

namespace WCDB {

namespace Repair {

class BackupBaseDelegate {
public:
    virtual ~BackupBaseDelegate() = 0;
    virtual void setBackupPath(const UnsafeStringView &path) = 0;
    virtual const StringView &getBackupPath() const = 0;

    virtual const Error &getBackupError() const = 0;
    virtual void finishBackup() = 0;
};

class BackupSharedDelegate : public BackupBaseDelegate {
public:
    virtual ~BackupSharedDelegate() = 0;

    virtual bool acquireBackupSharedLock() = 0;
    virtual bool releaseBackupSharedLock() = 0;
};

class BackupExclusiveDelegate : public BackupBaseDelegate {
public:
    BackupExclusiveDelegate();
    virtual ~BackupExclusiveDelegate() = 0;

    virtual bool acquireBackupExclusiveLock() = 0;
    virtual bool releaseBackupExclusiveLock() = 0;
    virtual bool backupSuspended() const = 0;

private:
    std::atomic<bool> m_suspend;
};

class BackupDelegateHolder {
public:
    BackupDelegateHolder();
    virtual ~BackupDelegateHolder() = 0;
    void setBackupSharedDelegate(BackupSharedDelegate *delegate);
    void setBackupExclusiveDelegate(BackupExclusiveDelegate *delegate);

protected:
    bool isDelegateValid() const;
    BackupSharedDelegate *m_sharedDelegate;
    BackupExclusiveDelegate *m_exclusiveDelegate;
};

class Backup final : public Crawlable,
                     public MasterCrawlerDelegate,
                     public SequenceCrawlerDelegate,
                     public CipherDelegateHolder,
                     public BackupDelegateHolder,
                     public ErrorProne {
#pragma mark - Initialize
public:
    Backup(const UnsafeStringView &path);
    ~Backup() override;

protected:
    Pager m_pager;

#pragma mark - Backup
public:
    bool work(SharedIncrementalMaterial material);

    const Material &getMaterial() const;
    SharedIncrementalMaterial getIncrementalMaterial();

protected:
    Optional<bool> tryLoadLatestMaterial(SharedIncrementalMaterial incrementalMaterial);
    bool fullBackup();
    bool incrementalBackup();
    bool loadWal();
    void updateMaterial(bool isIncremental);

    Material m_material;
    SharedIncrementalMaterial m_incrementalMaterial;
    Material::Content &getOrCreateContent(const UnsafeStringView &tableName);

    typedef Material::VerifiedPages VerifiedPages;
    VerifiedPages m_verifiedPagenos;

    typedef IncrementalMaterial::Pages IncrementalPages;
    IncrementalPages *m_verifyingPagenos;
    std::vector<bool> m_unchangedLeaves;
    int m_unchangedLeavesCount;

#pragma mark - Filter
public:
    typedef std::function<bool(const UnsafeStringView &table)> Filter;
    void filter(const Filter &tableShouldBeBackedUp);

protected:
    bool filter(const UnsafeStringView &tableName);
    Filter m_filter;

#pragma mark - Crawlable
protected:
    bool canCrawlPage(uint32_t pageno) override final;
    void onCellCrawled(const Cell &cell) override final;
    bool willCrawlPage(const Page &page, int height) override final;
    void onCrawlerError() override final;

#pragma mark - MasterCrawlerDelegate
protected:
    void onMasterPageCrawled(const Page &page) override final;
    void onMasterCellCrawled(const Cell &cell, const MasterItem &master) override final;
    void onMasterCrawlerError() override final;

    MasterCrawler m_masterCrawler;

#pragma mark - SequenceCrawlerDelegate
protected:
    void onSequencePageCrawled(const Page &page) override final;
    void onSequenceCellCrawled(const Cell &cell, const SequenceItem &sequence) override final;
    void onSequenceCrawlerError() override final;
};

} //namespace Repair

} //namespace WCDB
