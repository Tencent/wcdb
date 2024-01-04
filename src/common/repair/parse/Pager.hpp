//
// Created by sanhuazhang on 2018/05/19
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

#include "ErrorProne.hpp"
#include "HighWater.hpp"
#include "Initializeable.hpp"
#include "PageBasedFileHandle.hpp"
#include "WCDBError.hpp"
#include "Wal.hpp"

namespace WCDB {

class Data;

namespace Repair {

class Pager final : public ErrorProne, public Initializeable {
#pragma mark - Initialize
public:
    Pager(const UnsafeStringView& path);
    ~Pager() override;

    void setPageSize(int pageSize);
    void setReservedBytes(int reservedBytes);
    void setCipherContext(void* ctx);

    const StringView& getPath() const;

protected:
    PageBasedFileHandle m_fileHandle;
    void* m_pCodec;
    friend class PagerRelated;

#pragma mark - Page
public:
    int getNumberOfPages() const;
    UnsafeData acquirePageData(int number);
    UnsafeData acquirePageData(int number, offset_t offset, size_t size);

    int getUsableSize() const;
    int getPageSize() const;
    int getReservedBytes() const;
    int getSchemaCookie() const;

protected:
    UnsafeData acquireHeader();
    int m_pageSize;
    int m_reservedBytes;
    int m_numberOfPages;
    size_t m_fileSize;
    int m_schemaCookie;

#pragma mark - Wal
public:
    void setWalImportance(bool flag);
    int getNumberOfWalFrames() const;

    void setNBackFill(int nbackfill);
    int getNBackFill() const;

    typedef Wal::Salt Salt;
    const Salt& getWalSalt() const;
    void setWalSalt(const Salt& salt);

    int getMaxFrame() const;
    int getDisposedWalPages() const;
    void disposeWal();

    void setWalSkipped();
    bool loadWal();
    bool containPageInWal(uint32_t pageno);

protected:
    Wal m_wal;
    bool m_walImportance;
    bool m_skipWal;

#pragma mark - Error
public:
    void markAsCorrupted(int page, const UnsafeStringView& message);

    void markAsError(Error::Code code);

#pragma mark - Initializeable
protected:
    bool doInitialize() override final;

#pragma mark - Cache
protected:
    static constexpr const size_t maxAllowedCacheMemory = 16 * 1024 * 1024;
    class Cache final : public LRUCache<uint32_t, UnsafeData> {
    public:
        Cache(size_t maxAllowedMemory);
        ~Cache() override;

        void insert(uint32_t pageNum, const UnsafeData& data);

    protected:
        bool shouldPurge() const override final;
        void willPurge(const uint32_t& pageNum, const UnsafeData& data) override final;
        size_t m_maxAllowedMemory;
        size_t m_currentUsedMemery;
    };
    void tryPurgeCache();
    Cache m_cache;
    SharedHighWater m_highWater;
};

} //namespace Repair

} //namespace WCDB
