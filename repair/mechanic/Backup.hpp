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

#include <WCDB/MasterCrawler.hpp>
#include <WCDB/Material.hpp>
#include <WCDB/SequenceCrawler.hpp>
#include <WCDB/StringView.hpp>
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

    void suspendBackup(bool suspend);
    bool backupSuspended() const;

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
                     public BackupDelegateHolder,
                     public ErrorProne {
#pragma mark - Initialize
public:
    Backup(const UnsafeStringView &path);
    ~Backup() override final;

protected:
    Pager m_pager;

#pragma mark - Backup
public:
    bool work();

    const Material &getMaterial() const;

protected:
    Material m_material;
    Material::Content &getOrCreateContent(const UnsafeStringView &tableName);
    std::map<uint32_t, uint32_t> m_verifiedPagenos;

#pragma mark - Filter
public:
    typedef std::function<bool(const UnsafeStringView &table)> Filter;
    void filter(const Filter &tableShouldBeBackedUp);

protected:
    bool filter(const UnsafeStringView &tableName);
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
